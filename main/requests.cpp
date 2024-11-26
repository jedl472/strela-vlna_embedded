#include "requests.h"
#include "system_settings.h"

#include "display.h"


HTTPClient http;


int16_t request_vratit(StaticJsonDocument<200> _posledniAkce) {
  http.begin(serverName.c_str());
  display_message("posilam data");

  String requestBody;

  _posledniAkce["typ"] = "vratit";
  serializeJson(_posledniAkce, requestBody);
  _posledniAkce["typ"] = "";
              
  http.addHeader("Content-Type", "application/json");

  int16_t httpResponseCode = http.POST(requestBody);

  http.end();

  return (httpResponseCode);
}

int16_t request_overeni(String *_response_payload, String _tagIdString) {
  http.begin(serverName.c_str()); // start session

  String requestBody;
  {
    StaticJsonDocument<200> jsonContainer;
    jsonContainer["typ"] = "overeni";
    jsonContainer["id"] = _tagIdString;
    jsonContainer["ctecka"] = DEVICE_ID;
    serializeJson(jsonContainer, requestBody);
  }

  http.addHeader("Content-Type", "application/json");  

  int16_t httpResponseCode = http.POST(requestBody);

  *_response_payload = http.getString();
  
  http.end();

  return (httpResponseCode);
}

int16_t request_akce(String *_response_payload, String _tagIdString, uint8_t _akce, uint8_t _uloha, StaticJsonDocument<200> _posledniAkce) {
http.begin(serverName.c_str());
  String requestBody;
  {
    StaticJsonDocument<200> jsonContainer;
    jsonContainer["typ"] = "akce";                       _posledniAkce["typ"] = "akce";
    jsonContainer["id"] = _tagIdString;                  _posledniAkce["id"] = _tagIdString;
    jsonContainer["akce"] = _akce;                       _posledniAkce["akce"] = _akce;
    jsonContainer["uloha"] = _uloha;                     _posledniAkce["uloha"] = _uloha;
    jsonContainer["ctecka"] = DEVICE_ID;                 _posledniAkce["ctecka"] = DEVICE_ID;

              
    serializeJson(jsonContainer, requestBody);       
  }
            
  http.addHeader("Content-Type", "application/json");     

  int16_t httpResponseCode = http.POST(requestBody);

  *_response_payload = http.getString();
  http.end();
}