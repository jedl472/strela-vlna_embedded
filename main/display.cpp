#include <sys/_stdint.h>
#include "display.h"
#include "system_settings.h"

#include "xbm.h"

#include <U8g2lib.h>
#include <SPI.h>

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, display_clk, display_data, display_cs, /* reset=*/ U8X8_PIN_NONE); // TODO: zjistit co znamená U8X8_PIN_NONE a jestli je potřeba připojit reset pin (RST)


const uint8_t* display_default_font = u8g2_font_ncenB08_tr;


void init_display() {
  u8g2.begin();
  u8g2.setFont(display_default_font); //tady se dá zkrouhnout místo, fonty zaberou dost paměti
}

void display_cteni_menu(int8_t *_pozice, uint8_t *_data, String _nazevTymu, String _stavUctu) {
 /* u8g2.drawStr(0, 0, "                                      ");
  switch(*_pozice) { 
    case 0:
      u8g2.drawStr(0, 10, "Nastavte typ akce: ");
      u8g2.drawStr(0, 32, "_");
      break;
    case 1:
      u8g2.drawStr(0, 10, "Nastavte typ ulohy: ");
      u8g2.drawStr(20, 33, "_");
      break;
    default:
      break;
  }

  u8g2.drawStr(5, 30, ulohaToString(String(_data[0])).c_str());
  u8g2.drawStr(30, 30, akceToString(String(_data[1])).c_str());*/

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawFrame(-1, 0, 130, 12);
  u8g2.drawStr(10, 10, "Nastavte parametry");

  u8g2.setFont(u8g2_font_5x8_tf);
  u8g2.drawStr(0, 21, "Nazev tymu: ");
  u8g2.drawStr(64, 21, _nazevTymu.c_str());
  u8g2.drawStr(0, 31, "Stav uctu: ");
  u8g2.drawStr(64, 31, _stavUctu.c_str());
  u8g2.drawStr(0, 39, "------------------------------------------------------------");
  u8g2.drawStr(5, 47, "Typ ulohy: ");
  u8g2.drawStr(64, 47, ulohaToString(String(_data[0])).c_str());
  u8g2.drawStr(5, 57, "Typ akce:  ");

  u8g2.setDrawColor(0);
  u8g2.drawBox(64, 49, 64, 10);
  u8g2.setDrawColor(1);

  u8g2.drawStr(64, 57, akceToString(String(_data[1])).c_str());


  u8g2.setDrawColor(0);
  u8g2.drawBox(0, 40, 5, 30);
  u8g2.setDrawColor(1);
  
  u8g2.drawStr(0, ((1-(*_pozice))*10)+47, ">");

  u8g2.sendBuffer();
  u8g2.setFont(display_default_font);
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

void display_info_menu(uint8_t display_page, uint8_t cursor_position, String posledniAkce_uloha, String posledniAkce_typ, String posledniAkce_tym, String ip, String gw, String srv) {
  u8g2.setFont(u8g2_font_5x8_tf);
  //u8g2.drawXBM(64, 12, 80, 40, xbm_strela_vlna_logo_small);

  switch(display_page) {
    case 0:
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 0, 128, 64);
      u8g2.setDrawColor(1);

      u8g2.drawXBM(64, 25, 80, 40, xbm_strela_vlna_logo_small);
      u8g2.drawStr(5, 10, "exit");
      u8g2.drawStr(5, 20, "vratit posledni akci");
      u8g2.drawStr(5, 30, "system info");
      u8g2.drawStr(5, 40, "wifi");


      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 0, 5, 30);
      u8g2.setDrawColor(1);

      u8g2.drawStr(0, ((3-cursor_position)*10)+10, ">");

      break;

    case 1:
      u8g2.setFont(u8g2_font_profont10_tf );

      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 0, 128, 64);
      u8g2.setDrawColor(1);
      //u8g2.drawXBM(64, 12, 80, 40, xbm_strela_vlna_logo_small);
      u8g2.drawStr(0, 10, "verze: " + String(VERSION_INFO));

      u8g2.drawStr(100, 10, "ID:");
      u8g2.drawStr(117, 10, String(DEVICE_ID).c_str());

      u8g2.drawStr(0, 25, "IP:");
      u8g2.drawStr(0, 35, "GW:");
      u8g2.drawStr(0, 50, "server:");

      u8g2.drawStr(20, 25, ip.c_str());
      u8g2.drawStr(20, 35, gw.c_str());
      u8g2.drawStr(0, 60, srv.c_str());

      u8g2.setFont(u8g2_font_5x8_tf);
      break;

    case 2:
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 0, 128, 64);
      u8g2.setDrawColor(1);

      u8g2.drawStr(5, 10, "Prejete si vratit akci:");

      u8g2.drawStr(5, 30, ulohaToString(posledniAkce_uloha).c_str());
      u8g2.drawStr(30, 30, akceToString(posledniAkce_typ).c_str());


      break;
    case 3:
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 0, 128, 64);
      u8g2.setDrawColor(1);

      u8g2.drawStr(5, 10, "Prejete DJHsi vratit akci:");

      u8g2.drawStr(5, 30, ulohaToString(posledniAkce_uloha).c_str());
      u8g2.drawStr(30, 30, akceToString(posledniAkce_typ).c_str());


      break;
    default:
      break;
  }

  u8g2.sendBuffer();
  u8g2.setFont(display_default_font);
}

void display_wifi_menu(uint8_t cursor_position){
  u8g2.setFont(u8g2_font_5x8_tf);
  u8g2.setDrawColor(0);
  u8g2.drawBox(0, 0, 128, 64);
  u8g2.setDrawColor(1);

  u8g2.drawXBM(64, 25, 80, 40, xbm_strela_vlna_logo_small);
  for(int i = 1;i<5;i++){
    u8g2.drawStr(5, i*10, wifi_name[4-i][0].c_str()); //chyba
    // u8g2.drawStr(5, i*10, "test"); //chyba
  }

  u8g2.setDrawColor(0);
  u8g2.drawBox(0, 0, 5, 30);
  u8g2.setDrawColor(1);

  u8g2.drawStr(0, ((3-cursor_position)*10)+10, ">");
  u8g2.sendBuffer();
  u8g2.setFont(display_default_font);
}

void display_clear() {
  u8g2.clear();
}

String ulohaToString(String uloha) {
  if(uloha == "0") {
    return("A");
  } else if(uloha == "1") {
    return("B");
  } else if(uloha == "2") {
    return("C");
  } else {
    return("n");
  }
}

String akceToString(String akce) {
  if(akce == "0") {
    return("Vyresit");
  } else if(akce == "1") {
    return("Koupit");
  } else if(akce == "2") {
    return("Prodat");
  } else {
    return("n");
  }
}