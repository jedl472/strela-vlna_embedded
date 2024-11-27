// ---------------------------------------- Knihovny ---------------------------------------
//vlastni moduly (jsou zde i knihovny pro display - display.h, musí být importovány před knihovnamy pro nfc, jinak se program nezkompiluje)
#include "input.h"
#include "display.h"
#include "system_settings.h"

// knihovny pro čtečku nfc
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

// wifi
#include <WiFi.h>

#include "requests.h"  //musi byt volana na tomto miste, protoze knihovna pro requesty se jinak nezkompiluje

/*
 --------------------------------- Globání proměnné -------------------------------------
*/
// nfc
PN532_I2C pn532i2c(Wire);
PN532 nfc_pn532(pn532i2c);

/*
 ------------------------------------ Setup + loop --------------------------------------
*/

void setup() {

  //setup globalnich promennych
  posledniAkce["akce"] = "";
  posledniAkce["uloha"] = "";
  posledniAkce["id"] = "";

  init_input();
  //setup věcí pro debug, zatím v podstatě placeholder a WIP
  if (DEBUG_MODE) {
    Serial.begin(115200);
    Serial.println("system startuje");
  }

  //setup displeje, TODO: nechat vykreslit střela vlna startovací obrazovku
  if (DEBUG_MODE) { Serial.println("Nastavuji display"); }
  
  init_display();

  display_message("Inicializuji periferie");

  //setup nfc
  nfc_pn532.begin();
  uint32_t versiondata = nfc_pn532.getFirmwareVersion(); // uložení verze desky pro kontrolu jejího připojení
  if (!versiondata) {  //pokud nenajde čtečku, zastaví program;
    display_message("NFC nenalezeno");
    
    if (DEBUG_MODE) { Serial.println("Nebyl nalezen PN53x modul!"); }
    while (true) {
      nfc_pn532.begin();
      uint32_t versiondata = nfc_pn532.getFirmwareVersion();
      if (versiondata) { break; }
    }
  }

  nfc_pn532.setPassiveActivationRetries(0x40); // nastavení maximálního počtu pokusů o čtení NFC tagu, odpovídá cca jedné sekundě
  nfc_pn532.SAMConfig(); // konfigurace NFC modulu pro čtení tagů


  display_message("Pripojuji wifi");

  isSetupActive = 1;
  if (DEBUG_MODE) { Serial.println("Připojuji wifi"); }
  WiFi.begin(wifi_ssid, wifi_password);
  while(WiFi.status() != WL_CONNECTED && !wifiSetupBypass) { // zastaví program dokud se nepřipojí k wifi
    delay(1);
  }
  isSetupActive = 0;

  if (DEBUG_MODE) { Serial.println("Inicializace hotova"); }

  if(DEBUG_MODE) {
    if(wifiSetupBypass) {
      display_message("wifi bypass DEBUG_MODE");
      delay(500);
    } else {
      display_message("DEBUG_MODE active");
    }
  } else {
    display_message("");
  }
  canBeMainMenuTurnedOn = 1;
}


void loop() {
  bool uspech;                             // úspěšné čtení
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // unikátní ID tagu, asi by mělo jít dát kratší, ale všechny možné tagy mají asi max 7 (?) pozic a chceme riskovat stack overflow?
  uint8_t uidLength;                       // délka ID tagu

  // zahájení čtení tagů v okolí, výsledek čtení se uloží do nastavených proměnných, tady to by mělo locknout program
  uspech = nfc_pn532.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);


  if (uspech) {
    canBeMainMenuTurnedOn = 0;

    // ------------------ počáteční kontrola validity tagu se serverem: ------------------
    Serial.println("Nalezen NFC tag!");
    Serial.print("UID delka: "); Serial.print(uidLength, DEC); Serial.println(" bytu");
    Serial.print("UID hodnoty: ");

    String tagIdString = "00000000"; //proměnná, která obsahuje id tagu jako znaky, aby se dala tisknout, posílat, atd...

    for (uint8_t i = 0; i < 4; i++) {
      if(uid[i] < 10) {
        tagIdString.setCharAt(i*2 + 1, String(uid[i], HEX).charAt(0));
        tagIdString.setCharAt(i*2, '0');
      } else {
        tagIdString.setCharAt(i*2, String(uid[i], HEX).charAt(0));
        tagIdString.setCharAt(i*2 + 1, String(uid[i], HEX).charAt(1));
      }
    } Serial.println(tagIdString);

    display_message("cekam na server");

    String response_payload;
    int16_t httpResponseCode = request_overeni(&response_payload, tagIdString);

    if (httpResponseCode != 200) {
      if (DEBUG_MODE) { display_message(String("server vratil kod ") + String(httpResponseCode));} else { display_message("server neodpovida"); }
      delay(500);

      canBeMainMenuTurnedOn = 1;
    } else {
      JsonDocument jsonResponse;
      deserializeJson(jsonResponse, response_payload);

      String nazevTymu = jsonResponse["nazev"];
      String stavUctu = jsonResponse["penize"];

      if(jsonResponse["key"] == "n") {  //struktura requestů popsaná v souboru format-komunikace.txt
        display_message("neznamy tag");
      } else { //pokud projde počáteční request, může začít operátor dělat jeho magii
        Serial.print("Nazev tymu: ");  //debug
        Serial.println(nazevTymu);

        Serial.print("Stav účtu: ");  //debug
        Serial.println(stavUctu);

        bool amIFinished = false;
        
        char* typUlohy = "0";

        // ------------------ sem přijde všechna magie s tlačítky a dynamickými requesty ------------------
        unsigned long buttonPressedMillis = 0; // funkce vyuzivana updateParseInput kvuli debounce

        uint8_t volbyUzivatele[2] = {0, 0}; //tato promena uklada volby uzivatele, nemeni se dynamicky jako volby_dynamicMenu

        int8_t volby_dynamicMenu[3] = {0, 0, 0}; //x(sipka doleva/doprava), y(sipka nahoru/dolu), potvrzení(enter/escape), meni se dynamicky funkci updateParseInput  DULEZITE: da se volne upravovat
        int8_t last_volbyY = 0;

        bool last_jeStisknuteTlacitko[5] = {0, 0, 0, 0, 0};
        bool jeStisknuteTlacitko[5]; // promena pasovaná do raw_updateButtons kam se ukladaji cista data z tlacitek (hlavne kvuli modularite)

        
        display_clear();

        while (!amIFinished) {
          display_cteni_menu(&volby_dynamicMenu[1], &volbyUzivatele[0], nazevTymu, stavUctu);

          raw_updateButtons(&jeStisknuteTlacitko[0]); //blok pro update tlačítek
          updateParseInput(&jeStisknuteTlacitko[0], &last_jeStisknuteTlacitko[0], &volby_dynamicMenu[0], &buttonPressedMillis);

          if(volby_dynamicMenu[1] > 1) { volby_dynamicMenu[1] = 1; } if(volby_dynamicMenu[1] < 0) { volby_dynamicMenu[1] = 0; }
          if(volby_dynamicMenu[0] > 2) { volby_dynamicMenu[0] = 2; } if(volby_dynamicMenu[0] < 0) { volby_dynamicMenu[0] = 0; }

          if(last_volbyY != volby_dynamicMenu[1]) {
            volbyUzivatele[1-last_volbyY] = volby_dynamicMenu[0];
            volby_dynamicMenu[0] = volbyUzivatele[1-volby_dynamicMenu[1]];
            last_volbyY = volby_dynamicMenu[1];
          } else {
            volbyUzivatele[1-volby_dynamicMenu[1]] = volby_dynamicMenu[0];
          }


          if(volby_dynamicMenu[2] >= 1) {
            display_message("posilam data");

            posledniAkce["tym"] = nazevTymu;

            delay(500);

            String response_payload;
            int16_t httpResponseCode = request_akce(&response_payload, tagIdString, volbyUzivatele[1], volbyUzivatele[0]);

            amIFinished = true;

            display_message("poslano");
            if(httpResponseCode != 200) {
              display_message("chyba serveru, neodeslano");
            } else {
              display_message("");
            }
          } else if(volby_dynamicMenu[2] <= -1) {
            amIFinished = true;
            display_message("");
          }
        } //konec smycky amIFinished
      }
      //sem přijde kod po ukončení session
      canBeMainMenuTurnedOn = 1;
    }
  }

  if(isMainMenuActive) {
    canBeMainMenuTurnedOn = 0;
    display_clear();
    unsigned long buttonPressedMillis = 0; // funkce vyuzivana updateParseInput kvuli debounce

    uint8_t volbyUzivatele[2] = {0, 0}; //tato promena uklada volby uzivatele, nemeni se dynamicky jako volby_dynamicMenu

    int8_t volby_dynamicMenu[3] = {0, 2, 0}; //x(sipka doleva/doprava), y(sipka nahoru/dolu), potvrzení(enter/escape), meni se dynamicky funkci updateParseInput  DULEZITE: da se volne upravovat
    uint8_t menu_uroven = 0;

    bool last_jeStisknuteTlacitko[5];
    bool jeStisknuteTlacitko[5];

    while(isMainMenuActive) {
      raw_updateButtons(&jeStisknuteTlacitko[0]); //blok pro update tlačítek
      updateParseInput(&jeStisknuteTlacitko[0], &last_jeStisknuteTlacitko[0], &volby_dynamicMenu[0], &buttonPressedMillis);
      if(volby_dynamicMenu[1] < 0) { volby_dynamicMenu[1] = 0; } if(volby_dynamicMenu[1] > 2) { volby_dynamicMenu[1] = 2; }

      if(volby_dynamicMenu[2] >= 1) {
        if(menu_uroven == 0) {
          if(volby_dynamicMenu[1] == 2) {
            isMainMenuActive = 0;
          } else if(volby_dynamicMenu[1] == 1) {
            if(posledniAkce["typ"] == "akce") {
              menu_uroven = 2;
            } else {
              display_message("posledni akce neexistuje");
              delay(1500);
              isMainMenuActive = 0;
              menu_uroven = 99;
            }
          } else if(volby_dynamicMenu[1] == 0) {
            menu_uroven = 1;
          }
        } else if(menu_uroven == 2 ) {
          if(posledniAkce["typ"] == "akce") {

            int16_t httpResponseCode = request_vratit(posledniAkce);
            
            if(httpResponseCode != 200) {
              display_message("chyba serveru, neodeslano");
              delay(1500);
            } else {
              display_message("");
            }
            
            isMainMenuActive = 0;
            menu_uroven = 99;
          }
        }
      } else if(volby_dynamicMenu[2] <= -1) {
        if(menu_uroven == 0) {
          isMainMenuActive = 0;
        } else if(menu_uroven == 1) {
          menu_uroven = 0;
        } else if(menu_uroven == 2) {
          menu_uroven = 0;
        }
      }
      volby_dynamicMenu[2] = 0;

      display_info_menu(menu_uroven, volby_dynamicMenu[1], posledniAkce["uloha"], posledniAkce["akce"], posledniAkce["tym"], WiFi.localIP().toString(), WiFi.gatewayIP().toString(), serverName);
    }
    display_message("");

    delay(200);
    canBeMainMenuTurnedOn = 1;
  }
}