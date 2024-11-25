#include "Arduino.h"


#define display_clk 18   // Clock: RS   (označení pinů na samotné desce displeje)
#define display_data 23  // Data:  PW
#define display_cs 5     // CS:    E

void init_display();

void display_cteni_menu(int8_t *_pozice, uint8_t *_data, String _nazevTymu, String _stavUctu);

void display_message(String msg);

void display_info_menu(uint8_t display_page, uint8_t cursor_position, String posledniAkce_uloha, String posledniAkce_typ, String posledniAkce_tym, String ip, String gw, String srv);

void display_clear();

String ulohaToString(String uloha);

String akceToString(String akce);