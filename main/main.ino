// ---------------------------------------- Knihovny ---------------------------------------
#include <U8g2lib.h> //docasne, pak prijde do vlastniho modulu
#include <SPI.h>
#include "bmp.h"

//vlastni moduly (jsou zde i knihovny pro display, musí být importovány před knihovnamy pro nfc, jinak se program nezkompiluje)
#include "input.h"
#include "display.h"

// knihovny pro čtečku nfc
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

// wifi a http
#include <WiFi.h>
#include <HTTPClient.h>

//utilita na parsování json
#include <ArduinoJson.h>

/*
  -------------------------------------- Nastavení ---------------------------------------
*/
#define debug 1

#define display_clk 18   // Clock: RS   (označení pinů na samotné desce displeje)
#define display_data 23  // Data:  PW
#define display_cs 5     // CS:    E

const char* wifi_ssid = "GAM2";
const char* wifi_password = "JejTGame123+";
String serverName = "http://192.168.22.7:80";

/*
 --------------------------------- Globání proměnné -------------------------------------
*/

// nfc
PN532_I2C pn532i2c(Wire);
PN532 nfc_pn532(pn532i2c);

// http 
HTTPClient http;

/*
 ------------------------------------ Setup + loop --------------------------------------
*/

void setup() {
  inputSetup();
  //setup věcí pro debug, zatím v podstatě placeholder a WIP
  if (debug) {
    Serial.begin(115200);
    Serial.println("system startuje");
  }

  //setup displeje, TODO: nechat vykreslit střela vlna startovací obrazovku
  if (debug) { Serial.println("Nastavuji display"); }
  
  init_display()

  display_text("inicializace periferii");

  //setup nfc
  nfc_pn532.begin();
  uint32_t versiondata = nfc_pn532.getFirmwareVersion(); // uložení verze desky pro kontrolu jejího připojení
  if (!versiondata) {  //pokud nenajde čtečku, zastaví program;
    display_text("NFC nenalezeno");
    
    if (debug) { Serial.println("Nebyl nalezen PN53x modul!"); }
    while (true) {
      nfc_pn532.begin();
      uint32_t versiondata = nfc_pn532.getFirmwareVersion();
      if (versiondata) { break; }
    }
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


  
  display_text("inicializace hotova");
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
      if(uid[i] < 10) {
        tagIdString[i*2 + 1] = String(uid[i], HEX).charAt(0);
        tagIdString[i*2] = 48;
      } else {
        tagIdString[i*2] = String(uid[i], HEX).charAt(0);
        tagIdString[i*2 + 1] = String(uid[i], HEX).charAt(1);
      }
      Serial.println(uid[i]);
    }
    Serial.println(tagIdString);

    display_text("cekam na server");

    http.begin(serverName.c_str()); // start session

    String requestBody;

    {
      StaticJsonDocument<200> jsonContainer;
      jsonContainer["typ"] = "overeni";
      jsonContainer["id"] = tagIdString;
      serializeJson(jsonContainer, requestBody);
    }

    http.addHeader("Content-Type", "application/json");  
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode != 200) {
      display_text("server neodpovida");
    } else {
      String response_payload = http.getString();

      if(response_payload == "n") {  //struktura requestů popsaná v souboru format-komunikace.txt
        display_text("neznamy tag");

        http.end();  // neexistuje špatný tag, prostě se odpojí
      } else { //pokud projde počáteční request, může začít operátor dělat jeho magii
        Serial.println("Stav účtu: ");  //debug
        Serial.print(response_payload);

        bool amIFinished = false;
        
        char* typUlohy = "0";

        // ------------------ sem přijde všechna magie s tlačítky a dynamickými requesty ------------------
        unsigned long buttonPressedMillis = 0; // funkce vyuzivana updateParseInput kvuli debounce

        uint8_t volbyUzivatele[2] = {0, 0}; //tato promena uklada volby uzivatele, nemeni se dynamicky jako volby_dynamicMenu

        uint8_t lastVolby_dynamicMenu[3] = {-1, -1, -1}; // tyto hodnoty aby se poprve vykreslil display, uklada předchozi stav volby_dynamicMenu aby se mohl updatova display
        uint8_t volby_dynamicMenu[3] = {0, 0, 0}; //x(sipka doleva/doprava), y(sipka nahoru/dolu), potvrzení(enter/escape), meni se dynamicky funkci updateParseInput  DULEZITE: da se volne upravovat
        bool jeStisknuteTlacitko[5]; // promena pasovaná do raw_updateButtons kam se ukladaji cista data z tlacitek (pouze kvuli modularite)



        display_u8g2.clear();
        while (!amIFinished) {
          if((volby_dynamicMenu[0] != lastVolby_dynamicMenu[0]) || (volby_dynamicMenu[1] != lastVolby_dynamicMenu[1])) {  //tento blok pouze vykresluje na display TODO: udelat hezci
            if(lastVolby_dynamicMenu[0] != volby_dynamicMenu[0]) { volbyUzivatele[1] = 0; } //AKUTNE: vymyslet aby se nulovaly volby uzivatele pri prechazeni mezi menu (toto moc nefunguje)
            display_volby(&volby_dynamicMenu[0], &volbyUzivatele[0]);

            lastVolby_dynamicMenu[0] = volby_dynamicMenu[0];
            lastVolby_dynamicMenu[1] = volby_dynamicMenu[1];
            lastVolby_dynamicMenu[2] = volby_dynamicMenu[2];
          } //konec bloku vzkreslujiciho na display 


          raw_updateButtons(&jeStisknuteTlacitko[0]); //blok pro update tlačítek
          updateParseInput(&jeStisknuteTlacitko[0], &volby_dynamicMenu[0], &buttonPressedMillis);

          if(volby_dynamicMenu[0] > 2) { volby_dynamicMenu[0] = 0; }
          volbyUzivatele[volby_dynamicMenu[0]] = volby_dynamicMenu[1]; //updatuje volby uzivatele

          if(volby_dynamicMenu[0] == 2) {
            display_text("posilam data");

            amIFinished = true;

            String requestBody;
            {
              StaticJsonDocument<200> jsonContainer;
              jsonContainer["typ"] = "akce";
              jsonContainer["akce"] = String(volbyUzivatele[1]);
              jsonContainer["uloha"] = String(volbyUzivatele[0]);
              jsonContainer["id"] = tagIdString;
              
              serializeJson(jsonContainer, requestBody);
            }
            
            http.addHeader("Content-Type", "application/json");     

            http.POST(requestBody);
            http.end();

            display_text("hotovo");
          }
        } //konec fce amIFinished
      }
      //sem přijde kod po ukončení session
    }

    delay(1000); 
  }
  Serial.println("alive check");
}