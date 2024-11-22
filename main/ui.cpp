#include "ui.h"

#include "input.h"
#include "display.h"

/*void ui_main_menu() {
  unsigned long buttonPressedMillis = 0; // funkce vyuzivana updateParseInput kvuli debounce

  uint8_t volbyUzivatele[2] = {0, 0}; //tato promena uklada volby uzivatele, nemeni se dynamicky jako volby_dynamicMenu

  uint8_t lastVolby_dynamicMenu[3] = {-1, -1, -1}; // tyto hodnoty aby se poprve vykreslil display, uklada předchozi stav volby_dynamicMenu aby se mohl updatova display
  uint8_t volby_dynamicMenu[3] = {0, 0, 0}; //x(sipka doleva/doprava), y(sipka nahoru/dolu), potvrzení(enter/escape), meni se dynamicky funkci updateParseInput  DULEZITE: da se volne upravovat
  bool jeStisknuteTlacitko[5];

  for (;;) {
    if(isMainMenuActive) {
      raw_updateButtons(&jeStisknuteTlacitko[0]); //blok pro update tlačítek
      updateParseInput(&jeStisknuteTlacitko[0], &volby_dynamicMenu[0], &buttonPressedMillis);

      display_info_menu();
      
    }
    Serial.println("test");
  }

}*/