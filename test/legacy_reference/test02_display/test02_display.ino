#include <U8g2lib.h>
#include <SPI.h>

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18 /* A4 */ , /* data=*/ 23 /* A2 */, /* CS=*/ 5 /* A3 */, /* reset=*/ U8X8_PIN_NONE);


void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 40, "bottom text");
  u8g2.sendBuffer();




  Serial.begin(115200);
}

void loop() {
  u8g2.clearBuffer();
  //u8g2.setCursor(0, 10);
  u8g2.drawStr(0, 10, "ahoj svete");
  u8g2.drawStr(0, 40, "bottom text");
  u8g2.sendBuffer();

  delay(1000);

  u8g2.clearBuffer();
  //u8g2.setCursor(64, 10);
  u8g2.drawStr(64, 10, "tady ctecka");
  u8g2.drawStr(0, 40, "bottom text");
  u8g2.sendBuffer();
  
  delay(1000);
}