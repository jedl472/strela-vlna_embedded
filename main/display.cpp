#include "display.h"

#include "xbm.h"

#include <U8g2lib.h>
#include <SPI.h>


#define display_clk 18   // Clock: RS   (označení pinů na samotné desce displeje)
#define display_data 23  // Data:  PW
#define display_cs 5     // CS:    E

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, display_clk, display_data, display_cs, /* reset=*/ U8X8_PIN_NONE); // TODO: zjistit co znamená U8X8_PIN_NONE a jestli je potřeba připojit reset pin (RST)


const uint8_t* display_default_font = u8g2_font_ncenB08_tr;


void init_display() {
  u8g2.begin();
  u8g2.setFont(display_default_font); //tady se dá zkrouhnout místo, fonty zaberou dost paměti
}

void display_volby(uint8_t *_pozice, uint8_t *_data) {
  u8g2.clear();
  switch(*_pozice) { 
    case 0:
      u8g2.drawStr(0, 10, "Nastavte typ akce: ");
      u8g2.drawStr(0, 32, "_");
      u8g2.drawStr(0, 30, String(_data[0]).c_str());
      u8g2.drawStr(20, 30, String(_data[1]).c_str());
      break;
    case 1:
      u8g2.drawStr(0, 10, "Nastavte typ ulohy: ");
      u8g2.drawStr(20, 33, "_");
      u8g2.drawStr(0, 30, String(_data[0]).c_str());
      u8g2.drawStr(20, 30, String(_data[1]).c_str());
      break;
    default:
      break;
  }
  u8g2.sendBuffer();
}

void display_message(String msg) {
  u8g2.setFont(u8g2_font_5x8_tf);

  if(msg.length() > 0) {
    u8g2.drawXBM(0, -4, 128, 64, xbm_strela_vlna_logo);
    u8g2.setDrawColor(0);
    u8g2.drawBox(0, 59, 128, 5);
    u8g2.setDrawColor(1);
    u8g2.drawStr(0, 63, msg.c_str());
  } else {
    u8g2.drawXBM(0, 0, 128, 64, xbm_strela_vlna_logo);
  }

  u8g2.sendBuffer();
  u8g2.setFont(display_default_font);
}