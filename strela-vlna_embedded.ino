// ---------------------------------------- Knihovny ---------------------------------------

// knihovny pro display, musí být importovány před knihovnamy pro nfc, jinak se program nezkompiluje
#include <U8g2lib.h>
#include <SPI.h>

// knihovny pro čtečku nfc
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

//TODO: knihovny pro wifi, http a další random věci


// -------------------------------------- Nastavení ---------------------------------------

#define debug 1 // zatím jenom zapne UART

#define display_clk 18   // Clock: RS   (označení pinů na samotné desce displeje)
#define display_data 23  // Data:  PW
#define display_cs 5     // CS:    E





// --------------------------------- Globání proměnné -------------------------------------
// nfc
PN532_I2C pn532i2c(Wire);
PN532 nfc_pn532(pn532i2c);

// display
U8G2_ST7920_128X64_F_SW_SPI display_u8g2(U8G2_R0, display_clk, display_data, display_cs, /* reset=*/ U8X8_PIN_NONE); // TODO: zjistit co znamená U8X8_PIN_NONE a jestli je potřeba připojit reset pin (RST)

// TODO: blbosti k wifině


// TODO: proměné : awaitRequest, deviceState, pressedButon[3]



// ------------------------------------ Setup + loop --------------------------------------

void setup() {
  //TODO: nechat ať se vypisují fáze setupu na display

  //setup věcí pro debug, zatím v podstatě placeholder a WIP
  if (debug) {
    Serial.begin(115200);
  }

  //setup displeje, TODO: nechat vykreslit střela vlna startovací obrazovku
  display_u8g2.begin();
  display_u8g2.setFont(u8g2_font_ncenB08_tr); //tady se dá zkrouhnout místo, fonty zaberou dost paměti

  //setup nfc
  nfc_pn532.begin();
  uint32_t versiondata = nfc_pn532.getFirmwareVersion(); // uložení verze desky pro kontrolu jejího připojení
  if (!versiondata) {  //pokud nenajde čtečku, zastaví program
    if (debug) { Serial.println("Nebyl nalezen PN53x modul!"); } //TODO: nechat kreslit na display
    while (1);
  }

  // TODO: setup wifi a http
}


void loop() {
  /*
    Tady to je trochu na diskuzi, zatím nechám pro inspiraci. Mělo by locknout program, dokud nenajde tag (ještě dělá sekundové pauzy mezi čteními), 
    pak pokračuje v ifu "uspech". Je to přesně ten způsob na prasaka, který jsem kritizoval. Na druhou stranu, chceme si ztěžovat život čtením 
    dokumentace k PN532.h? (jasně, když to je locklý, tak se nedá kontrolovat třeba připojení k wifi. Otázkou je, jestli nás to zajímá)
  */

  bool uspech;                             // úspěšné čtení
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // unikátní ID tagu, asi by mělo jít dát kratší, ale všechny možné tagy mají asi max 7 (?) pozic a chceme riskovat stack overflow?
  uint8_t uidLength;                       // délka ID tagu

  // zahájení čtení tagů v okolí, výsledek čtení se uloží do nastavených proměnných, tady to by mělo locknout program
  uspech = nfc_pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);

  /*
    Tady do této funkce program vstoupí, jestli se načte tag.

    Jak bych to tedy udělal: Na začátku program projde setupem a začne loopovat. V momentě, kdy se načte tag 
    (program vstoupí do tohodle if (uspech)) se otevře http session a vstoupí do další smyčky. Tam se bude dít:
      1. Aktualizace načtených hodnot (prostě ta funkce s tlačítkama)
      2. Následné posílání requestů
      3. Kontrola, jestli je http session pořád otevřená (pro případ, kdyby to operátor zapnul, načetl, došel si na kafe a pak vypadla síť).
      4. Profit ???

    Co je tady napsaný je zatím jenom testovací placeholder, pak by to mělo být obsažené ideálně v nějakém debug obalu
  */
  if (uspech) {
    Serial.println("Nalezen NFC tag!");
    Serial.print("UID delka: "); Serial.print(uidLength, DEC); Serial.println(" bytu");
    Serial.print("UID hodnoty: ");
    display_u8g2.clearBuffer();	
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
      display_u8g2.drawStr(i*20, 10, &String(uid[i], HEX)[0]);
    }
    display_u8g2.sendBuffer();

    // vyčkání jedné sekundy před novým čtením (řekl bych, že kdybych smazal, uvařím si USB hub)
    delay(1000);
  }

}


// ------------------------------------ Ostatní funkce ------------------------------------
// TODO fce: poslat GET, a zkontrolovat response code -> freeze do reconnectu - info na display. Jestli se v posledních 10 sec dělal request, skipnu check
// TODO fce: update tlačítek
// TODO fce: poslat request dle awaitRequest