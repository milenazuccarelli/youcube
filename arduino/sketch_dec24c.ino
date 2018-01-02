#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include "Adafruit_MPR121.h"

Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;
uint8_t  captouched = 0;
uint8_t  captouchedcurrent = 0;
uint8_t  capcount[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

const char* ssid = "vodafone-965C";
const char* password =  "GEM2VB79BNXWFR";

void setup() {

  Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to the WiFi network");

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
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
      break;
    }
  }

  // reset our state
  lasttouched = currtouched;
  
  if (captouched == captouchedcurrent && capcount[captouched] == 1) {
    capcount[captouched] = 0;
  } else if(captouched != captouchedcurrent && capcount[captouched] == 0) {
    capcount[captouched] = 1;
  }
  
  if (captouched > 0) {

    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status

      HTTPClient http;
      String httpString = String("http://192.168.1.34:8080/update?cube=") + String(captouched) + "&count=" + String(capcount[captouched]);
      Serial.println(httpString);

      http.begin(httpString);  //Specify destination for HTTP request
      http.addHeader("Content-Type", "text/plain");             //Specify content-type header

      int httpResponseCode = http.POST("POSTING from ESP32");   //Send the actual POST request

      if (httpResponseCode > 0) {

        String response = http.getString();                       //Get the response to the request

        Serial.println(httpResponseCode);   //Print return code
        Serial.println(response);           //Print request answer

      } else {

        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);

      }

      http.end();  //Free resources

    } else {

      Serial.println("Error in WiFi connection");
    }
  }
  
  captouchedcurrent = captouched;

  delay(300);

}


