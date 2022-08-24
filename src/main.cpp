/*************************************************************
 * File: main.cpp
 * Documentation: https://github.com/taunoe/flower-camera
 * Started 09.07.2022
 * Edited  24.08.2022
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

/* Bell */
const int BELL_DURATION = 9000; // 9 seconds
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
#define MOBILE_THRESHOLD 0.6
#define HUMAN_THRESHOLD 0.5
#define INFERENCE_DELAY 1000

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

 tests();  // Test hardware elements
}


void loop() {
  bool stop_inferencing = false;
  uint32_t time_now = millis();

#if BUILD_DATASET == 1
    Status.off(GREEN);

    Status.on(RED);
    Light.on();
    Camera.readFrame(data);
    uint8_t rgb888[3];
    Light.off();
    Status.off(RED);

    Status.on(ORANGE);
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
    Status.off(ORANGE);

    Status.on(GREEN);

#else

  while(stop_inferencing == false) {
    time_now = millis();
    
    // Set Is_bell_on to false after BELL_DURATION
    if (Is_bell_on) {
      if ( (time_now - Bell_on_time) >= BELL_DURATION) {
        Is_bell_on = false;
        Status.off(DARK_RED);
        Serial.println("Bell OFF");
      }
    }

    // Check if a proximity reading is available.
    if (APDS.proximityAvailable()) {
      proximity = APDS.readProximity();
      Serial.print("PR=");
      Serial.print(proximity);
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

    ei_printf("\nStarting inferencing in 2 seconds...\n");
    // instead of wait_ms, we'll wait on the signal, this allows threads to cancel us...
    if (ei_sleep(INFERENCE_DELAY) != EI_IMPULSE_OK) {
      break;
    }

    ei_printf("Taking photo...\n");
    Status.off(YELLOW);    // Mobile detected
    Status.off(BLUE);      // Human detected
    Status.off(DARK_GREEN); // Camera not active
    Status.on(RED);        // Camera active

    if (Ambient < LIGHT_THRESHOLD) {
      Light.on();
    }

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

    Light.off();
    Status.off(RED);        // Camera active
    Status.on(DARK_GREEN);  // Camera not active

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

    if (result.classification[0].value > HUMAN_THRESHOLD ) {
      Status.on(BLUE);  // Human tetected
    }

    if (result.classification[1].value > MOBILE_THRESHOLD ) {
      if (!Is_bell_on) {
        Is_bell_on = true;
        Bell_on_time = millis();
        Bell.bell();
        Status.on(YELLOW);    // Mobile detected
        Status.on(DARK_RED);  // Bell on
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



}  // Loop end
