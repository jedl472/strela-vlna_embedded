#include "Arduino.h"

void inputSetup();

void raw_updateButtons(bool *_input);

void updateParseInput(bool *_inputButtons, uint8_t *_output, unsigned long *buttonPressedMillis);