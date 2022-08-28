/*************************************************************
 * File: main.cpp
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 09.07.2022
 * Edited  28.08.2022
 * Tauno Erik 2022
 * 
 * Status Colours
 * DARK_RED  : Bell active,
 * RED       : Camera active
 * ORANGE    : Data sending over serial,
 * YELLOW    : Mobile detected
 * GREEN     : Data sending finished,
 * DARK_GREEN: Camera not active
 * BLUE      : Human tetected
**************************************************************/
#include <Arduino.h>
#include <Arduino_APDS9960.h>      // Proximity and gesture sensor
#include <Arduino_OV767X.h>        // Camera
#include "Tauno_Status.h"          // Shift Register with 7 diffrent colour LEDs
#include "Tauno_LEDs.h"            // LEDs for the camera, on left and right
#include "Tauno_Bell.h"            // Doorbell
#include <Scheduler.h>             // Scheduler since we want to manage multiple tasks.

// 0 run ML MODEL
// 1 Take images and send them to computer over serial. (Py script to same them).
#define BUILD_DATASET 0


#if BUILD_DATASET == 1
  #include "mbed.h"
  #include "img_functions.h"  // Image functions for data collecting
#else
  #include "ei_functions.h"   // for mobiili-tuvastus_inferencing
#endif


/* Pins */
#define BELL_PIN      11  // D11
#define LEFT_LED_PIN   7  // D7
#define RIGHT_LED_PIN A7  // A7
#define LATCH_PIN     13  // D13 Status Shift Register
#define CLOCK_PIN     A2  // p30 Status Shift Register 
#define DATA_PIN      12  // D12 Status Shift Register

/* LEDs:
   DARK_RED, RED, ORANGE, YELLOW, GREEN, DARK_GREEN, BLUE
*/
#define SYTEM_LED   GREEN
#define CAMERA_LED  RED
#define BELL_LED    DARK_RED
#define MOBILE_LED  YELLOW
#define HUMAN_LED   BLUE
#define DATA_LED    ORANGE

/* Bell */
const int BELL_DURATION = 8000;  // 8 seconds
uint32_t Bell_on_time = 0;
bool Is_bell_on = false;

/* APDS9960 */
int proximity = 0;
int R = 0;
int G = 0;
int B = 0;
int Ambient = 0;
const int LIGHT_THRESHOLD = 1700;  // if Ambient is < lights on

/* ML model*/
#define MOB_GAT_ID       1
#define MOBILE_THRESHOLD 0.65
#define INFERENCE_DELAY  1000

/* Init objects */
Tauno_Status Status(LATCH_PIN, CLOCK_PIN, DATA_PIN);  // Shift Register
Tauno_LEDs Light(LEFT_LED_PIN, RIGHT_LED_PIN);
Tauno_Bell Bell(BELL_PIN);

/*
Function to test hardware elements one by one
*/
void tests() {
  Bell.bell();
  Light.test();
  Status.test();
}


/****************************************************************
 loop: 0, CAMERA thread
 ****************************************************************/
void loop() {
  bool stop_inferencing = false;
  uint32_t time_now = millis();

#if BUILD_DATASET == 1

    Status.on(CAMERA_LED);
    Light.on();
    Camera.readFrame(data);
    uint8_t rgb888[3];
    Light.off();
    Status.off(CAMERA_LED);

    Status.on(DATA_LED);
    Serial.println("<image>");
    Serial.println(Camera.width());
    Serial.println(Camera.height());

    //const int step_bytes = bytes_per_pixel * 2;

    for(int i = 0; i < bytes_per_frame; i+=bytes_per_pixel * 2) {
      // Note: U and V are swapped
      const int32_t Y0 = data[i + 0];
      const int32_t Cr = data[i + 1];
      const int32_t Y1 = data[i + 2];
      const int32_t Cb = data[i + 3];
      ycbcr422_to_rgb888(Y0, Cb, Cr, &rgb888[0]);
      Serial.println(rgb888[0]);
      Serial.println(rgb888[1]);
      Serial.println(rgb888[2]);
      ycbcr422_to_rgb888(Y1, Cb, Cr, &rgb888[0]);
      Serial.println(rgb888[0]);
      Serial.println(rgb888[1]);
      Serial.println(rgb888[2]);
    }

    Serial.println("</image>");
    Status.off(DATA_LED);

#else

  while(stop_inferencing == false) {
    time_now = millis();
    
    // Set Is_bell_on to false after BELL_DURATION
    if (Is_bell_on) {
      if ( (time_now - Bell_on_time) >= BELL_DURATION) {
        Is_bell_on = false;
        Status.off(BELL_LED);
        //Serial.println("Bell OFF");
      }
    }

    ei_printf("\nStarting inferencing...\n");
    // instead of wait_ms, we'll wait on the signal, this allows threads to cancel us...
    if (ei_sleep(INFERENCE_DELAY) != EI_IMPULSE_OK) {
      break;
    }

    ei_printf("Taking photo...\n");
    Status.off(MOBILE_LED);     // Mobile detected
    Status.off(HUMAN_LED);      // Human detected
    Status.on(CAMERA_LED);      // Camera active

    if (ei_camera_init() == false) {
      ei_printf("ERR: Failed to initialize image sensor\r\n");
      break;
    }

    // choose resize dimensions
    uint32_t resize_col_sz;
    uint32_t resize_row_sz;
    bool do_resize = false;
    int res = calculate_resize_dimensions(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT, &resize_col_sz, &resize_row_sz, &do_resize);
    if (res) {
      ei_printf("ERR: Failed to calculate resize dimensions (%d)\r\n", res);
      break;
    }

    void *snapshot_mem = NULL;
    uint8_t *snapshot_buf = NULL;
    snapshot_mem = ei_malloc(resize_col_sz*resize_row_sz*2);
    if(snapshot_mem == NULL) {
      ei_printf("failed to create snapshot_mem\r\n");
      break;
    }
    snapshot_buf = (uint8_t *)DWORD_ALIGN_PTR((uintptr_t)snapshot_mem);

    if (ei_camera_capture(EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf) == false) {
      ei_printf("Failed to capture image\r\n");
      if (snapshot_mem) ei_free(snapshot_mem);
      break;
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_cutout_get_data;

    Status.off(CAMERA_LED);     // Camera active

    // run the impulse: DSP, neural network and the Anomaly algorithm
    ei_impulse_result_t result = { 0 };

    EI_IMPULSE_ERROR ei_error = run_classifier(&signal, &result, debug_nn);
    if (ei_error != EI_IMPULSE_OK) {
      ei_printf("Failed to run impulse (%d)\n", ei_error);
      ei_free(snapshot_mem);
      break;
    }

    // print the predictions
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
              result.timing.dsp, result.timing.classification, result.timing.anomaly);
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    bool bb_found = result.bounding_boxes[0].value > 0;
    for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
      auto bb = result.bounding_boxes[ix];
      if (bb.value == 0) {
        continue;
      }

      ei_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
    }

    if (!bb_found) {
      ei_printf("    No objects found\n");
    }
#else
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
      ei_printf("    %s: %.5f\n", result.classification[ix].label,
                                  result.classification[ix].value);
    }


    if (result.classification[MOB_GAT_ID].value > MOBILE_THRESHOLD ) {
      if (!Is_bell_on) {
        Is_bell_on = true;
        Bell_on_time = millis();
        Bell.bell();
        Status.on(MOBILE_LED);     // Mobile detected
        Status.on(BELL_LED);       // Bell on
        Serial.println("Bell ON");
      }
    }

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
#endif
    /* // Tahan, et töötaks ka kui serial ei ole ühendatud
    while (ei_get_serial_available() > 0) {
      if (ei_get_serial_byte() == 'b') {
        ei_printf("Inferencing stopped by user\r\n");
        stop_inferencing = true; 
      }
    }
    */
    if (snapshot_mem) ei_free(snapshot_mem);
  }
  ei_camera_deinit();

#endif // Build dataset

yield();
}  // Loop 0 end

/****************************************************************
 loop: , sensors thread
 ****************************************************************/

void loop1() {
  // Check if a proximity reading is available.
  if (APDS.proximityAvailable()) {
    proximity = APDS.readProximity();
    Serial.print("PR=");
    Serial.print(proximity);
  }

  // Kui vaataja on lähedal
  if (proximity < 250) {
    Light.on();
  } else {
    Light.off();
  }

  // Check if a color reading is available
  if (APDS.colorAvailable()) {
      APDS.readColor(R, G, B, Ambient);
      Serial.print(" RGB=");
      Serial.print(R);
      Serial.print(",");
      Serial.print(G);
      Serial.print(",");
      Serial.println(B);
      Serial.print("A=");
      Serial.println(Ambient);
  }

  // kui RGB = 0,0,0 tuled off
    // Kui ambient 10-thres tuled on
    // TODO:
    // Pimedas ei ole tegelikult üldse mõtet pildistada!?
    if (Ambient > 10 && Ambient < LIGHT_THRESHOLD) {
      //Light.on();
    }

  yield();
}  // Loop 1 end

/************************************************************/
void setup() {
  Serial.begin(115600);

  // Setup Pins
  Bell.begin();
  Status.begin();
  Light.begin();

  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
  }

#if BUILD_DATASET == 1
  if (!Camera.begin(QQVGA, YUV422, 1)) {
    Serial.println("Failed to initialize camera!");
    while (1);
  } else {
    Serial.println("OV7670 Camera initialized!");
  }


  bytes_per_pixel = Camera.bytesPerPixel();
  bytes_per_frame = Camera.width() * Camera.height() * bytes_per_pixel;

  //Camera.testPattern();  // fixed test pattern image with color bands
#else
  // summary of inferencing settings (from model_metadata.h)
  ei_printf("Inferencing settings:\n");
  ei_printf("\tImage resolution: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
  ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
  ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));
#endif

  // Add "loop" to scheduling.
  // "loop" is always started by default.
  Scheduler.startLoop(loop1);

 tests();             // Test hardware elements
 Status.on(SYTEM_LED);
} // setup end
