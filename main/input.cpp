#include "input.h"

#include "ui.h"

bool isMainMenuActive = 0;

void IRAM_ATTR menu_on_interrupt() { isMainMenuActive = 1; }

void init_input() {
  pinMode(TL0,INPUT_PULLUP);
  pinMode(TL1,INPUT_PULLUP);
  pinMode(TL2,INPUT_PULLUP);
  pinMode(TL3,INPUT_PULLUP);
  pinMode(TL4,INPUT_PULLUP);
  pinMode(TL5,INPUT_PULLUP);

  attachInterrupt(TL4, menu_on_interrupt, RISING);
}


void updateParseInput(bool *_inputButtons, uint8_t *_output, unsigned long *buttonPressedMillis) { 
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
}

void raw_updateButtons(bool *_input) { //tato funkce pouze přečte digital read (invertuje ho) a nastaví ho do vstupního listu
  _input[0] = !digitalRead(TL0); //TODO zastřelit se
  _input[1] = !digitalRead(TL1);
  _input[2] = !digitalRead(TL2);
  _input[3] = !digitalRead(TL3);
  _input[4] = !digitalRead(TL4);
  _input[5] = !digitalRead(TL5);
}

