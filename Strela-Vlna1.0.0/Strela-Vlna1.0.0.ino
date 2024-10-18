/*
  Rui Santos
  Complete project details at Complete project details at https://RandomNerdTutorials.com/esp32-http-get-post-arduino/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define button1 = 2; //pin
#define button2 = 8;//pin

// uint8_t tlacitko[2] = {0};
// const char* ssid = "GAM2";
// const char* password = "JejTGame123+";

const char* ssid = "Simon774";
const char* password = "skaut123";
String poslat = "";

//Your Domain name with URL path or IP address with path
// String serverName = "http://192.168.22.222:80";
String serverName = "http://192.168.226.52:80";
String serverPath = serverName;
HTTPClient http;

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 50;

bool gameSession = 0;
bool PressedButton = false;
int awaitRequest = 0;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  // HTTPClient http;

  String serverPath = serverName;
  
  // Your Domain name with URL path or IP address with path
  http.begin(serverPath.c_str());
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    button();
    // Serial.print(poslat);
    String msg = poslat;
    if (msg == "12"){
      Serial.println("k");
      http.end();
    }
    wifina(msg);
    lastTime = millis();
  }

  // if(gameSession) {
  //   updateButtons();
  //   updateRequest();
    

  // }
}


void wifina(String msg){
  // if(WiFi.status()== WL_CONNECTED){
  if(true){
    
    // If you need Node-RED/server authentication, insert user and password below
  
  // Your Domain name with URL path or IP address with path
  // http.begin(serverPath.c_str());
    //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
    
    // Send HTTP GET request
    // String msg = "a";
    int httpResponseCode = http.sendRequest("POST",msg);
    // int httpResponseCode = http.GET();

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      if(WiFi.status()== WL_CONNECTED){
        Serial.println("p");
        http.begin(serverPath.c_str());
      }
    }
    // Free resources
    // http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}




int button(){
  // uint8_t tlacitko[2] = {0};
  String tlacitko = "";
  for(uint8_t i = 0;i<2;i++){
    bool wait = true;
    do{
      // if (button1 != 0){button[i] = 1;wait = false;}
      // if (button2 != 0){button[i] = 1;wait = false;}
      int inByte = Serial.read();
      switch (inByte) {
          case '1':
            // tlacitko[i]=1;
            tlacitko = tlacitko + "1";
            Serial.print("1");
            wait = false;
            break;
          case '2':
            // tlacitko[i]=2;
            Serial.print(2);
            tlacitko = tlacitko + "2";
            wait = false;
            break;
          case '3':
            // tlacitko[i]=2;
            Serial.print(3);
            tlacitko = tlacitko + "3";
            wait = false;
            break;

          default:

            break;
        }
      // if (button1 != 1 && button2 != 1){wait = False}
    }while(wait);
  }
  poslat = tlacitko;
  return 1;
}

// void updateButtons(uint) {

// }
// void updateRequest() { //posilání requestů podle stavu awaitRequest
//   switch(awaitRequest){
//     case 0: //default state
//       break;
//     case 1:

//       break;
//     case 2:
//       //reserved for debug
//       break;
//   }
// }
