#include "Arduino.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>

#define DEVICE_ID 1

extern StaticJsonDocument<200> posledniAkce;

int16_t request_vratit(StaticJsonDocument<200> _posledniAkce);

int16_t request_overeni(String *_response_payload, String _tagIdString);

int16_t request_akce(String *_response_payload, String _tagIdString, uint8_t _akce, uint8_t _uloha);