/*
  Pedal unit (RC transmitter and MPU 6050) for WIFI-enabled bike controller
  28 januar 2016
*/

#include "Wire.h"
#include <VirtualWire.h>
#include <string.h>
#include <HMC5883L.h>

HMC5883L compass;

int error = 0;
int initialHeading;
int lastHeading;

int buttonPin = 12;

void setup() {
  Serial.println("Setup initiated"); //Prints "Setup" to the serial monitor
  Wire.begin();
  Serial.begin(9600);
  
  // Compass
  Serial.println("HMC5883L initializing...");
  compass = HMC5883L();
  Serial.println("Setting scale to +/- 1.3 Ga");
  error = compass.SetScale(1.3); // Set the scale of the compass to 1.3Ga
  if(error != 0){ // If there is an error, print it out. 
    Serial.println(compass.GetErrorText(error));
    error =0;
  }
  Serial.println("Setting measurement mode to continous.");
  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  if(error != 0) {// If there is an error, print it out.
    Serial.println(compass.GetErrorText(error)); //Todo: Error handling for this method in .h and .cpp
    error=0;
  }
  
  // RF module
  Serial.println("RF transmitter initializing...");
  vw_set_tx_pin(4);         
  vw_set_ptt_inverted(true);  // Required for DR3100
  vw_setup(4000);         // Bits per sec

  // Callibration button
  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("Setup done"); //Prints "Setup" to the serial monitor
}

void loop() {
  // Get heading
  int heading = getHeading();
  Serial.print(heading);
  sendRC(heading);
  Serial.println("\tSent");

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

int getHeading() {
  MagnetometerRaw raw = compass.ReadRawAxis();
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  // Values are accessed like so:
  int MilliGauss_OnThe_XAxis = scaled.XAxis;// (or YAxis, or ZAxis)
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  // Once you have your heading, you must then add your 'Declination Angle',
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  float declinationAngle = 0.0325;
  heading += declinationAngle;
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
  // Convert radians to degrees for readability
  int headingDegrees = heading * 180/M_PI;
  // Callib
  int callibBool = digitalRead(buttonPin);
  if (callibBool == LOW) {
    initialHeading = headingDegrees;
    Serial.println("Callibrated");
  }
  // Check for wild readings
  /*if (abs(headingDegrees - lastHeading) > 30) {
    return lastHeading;
  }*/
  lastHeading = headingDegrees;
  // Output
  return headingDegrees-initialHeading;
}


