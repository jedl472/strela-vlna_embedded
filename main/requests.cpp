#include "requests.h"
#include "system_settings.h"
#include "auth.h"

#include "display.h"


HTTPClient http;

JsonDocument posledniAkce; 
int16_t request_overeni(String *_response_payload, String _tagIdString) {
  http.begin(serverName.c_str()); // start session

  String requestBody;

  JsonDocument jsonContainer;
  jsonContainer["typ"] = "overeni";
  jsonContainer["id"] = _tagIdString;
  jsonContainer["ctecka"] = String(DEVICE_ID);
  serializeJson(jsonContainer, requestBody);

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", AUTH_TOKEN);  

  int16_t httpResponseCode = http.POST(requestBody);

  *_response_payload = http.getString();

  if(DEBUG_MODE) {
    Serial.println();
    Serial.print("requestBody:");
    Serial.print(requestBody);
    Serial.print(" httpResponseCode:");
    Serial.println(httpResponseCode);
    Serial.print("server response:");
    Serial.println(*_response_payload);
  }
  
  http.end();

  return (httpResponseCode);
}

int16_t request_akce(String *_response_payload, String _tagIdString, uint8_t _akce, uint8_t _uloha) {
  http.begin(serverName.c_str());

  String requestBody;
  {
    JsonDocument jsonContainer;
    jsonContainer["typ"] = "akce";                       posledniAkce["typ"] = "akce";
    jsonContainer["id"] = _tagIdString;                  posledniAkce["id"] = _tagIdString;
    jsonContainer["akce"] = String(_akce);               posledniAkce["akce"] = String(_akce);
    jsonContainer["uloha"] = String(_uloha);             posledniAkce["uloha"] = String(_uloha);
    jsonContainer["ctecka"] = String(DEVICE_ID);         posledniAkce["ctecka"] = String(DEVICE_ID);
    jsonContainer["pocet"] = String(DEVICE_ID);         posledniAkce["pocet"] = String(DEVICE_ID);

              
    serializeJson(jsonContainer, requestBody);       
  }

  http.addHeader("Content-Type", "application/json");   
  http.addHeader("Authorization", AUTH_TOKEN);  

  
  int16_t httpResponseCode = http.POST(requestBody);

  *_response_payload = http.getString();

  if(DEBUG_MODE) {
    Serial.println();
    Serial.print("requestBody:");
    Serial.print(requestBody);
    Serial.print(" httpResponseCode:");
    Serial.println(httpResponseCode);
  }

  http.end();

  return (httpResponseCode);
}

int16_t request_vratit(JsonDocument _posledniAkce) {
  http.begin(serverName.c_str());
  display_message("posilam data");

  String requestBody;

  _posledniAkce["typ"] = "vratit";
  serializeJson(_posledniAkce, requestBody);
              
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", AUTH_TOKEN);  

  int16_t httpResponseCode = http.POST(requestBody);

  if(DEBUG_MODE) {
    Serial.println();
    Serial.print("requestBody:");
    Serial.print(requestBody);
    Serial.print(" httpResponseCode:");
    Serial.println(httpResponseCode);
  }

  http.end();

  return (httpResponseCode);
}