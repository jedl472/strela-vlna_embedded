#include "input.h"
#include "system_settings.h"

bool isMainMenuActive = 0;

bool isSetupActive = 0;
bool wifiSetupBypass = 0;

bool canBeMainMenuTurnedOn = 0;

const int8_t menuInputPattern[6] = {1, -1, 1, -1, 1, -1}; //bindovani tlacitek
const int8_t menuOutputPattern[6] = {0, 0, 1, 1, 2, 2};

void IRAM_ATTR menu_on_interrupt() { 
  if(canBeMainMenuTurnedOn == 1) { isMainMenuActive = 1; } 
  //if(DEBUG_MODE && isSetupActive) {
    wifiSetupBypass = 1;
  //}
}

void init_input() {
  pinMode(TL0,INPUT_PULLUP);
  pinMode(TL1,INPUT_PULLUP);
  pinMode(TL2,INPUT_PULLUP);
  pinMode(TL3,INPUT_PULLUP);
  pinMode(TL4,INPUT_PULLUP);
  pinMode(TL5,INPUT_PULLUP);

  attachInterrupt(TL4, menu_on_interrupt, RISING);
}

void updateParseInput(bool *_inputButtons, bool *_lastInputButtons, int8_t *_output, unsigned long *buttonPressedMillis) { 
  for(int i = 0; i < 6; i++) { //kdyz se tady vymeni < za <=, je to silenej heap overflow, staci si vyprintit jak vypada _output
   if(_inputButtons[i] == 1 && _lastInputButtons[i] == 0) {
      _output[menuOutputPattern[i]] = _output[menuOutputPattern[i]] += menuInputPattern[i];
    }
    _lastInputButtons[i] = _inputButtons[i];
  }

  /*for(int i = 0; i < 6; i++) {
    Serial.print(_output[i]);
    Serial.print(" ");
  }
  Serial.println();*/
}

void raw_updateButtons(bool *_input) { //tato funkce pouze přečte digital read (invertuje ho) a nastaví ho do vstupního listu
  _input[0] = !digitalRead(TL0); //TODO zastřelit se
  _input[1] = !digitalRead(TL1);
  _input[2] = !digitalRead(TL2);
  _input[3] = !digitalRead(TL3);
  _input[4] = !digitalRead(TL4);
  _input[5] = !digitalRead(TL5);

  
}

