// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO

#define sensorAThresh 15200
#define sensorBThresh 15000
#define delayAfterDetect 40
#define loopDelayInUs 10

//#define PRINT_DEBUG


#define LED_PIN 13
#define SENSORAPIN 5
#define SENSORBPIN 6

bool blinkState = false;
bool reading = false;
int maxAcc = 0, temp = 0;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    #if defined PRINT_DEBUG 
      Serial.begin(115200);
    #endif

    // initialize device
    //Serial.println("Initializing I2C devices...");
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(SENSORAPIN, OUTPUT);
    pinMode(SENSORBPIN, OUTPUT);
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    accelgyro.initialize();
    
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
    accelgyro.initialize();

    // verify connection
    //Serial.println("Testing device connections...");
    //Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // configure Arduino LED for
    pinMode(LED_PIN, OUTPUT);

}

void loop() {
  //Sensor A
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    //delay(1);
    temp = accelgyro.getAccelerationZ();
    
    #if defined PRINT_DEBUG 
      Serial.print(temp); 
    #endif
    
    if(temp > sensorAThresh){
      digitalWrite(SENSORAPIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      delay(delayAfterDetect);
      digitalWrite(SENSORAPIN, LOW);
      digitalWrite(LED_PIN, LOW);
    }
    
    //Sensor B
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
    //delay(1);
    
    #if defined PRINT_DEBUG 
      Serial.print('\t');
    #endif
    
    temp = accelgyro.getAccelerationZ();
    if(temp > sensorBThresh){
      digitalWrite(SENSORBPIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      delay(delayAfterDetect);
      digitalWrite(SENSORBPIN, LOW);
      digitalWrite(LED_PIN, LOW);
    }
    #if defined PRINT_DEBUG 
      Serial.println(temp);
    #endif
    delayMicroseconds(loopDelayInUs);
}
