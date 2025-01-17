/*
Soubor, kde je všechno posílání requestů, a taky se zde pracuje s json.
V každé funkci se na začátku zavolá http.begin a na konci http.end, není tedy potřeba volat nikde jinde.

Všechny funkce vrací http response code do int16_t.
*/

#include "Arduino.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>


extern JsonDocument posledniAkce; //zde se ukládá poslední request s typem akce, taky se ukládá v serializované formě do EEPROM (main.ino)

int16_t request_vratit(JsonDocument _posledniAkce);
/*
Pošle request na vrácení poslední akce. Vezme si přímo před tím odeslaný json.
*/

int16_t request_overeni(String *_response_payload, String _tagIdString);
/*
Ověří zprávnost tagu, do _response_payload uloží odpověď a _tagIdString je tag k ověření.
*/

int16_t request_akce(String *_response_payload, String _tagIdString, uint8_t _akce, uint8_t _uloha, uint8_t _pocet);
/*
Udělá akci, odpověd uloží do _response_payload. Data do requestu bere ze zbytku promennych.
*/