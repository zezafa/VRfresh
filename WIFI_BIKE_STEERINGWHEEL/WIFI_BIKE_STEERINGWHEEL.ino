/*
  Steering wheel unit (RC receiver and ESP8266) for WIFI-enabled bike controller
  28 januar 2016
*/

#include "ESP8266.h"
#include <SoftwareSerial.h>
#include <VirtualWire.h>

#define SSID        "ZyXEL_5734"
#define PASSWORD    "EJ73W3N79PCXX"
#define HOST_NAME   "192.168.1.35"
#define HOST_PORT   (5416)

SoftwareSerial mySerial(9, 8); /* RX:9, TX:8 */
ESP8266 wifi(mySerial);

unsigned long tid;

int button_1 = 0;
int button_2 = 0;

int heading = 90;

void setup(void)
{
    Serial.begin(9600);  
    Serial.print("setup begin\r\n");
    wifi.restart();
    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());
      
    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        wifi.setOprToStationSoftAP();
        Serial.print("to station + softap err\r\n");
    }
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");
        Serial.println(wifi.getLocalIP().c_str());       
    } else {
        wifi.joinAP(SSID, PASSWORD);
        wifi.getLocalIP().c_str();
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }

    vw_set_rx_pin(12);       //Sets pin D12 as the RX Pin
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(4000);       // Bits per sec
    vw_rx_start();       // Start the receiver PLL running
    
    Serial.print("setup end\r\n");
    
}
 
void loop(void)
{
  //********
  //get heading form HMC5883L
  //********

  //********
  //check button 1 and 2
  //********
  
  //********
  //receive RC msg from BIKE_PEDAL and send WIFI msg to PC
  //********
  
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  //sendWIFI("test");
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    digitalWrite(13, true);
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

    String string_msgWIFI = msgRC; //+ "," + String(heading) + "," + String(button_1) + "," + String(button_2);
    char *msgWIFI = strdup(string_msgWIFI.c_str());
    sendWIFI(msgWIFI);
    digitalWrite(13, false);
  }

  delay(25);
}

void sendWIFI(char *charMsg){
    //uint8_t buffer[128] = {0};

    Serial.println("register UDP");
    wifi.registerUDP(HOST_NAME, HOST_PORT);
    Serial.println("send package");
    wifi.send((const uint8_t*)charMsg, strlen(charMsg));
    /*Serial.println("recv");
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 50);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }*/
    Serial.println("unregister UDP");
    wifi.unregisterUDP();
}
          
