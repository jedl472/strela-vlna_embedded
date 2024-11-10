#include <U8g2lib.h>
#include <SPI.h>

#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>


U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 18 /* A4 */ , /* data=*/ 23 /* A2 */, /* CS=*/ 5 /* A3 */, /* reset=*/ U8X8_PIN_NONE);

// vytvoření instance I2C komunikace pro PN532
PN532_I2C pn532i2c(Wire);
// vytvoření instance PN532 NFC modulu
PN532 nfc(pn532i2c);

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  Serial.begin(115200);
  // zahájení komunikace s NFC modulem
  nfc.begin();
  // uložení verze desky pro kontrolu jejího připojení
  uint32_t versiondata = nfc.getFirmwareVersion();
  // kontrola načtené verze NFC modulu
  if (!versiondata) {
    Serial.println("Nebyl nalezen PN53x modul!");
    // zastavení programu
    while (1);
  }
  else {
    // vytištění informací o připojeném modulu
    Serial.print("Nalezen modul PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware verze "); Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);
  }
  // nastavení maximálního počtu pokusů o čtení NFC tagu,
  // odpovídá cca jedné sekundě
  nfc.setPassiveActivationRetries(0xFF);
  // konfigurace NFC modulu pro čtení tagů
  nfc.SAMConfig();
  // vytištění informace o začátku čtení
  Serial.println("PN532 modul nastaven, prilozte tag...");
}

void loop() {
  // vytvoření proměnných, do kterých se budou ukládat výsledky čtení
  boolean uspech;                          // úspěšné čtení
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // unikátní ID tagu
  uint8_t uidLength;                       // délka ID tagu
  // zahájení čtení tagů v okolí, výsledek čtení se uloží do nastavených proměnných
  uspech = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  // pokud je čtení úspěšné, vypíšeme získané informace
  if (uspech) {
    Serial.println("Nalezen NFC tag!");
    Serial.print("UID delka: "); Serial.print(uidLength, DEC); Serial.println(" bytu");
    Serial.print("UID hodnoty: ");
    u8g2.clearBuffer();	
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
      u8g2.drawStr(i*20, 10, &String(uid[i], HEX)[0]);
    }

    u8g2.sendBuffer();
    Serial.println("");
    // vyčkání jedné sekundy před novým čtením
    delay(1000);
  }
  else {
    // vytištění informace o vypršení času čtení
    Serial.println("Behem cteni nebyl prilozen zadny tag.");
  }
}