// ---------------------------------------- Knihovny ---------------------------------------

// knihovny pro display, musí být importovány před knihovnamy pro nfc, jinak se program nezkompiluje
#include <U8g2lib.h>
#include <SPI.h>

// knihovny pro čtečku nfc
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

// wifi a http
#include <WiFi.h>
#include <HTTPClient.h>

/*
  -------------------------------------- Nastavení ---------------------------------------
*/

#define debug 1 // zatím jenom zapne UART

#define display_clk 18   // Clock: RS   (označení pinů na samotné desce displeje)
#define display_data 23  // Data:  PW
#define display_cs 5     // CS:    E

const char* wifi_ssid = "GAM2";
const char* wifi_password = "JejTGame123+";
String serverName = "http://192.168.22.222:80";

/*
 --------------------------------- Globání proměnné -------------------------------------
*/

// nfc
PN532_I2C pn532i2c(Wire);
PN532 nfc_pn532(pn532i2c);

// display
U8G2_ST7920_128X64_F_SW_SPI display_u8g2(U8G2_R0, display_clk, display_data, display_cs, /* reset=*/ U8X8_PIN_NONE); // TODO: zjistit co znamená U8X8_PIN_NONE a jestli je potřeba připojit reset pin (RST)

// http 
HTTPClient http;

// TODO: proměné : awaitRequest, deviceState, pressedButon[3]


/*
 ------------------------------------ Setup + loop --------------------------------------
*/

void setup() {
  //TODO: nechat ať se vypisují fáze setupu na display


  //setup věcí pro debug, zatím v podstatě placeholder a WIP
  if (debug) {
    Serial.begin(115200);
  }


  //setup displeje, TODO: nechat vykreslit střela vlna startovací obrazovku
  if (debug) { Serial.println("Nastavuji display"); }
  display_u8g2.begin();
  display_u8g2.setFont(u8g2_font_ncenB08_tr); //tady se dá zkrouhnout místo, fonty zaberou dost paměti

  display_u8g2.drawStr(0, 10, "Inicializace displeje");
  display_u8g2.sendBuffer();


  //setup nfc
  nfc_pn532.begin();
  uint32_t versiondata = nfc_pn532.getFirmwareVersion(); // uložení verze desky pro kontrolu jejího připojení
  if (!versiondata) {  //pokud nenajde čtečku, zastaví program
    if (debug) { Serial.println("Nebyl nalezen PN53x modul!"); } //TODO: nechat kreslit na display
    while (1);
  }
  nfc_pn532.setPassiveActivationRetries(0xFF); // nastavení maximálního počtu pokusů o čtení NFC tagu, odpovídá cca jedné sekundě
  nfc_pn532.SAMConfig(); // konfigurace NFC modulu pro čtení tagů


  // TODO: setup wifi a http
  if (debug) { Serial.println("Připojuji wifi"); }
  WiFi.begin(wifi_ssid, wifi_password);
  while(WiFi.status() != WL_CONNECTED) { // zastaví program dokud se nepřipojí k wifi
    delay(1);
  }
  if (debug) { Serial.println("Inicializace hotova"); }
}


void loop() {
  bool uspech;                             // úspěšné čtení
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // unikátní ID tagu, asi by mělo jít dát kratší, ale všechny možné tagy mají asi max 7 (?) pozic a chceme riskovat stack overflow?
  uint8_t uidLength;                       // délka ID tagu

  // zahájení čtení tagů v okolí, výsledek čtení se uloží do nastavených proměnných, tady to by mělo locknout program
  uspech = nfc_pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);


  if (uspech) {
    // ------------------ počáteční kontrola validity tagu se serverem: ------------------
    Serial.println("Nalezen NFC tag!");
    Serial.print("UID delka: "); Serial.print(uidLength, DEC); Serial.println(" bytu");
    Serial.print("UID hodnoty: ");

    char tagIdString[8] = ""; //proměnná, která obsahuje id tagu jako znaky, aby se dala tisknout, posílat, atd...

    for (uint8_t i = 0; i < uidLength; i++) {  //TODO: vyřešit, že když začíná id tagu na 0 hází bordel
      tagIdString[i*2] = String(uid[i], HEX).charAt(0);
      tagIdString[i*2 + 1] = String(uid[i], HEX).charAt(1);
    }
    Serial.println(tagIdString);


    http.begin(serverName.c_str()); // start session

    int httpResponseCode = http.sendRequest("POST", String(tagIdString, HEX));
    if (httpResponseCode != 200) {
      Serial.println("Nepodařilo se navázat spojení se serverem");
    }
    String payload = http.getString();

    if(payload == "n") {  //struktura requestů popsaná v souboru format-komunikace.txt
      Serial.println("Neznámý nfc tag"); 
      http.end();  // neexistuje špatný tag, prostě se odpojí
    } else { //pokud projde počáteční request, může začít operátor dělat jeho magii
      Serial.println("Stav účtu: ");  //debug
      Serial.print(payload);

      bool amIFinished = false;
      
      char* typUlohy = "0";

      // ------------------ sem přijde všechna magie s tlačítky a dynamickými requesty ------------------
      while (!amIFinished) {
        //TODO: Sem přijdou updaty tlačítek
        //A vykreslování zvolených hodnot na displej
        //taky posílání requestů

        if (/*je zmáčknuté end tlačítko?*/ true) {
          Serial.println("zabíjím session");
          amIFinished = true;
          http.sendRequest("POST", "end");
          http.end();
        }
      }
    }

    delay(1000); 
  }
}

/*
 ------------------------------------ Ostatní funkce ------------------------------------
*/
// TODO fce: poslat GET, a zkontrolovat response code -> freeze do reconnectu - info na display. Jestli se v posledních 10 sec dělal request, skipnu check