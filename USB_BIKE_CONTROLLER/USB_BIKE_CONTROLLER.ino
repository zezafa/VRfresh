/*
  USB bike controller: Magnet switch for pedals + three capacitive buttons for bike handles
  28 januar 2016
*/

#include <CapacitiveSensor.h>

long timer;
int button_left;
int button_right;

//Connections
int buttonPin = 8;

//Checker
int checkForChange;
int w_active;
int x_active;
int d_active;
int bell_active;

CapacitiveSensor cs_4_2 = CapacitiveSensor(4,2);        // 1M resistor between pins 4 & 2, pin 2 is sensor pin
CapacitiveSensor cs_4_6 = CapacitiveSensor(4,6);        
CapacitiveSensor cs_bell = CapacitiveSensor(14,16);        

void setup() {
  Serial.begin(9600);
  Keyboard.begin();
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // Magnet switch for pedals
  int buttonState = digitalRead(buttonPin);
  if (buttonState == 0) {
    if (checkForChange == 0) {
      //Serial.println("Activate forward motion");
      checkForChange = 1;
      timer = millis();
    }
  }
  else if (buttonState == 1) {
    checkForChange = 0;
  }
  if (millis()-timer<500) {
    Serial.println("Pressing W..."+String(millis()-timer));
    Keyboard.press('W');
    w_active = 1;
  }
  else {
    if (w_active==1){
      Serial.println("Releasing W...");
      Keyboard.release('W');
      w_active = 0;
    }
  }

  // Capacitive buttons
  long total1 = cs_4_2.capacitiveSensor(30);
  long total2 = cs_4_6.capacitiveSensor(30);
  long total3WWW = cs_bell.capacitiveSensor(30);
  if (total1>300){
    Serial.println("Handbrake active");
    timer += 5000; //To cancel acceleration
    Keyboard.press('X');
    x_active = 1;
  }
  else if (total1 <= 300) {
    if (x_active == 1) {
      Keyboard.release('X');
      //Serial.println("Handbrake disabled");
      x_active = 0;
    }
  }
  if (total2>300){
    Keyboard.press('Z');
    Serial.println("Brake active");
    timer += 5000; //To cancel acceleration
    d_active = 1;
  }
  else if (total2 <= 300){
    if (d_active==1) {
      Keyboard.release('Z');
      //Serial.println("Brake disabled");
      d_active = 0;
    }
  }
    if (total3>300){
    Keyboard.press('-');
    Serial.println("Bell active");
    timer += 5000; //To cancel acceleration
    bell_active = 1;
  }
  else if (total3 <= 300){
    if (bell_active==1) {
      Keyboard.release('-');
      //Serial.println("Brake disabled");
      bell_active = 0;
    }
  }
}
