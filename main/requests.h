#include "Arduino.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

extern JsonDocument posledniAkce;

int16_t request_vratit(JsonDocument _posledniAkce);

int16_t request_overeni(String *_response_payload, String _tagIdString);

int16_t request_akce(String *_response_payload, String _tagIdString, uint8_t _akce, uint8_t _uloha);