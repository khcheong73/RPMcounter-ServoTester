Servo/RPM Tester Arduino Code README

** Overview
This code is an Arduino program that controls a Servo motor and measures its rotation speed (RPM) using an OLED display.

** Key Features
1. Servo Motor Control:
- Set the Servo motor angle using a potentiometer.
- Provides a smooth animation effect when the Servo motor angle changes.
2. RPM Measurement:
- Measures the rotation speed of the Servo motor using a Rotary Encoder.
- Displays the measured RPM on the OLED display.
3. OLED Display:
- Displays Servo motor angle, RPM, and voltage information on the OLED display.
- Displays the title "Servo/RPM Tester".
4. Button:
- Press the button to change the number of Blades used for Servo motor control.

** Arduino Pin Function Summary
2	  Rotary Encoder (Signal)
3	  Blade Count Button
A6	Servo Angle Setting Potentiometer
A2	External Power Input Voltage Measurement
A3	Servo Motor Power Voltage Measurement
9	  Servo Motor PWM Control
SCL	OLED Display I2C Clock Signal
SDA	OLED Display I2C Data Signal

** For the reference hardware, see the attached schematic.

** Required Arduino Libraries for Servo/RPM Tester Code
The Servo/RPM Tester Arduino code utilizes several libraries to provide various functionalities. Here's a summary of the essential libraries:

1. Arduino.h:
This core Arduino library provides fundamental functions and definitions for interacting with the Arduino hardware. It includes functions for input/output operations, timers, interrupts, and other essential functionalities.

2. U8g2lib.h:
This library is specifically designed for controlling and communicating with OLED displays. It offers a comprehensive set of functions for drawing text, graphics, and manipulating the display buffer. The code uses U8g2lib to display various information on the OLED display, including servo angle, RPM, voltage readings, and the title "Servo/RPM Tester".

3. Servo.h:
This library provides high-level functions for controlling servo motors. It simplifies the process of sending control signals to servo motors, allowing for precise angle adjustments. The code uses Servo.h to control the servo motor based on potentiometer input and animation effects.

Additional Libraries:
Wire.h: This library is used for I2C communication, which is the interface used to connect the OLED display to the Arduino board.
Note:

These libraries are typically included in the Arduino IDE's library manager, making it easy to install and integrate them into your projects.
Ensure that you have the latest versions of these libraries installed for optimal compatibility and performance.
Refer to the official documentation of each library for detailed usage instructions and examples.
