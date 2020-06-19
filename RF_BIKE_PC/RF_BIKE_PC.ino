/*
  RC command receiver (RC receiver 433 mhz) for bike PC controller
  29 januar 2016
*/

#include <VirtualWire.h>

int button_1 = 0;
int button_2 = 0;

int heading = 90;

void setup() {
  Serial.begin(9600);  
  Serial.print("setup begin\r\n");
  vw_set_rx_pin(4);       //Sets pin D12 as the RX Pin
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(4000);       // Bits per sec
  vw_rx_start();       // Start the receiver PLL running
    
  Serial.print("setup end\r\n");
}

void loop() {
  //********
  //receive RC msg from BIKE_PEDAL and send WIFI msg to PC
  //********
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    digitalWrite(17, true);
    int i;
    Serial.print("Got: ");
    String msgRC = "";
    for (i = 0; i < buflen; i++)
    {
      char c = (buf[i]);
      msgRC += c;
    }
    Serial.print(msgRC);
    Serial.println("");
    digitalWrite(17, false);
  }
  //********
  //get heading form HMC5883L
  //********

  //********
  //check button 1 and 2
  //********

  //********
  //check pedal motion
  //********
}
