# Original code: https://github.com/PacktPublishing/TinyML-Cookbook/blob/main/Chapter05/PythonScripts/02_parse_camera_frame.py
import numpy as np
import serial          # pip install pyserial
import sys
import uuid
from PIL import Image  # pip install Pillow

port = '/dev/ttyACM0' # /dev/ttyACM0
baudrate = 115600

# Initialize serial port
ser = serial.Serial()
ser.port     = port
ser.baudrate = baudrate

ser.open()
ser.reset_input_buffer()

def serial_readline():
  '''Return line from the serial port as a string.'''
  data = ser.readline() # read a '\n' terminated line
  return data.decode("utf-8").strip()

width  = 320
height = 240
num_ch = 3

# 3D Numpy array
image = np.empty((height, width, num_ch), dtype=np.uint8)

print("Waiting")

while True:
  data_str = serial_readline()

  if str(data_str) == "<image>":
    print("Image started")
    # parse the frame resolution
    # and resize the Numpy array
    w_str = serial_readline()
    h_str = serial_readline()
    w = int(w_str)
    h = int(h_str)

    if w != width or h != height:
      print("Resizing numpy array")

      if w * h != width * height:
        image.resize((h, w, num_ch))
      else:
        image.reshape((h, w, num_ch))

      width  = w
      height = h

    print("Reading frame:", width, height)

    # Parse the pixel values
    # ver 1
    #'''
    for y in range(0, height):
      for x in range(0, width):
        for c in range(0, num_ch):
          data_str = serial_readline()
          image[y][x][c] = int(data_str)
    #'''
    # ver 2: more efficient solution
    '''
    for i in range(0, width * height * num_ch):
      c = int(i % num_ch)
      x = int((i / num_ch) % width)
      y = int((i / num_ch) / width)
      data_str = serial_readline()
    image[y][x][c] = int(data_str)
    '''

    # image end
    data_str = serial_readline()
    if str(data_str) == "</image>":
      print("Image ended.")
      image_pil = Image.fromarray(image)
      image_pil.show()
    else:
      print("Error capture image")
