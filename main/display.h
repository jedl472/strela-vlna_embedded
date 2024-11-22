#include "Arduino.h"

void init_display();

void display_volby(uint8_t *_pozice, uint8_t *_data);

void display_text(String msg);

void display_message(String msg);

void IRAM_ATTR menu_on_interrupt();