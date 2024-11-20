#include "display.h"

//#include <U8g2lib.h>
//#include <SPI.h>

#include "bmp.h"

/* ---------------------------- WIP --------------------------------
if((volby_dynamicMenu[0] != lastVolby_dynamicMenu[0]) || (volby_dynamicMenu[1] != lastVolby_dynamicMenu[1])) {  //tento blok pouze vykresluje na display TODO: udelat hezci
  display_u8g2.clear();

  switch(volby_dynamicMenu[0]) { 
    case 0:
      display_u8g2.drawStr(0, 10, "Nastavte typ akce: ");
      display_u8g2.drawStr(0, 32, "_");
      display_u8g2.drawStr(0, 30, String(volbyUzivatele[0]).c_str());
      display_u8g2.drawStr(20, 30, String(volbyUzivatele[1]).c_str());
      break;
    case 1:
      display_u8g2.drawStr(0, 10, "Nastavte typ ulohy: ");
      display_u8g2.drawStr(20, 33, "_");
      display_u8g2.drawStr(0, 30, String(volbyUzivatele[0]).c_str());
      display_u8g2.drawStr(20, 30, String(volbyUzivatele[1]).c_str());
      break;
    default:
      break;
  }
  display_u8g2.sendBuffer();
  lastVolby_dynamicMenu[0] = volby_dynamicMenu[0];
  lastVolby_dynamicMenu[1] = volby_dynamicMenu[1];
  lastVolby_dynamicMenu[2] = volby_dynamicMenu[2];
}*/

/*void display_startup(String msg) {
  u8g2.drawXBM(0, 0, 128, 64, bmp_strela_vlna_logo);
}*/