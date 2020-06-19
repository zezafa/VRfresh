/*
  Pedal unit (RC transmitter and MPU 6050) for WIFI-enabled bike controller
  28 januar 2016
*/

#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <VirtualWire.h>
#include <string.h>

MPU6050 accelgyro;

int16_t ax, ay, az, gx, gy, gz;

double timeStep, time, timePrev;
double arx, ary, arz, grx, gry, grz, gsx, gsy, gsz, rx, ry, rz;
double old_rz;
int i;
double gyroScale = 131;

void setup() {
  Serial.println("Setup initiated"); //Prints "Setup" to the serial monitor
  Wire.begin();
  Serial.begin(9600);
  accelgyro.initialize();

  time = millis();

  i = 1;

  vw_set_tx_pin(4);         
  vw_set_ptt_inverted(true);  // Required for DR3100
  vw_setup(4000);         // Bits per sec
    
  Serial.println("Setup done"); //Prints "Setup" to the serial monitor
}

void loop() {
  // set up time for integration
  timePrev = time;
  time = millis();
  timeStep = (time - timePrev) / 1000; // time-step in s

  // collect readings
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // apply gyro scale from datasheet
  gsx = gx/gyroScale;   gsy = gy/gyroScale;   gsz = gz/gyroScale;

  // calculate accelerometer angles
  arx = (180/3.141592) * atan(ax / sqrt(square(ay) + square(az))); 
  ary = (180/3.141592) * atan(ay / sqrt(square(ax) + square(az)));
  arz = (180/3.141592) * atan(sqrt(square(ay) + square(ax)) / az);

  // set initial values equal to accel values
  if (i == 1) {
    i = i + 1;
    grx = arx;
    gry = ary;
    grz = arz;
  }
  // integrate to find the gyro angle
  else{
    grx = grx + (timeStep * gsx);
    gry = gry + (timeStep * gsy);
    grz = grz + (timeStep * gsz);
  }  

  // apply filter
  rx = (0.1 * arx) + (0.9 * grx);
  ry = (0.1 * ary) + (0.9 * gry);
  rz = (0.1 * arz) + (0.9 * grz);

  double velrz = (rz-old_rz)/timeStep; 
  velrz = constrain(abs(velrz), 0, 250);
  velrz = map(velrz, 0, 250, 0, 50);
  // print and send result
  Serial.print(velrz);   Serial.print("\t\t");

  sendRC(arz);
  Serial.println("Sent");

  old_rz = rz;
  delay(50);
}

void sendRC(double msgDouble){
  digitalWrite(17, true);
  int msgInt = int(msgDouble);
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  char charVal[100];
  sprintf(charVal, "%d", msgInt);
  const char *msg = charVal;
  vw_send((uint8_t *)msg, strlen(msg)); //Sending the message
  vw_wait_tx(); // Wait until the whole message is gone
  digitalWrite(17, false);
}


