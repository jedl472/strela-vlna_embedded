#include "input.h"
#include "system_settings.h"

bool isMainMenuActive = 0;
bool canBeMainMenuTurnedOn = 0;

const int8_t menuInputPattern[6] = {1, -1, 1, -1, 1, -1}; //bindovani tlacitek
const int8_t menuOutputPattern[6] = {0, 0, 1, 1, 2, 2};

void IRAM_ATTR menu_on_interrupt() { if(canBeMainMenuTurnedOn == 1) { isMainMenuActive = 1; } }

void init_input() {
  pinMode(TL0,INPUT_PULLUP);
  pinMode(TL1,INPUT_PULLUP);
  pinMode(TL2,INPUT_PULLUP);
  pinMode(TL3,INPUT_PULLUP);
  pinMode(TL4,INPUT_PULLUP);
  pinMode(TL5,INPUT_PULLUP);

  attachInterrupt(TL4, menu_on_interrupt, RISING);
}

/*Driv fungoval tenhle polo-debounce, ted uz jenom jako reference*/
/*void updateParseInput(bool *_inputButtons, int8_t *_output, unsigned long *buttonPressedMillis) { 
  if(_inputButtons[0] == 1) { //tlačítko doprava       
    if((millis() - *buttonPressedMillis) > BUTTON_DEBOUNCE) {
      _output[0] += 1;
    }
    *buttonPressedMillis = millis();
  }

  if(_inputButtons[1] == 1) { //tlačítko doleva       
    if((millis() - *buttonPressedMillis) > BUTTON_DEBOUNCE) {
      _output[0] -= 1;
    }
    *buttonPressedMillis = millis();
  }

  if(_inputButtons[2] == 1) { //tlačítko nahoru       
    if((millis() - *buttonPressedMillis) > BUTTON_DEBOUNCE) {
      _output[1] += 1;
    }
    *buttonPressedMillis = millis();
  }

  if(_inputButtons[3] == 1) { //tlačítko dolu       
    if((millis() - *buttonPressedMillis) > BUTTON_DEBOUNCE) {
      _output[1] -= 1;
    }
    *buttonPressedMillis = millis();
  }

  if(_inputButtons[4] == 1) { //tlačítko enter       
    if((millis() - *buttonPressedMillis) > BUTTON_DEBOUNCE) {
      _output[2] += 1;
    }
    *buttonPressedMillis = millis();
  }

  if(_inputButtons[5] == 1) { //tlačítko esc       
    if((millis() - *buttonPressedMillis) > BUTTON_DEBOUNCE) {
      _output[2] -= 1;
    }
    *buttonPressedMillis = millis();
  }
}*/

void updateParseInput(bool *_inputButtons, bool *_lastInputButtons, int8_t *_output, unsigned long *buttonPressedMillis) { 
  for(int i = 0; i <= 6; i++) {
    if(_inputButtons[i] == 1 && _lastInputButtons[i] == 0) {
      _output[menuOutputPattern[i]] = _output[menuOutputPattern[i]] += menuInputPattern[i];
    }
    _lastInputButtons[i] = _inputButtons[i];
  }

  Serial.println();
}

void raw_updateButtons(bool *_input) { //tato funkce pouze přečte digital read (invertuje ho) a nastaví ho do vstupního listu
  _input[0] = !digitalRead(TL0); //TODO zastřelit se
  _input[1] = !digitalRead(TL1);
  _input[2] = !digitalRead(TL2);
  _input[3] = !digitalRead(TL3);
  _input[4] = !digitalRead(TL4);
  _input[5] = !digitalRead(TL5);
}

