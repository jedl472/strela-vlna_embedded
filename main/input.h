#include "Arduino.h"


//#define BUTTON_DEBOUNCE 50

extern TaskHandle_t Handle_ui_main_menu;
extern bool isMainMenuActive;
extern bool canBeMainMenuTurnedOn;


void init_input();
//prvotni setup tlacitek typu pinMode

void raw_updateButtons(bool *_input);
//proste vezme vstupy a umisti je do _input promene (bool[6])

void updateParseInput(bool *_inputButtons, bool *_lastInputButtons, int8_t *_output, unsigned long *buttonPressedMillis);
//čistě účelová funkce přímo pro parsování inputu z tlačítek pro vytvoreni menu. Je tu schovaný bordel jako debounce. existuje jenom kvůli větší modularitě