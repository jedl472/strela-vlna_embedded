#include "Arduino.h"

void inputSetup();
//prvotni setup tlacitek typu pinMode

void raw_updateButtons(bool *_input);
//proste vezme vstupy a umisti je do _input promene (bool[6])

void updateParseInput(bool *_inputButtons, uint8_t *_output, unsigned long *buttonPressedMillis); 
//čistě účelová funkce přímo pro parsování inputu z tlačítek pro vytvoreni menu. Je tu schovaný bordel jako debounce. existuje jenom kvůli větší modularitě