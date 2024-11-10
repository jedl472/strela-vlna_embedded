#include <U8glib.h>

// nastavení propojovacích pinů
#define EN 18
#define RW 19
#define RS 23

// inicializace OLED displeje z knihovny U8glib
U8GLIB_ST7920_128X64_1X lcd(EN, RW, RS);

// proměnná pro uchování času poslední obnovy displeje
long int prepis = 0;

void setup(void) {
  // nastavení bílé barvy pro výpis
  if ( lcd.getMode() == U8G_MODE_R3G3B2 ) {
    lcd.setColorIndex(255);
  }
  // nastavení maximální intenzity svitu
  else if ( lcd.getMode() == U8G_MODE_GRAY2BIT ) {
    lcd.setColorIndex(3);
  }
  // nastavení viditelnosti pixelů
  else if ( lcd.getMode() == U8G_MODE_BW ) {
    lcd.setColorIndex(1);
  }
  // pro otočení displeje o 180 stupňů
  // stačí odkomentovat řádek níže
  // lcd.setRot180();
}

void loop(void) {
  // porovnání uloženého a aktuálního času,
  // při rozdílu větším než 100 ms se provede
  // obnovení displeje, čas můžeme nastavit dle potřeby
  if (millis()-prepis > 100) {
    // následující skupina příkazů
    // obnoví obsah OLED displeje
    lcd.firstPage();
    do {
      // funkce vykresli vykreslí žádanou obsah
      vykresli();
    } while( lcd.nextPage() );
    // uložení posledního času obnovení
    prepis = millis();
  }
  
  // zde je místo pro další příkazy pro Arduino
  
  // volitelná pauza 10 ms pro demonstraci
  // vykonání dalších příkazů
  delay(10);
}
// funkce vykresli pro nastavení výpisu informací na OLED
void vykresli(void) {
  // nastavení písma, toto písmo umožní vypsat
  // přibližně 15x4 znaků
  lcd.setFont(u8g_font_unifont);
  // nastavení pozice výpisu v pixelech
  // souřadnice jsou ve tvaru x, y
  // souřadnice 0, 0 je v levém horní rohu
  // OLED displeje, maximum je 128, 64
  lcd.setPrintPos(0, 10);
  // výpis textu na zadanou souřadnici
  lcd.print("Arduino navody");
  lcd.setPrintPos(0, 25);
  lcd.print("dratek.cz");
  lcd.setPrintPos(0, 40);
  lcd.print("Cas od zapnuti:");
  lcd.setPrintPos(40, 55);
  lcd.print(millis()/1000);
  lcd.print(" vterin");
}