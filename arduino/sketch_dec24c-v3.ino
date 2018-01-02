#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "Adafruit_MPR121.h"

Adafruit_MPR121 cap = Adafruit_MPR121();

uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint8_t captouched = 0;
boolean capcount[12] = {false, false, false, false, false, false, false, false, false, false, false, false};
boolean keypressed = false;

const char* ssid = "SSID HERE";
const char* password =  "PASSWORD HERE";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  while (!Serial) { 
    delay(10);
  }

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test");

  delay(2000);

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
}

void loop() {
  currtouched = cap.touched();

  for (uint8_t i = 0; i < 12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
    }
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
      captouched = i;
      keypressed = true;
      break;
    }
  }

  // reset our state
  lasttouched = currtouched;

  if (keypressed) {

    Serial.println("debug " + String(captouched));
    Serial.println("debug " + String(capcount[captouched]));

    if (capcount[captouched]) {
      capcount[captouched] = false;
    } else {
      capcount[captouched] = true;
    }

    if (WiFi.status() == WL_CONNECTED) { 
      HTTPClient http;
      String httpString = String("http://192.168.1.34:8080/update?cube=") + String(captouched) + "&state=" + String(capcount[captouched]);
      Serial.println(httpString);

      http.begin(httpString);  
      http.addHeader("Content-Type", "text/plain");             

      int httpResponseCode = http.POST("POSTING from ESP32");   

      if (httpResponseCode > 0) {
        String response = http.getString();                      

        //Serial.println(httpResponseCode);  
        //Serial.println(response);           
      } 
      http.end();  
    } else {
      Serial.println("Error in WiFi connection");
    }
  }
  keypressed = false;

  delay(350);
}


