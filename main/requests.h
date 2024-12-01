#include "Arduino.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DEFAULT_MAX_JSON_SIZE 200

extern StaticJsonDocument<DEFAULT_MAX_JSON_SIZE> posledniAkce;

int16_t request_vratit(StaticJsonDocument<DEFAULT_MAX_JSON_SIZE> _posledniAkce);

int16_t request_overeni(String *_response_payload, String _tagIdString);

int16_t request_akce(String *_response_payload, String _tagIdString, uint8_t _akce, uint8_t _uloha);