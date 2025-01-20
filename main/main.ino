/*
Hlavní soubor, zde je hlavní struktura programu a dále:
 - všechna logika pro UI
 - čtení NFC

Všechny použité knihovny v kódu se dají najít v strela-vlna_embedded/libraries.zip
*/


// ---------------------------------------- Knihovny ---------------------------------------
//vlastni moduly (jsou zde i knihovny pro display - display.h, musí být importovány před knihovnamy pro nfc, jinak se program nezkompiluje)
#include "input.h"
#include "display.h"
#include "system_settings.h"

// knihovny pro čtečku nfc
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>

//komunikace
#include <WiFi.h>
#include "requests.h"  //musi byt volana na tomto miste, protoze knihovna pro requesty se jinak nezkompiluje

#include <StreamUtils.h> //knihovna používaná kvůli zápisu do EEPROM

/*
 --------------------------------- Globání proměnné -------------------------------------
*/
// nfc
PN532_I2C pn532i2c(Wire);
PN532 nfc_pn532(pn532i2c);

// extern int8_t type_of_buttone_menu =1;



/*
 ------------------------------------ Setup + loop --------------------------------------
*/
void setup() {
  /*
  Následuje setup všeho, co jde. Snaží se tisknout na display pomocí display_message() všechny kroky co se dějí
  */
  if (DEBUG_MODE) {
    Serial.begin(115200);
    Serial.println("system startuje");
  }

  EEPROM.begin(512);

  EepromStream eepromStream(0, 510); //vyčtení poslední akce z EEPROM
  deserializeJson(posledniAkce, eepromStream);
  init_input();
  EepromStream s(510, 515);
  // if (s.readString().toInt()){
    type_of_buttone_menu = s.readString().toInt();
  // }
  // else {
  //   Serial.println(s.readString().toInt());
  // }

  if (DEBUG_MODE) { Serial.println("Nastavuji display"); }
  
  init_display();

  display_message("Inicializuji periferie");

  //setup nfc
  nfc_pn532.begin();
  uint32_t versiondata = nfc_pn532.getFirmwareVersion(); // uložení verze desky pro kontrolu jejího připojení
  if (!versiondata) {  //pokud nenajde čtečku, zastaví program;
    display_message("NFC nenalezeno");
    
    if (DEBUG_MODE) { Serial.println("Nebyl nalezen PN53x modul!"); }
    while (true) { //pokud se čtečka nenalezne napoprvé, zkouší to znovu. Těžko říct, jestli to je bug, protože nikdo nečte dokumentaci ke knihovnám.
      nfc_pn532.begin();
      uint32_t versiondata = nfc_pn532.getFirmwareVersion();
      if (versiondata) { break; }
    }
  }


  nfc_pn532.setPassiveActivationRetries(0x40); // nastavení maximálního počtu pokusů o čtení NFC tagu, odpovídá cca 250ms (255 odpovida cca 1 sekunde)
  nfc_pn532.SAMConfig(); // konfigurace NFC modulu pro čtení tagů

  if (DEBUG_MODE) { Serial.println("Připojuji wifi"); }

  WiFi.begin(wifi_name[1][0].c_str(), wifi_name[1][1]);
  
  uint32_t millis_start = millis();
  display_message("Pripojuji wifi");
  while(WiFi.status() != WL_CONNECTED && (millis() - millis_start) < 5000) {} // zastaví program dokud se nepřipojí k wifi, nebo neubehne 5s

  if (WiFi.status() != WL_CONNECTED){

    int8_t volby_dynamicMenu[3] = {0, 0, 0}; //x(sipka doleva/doprava), y(sipka nahoru/dolu), potvrzení(enter/escape), meni se dynamicky funkci updateParseInput  DULEZITE: da se volne upravovat      int8_t last_volbyY = 0;

    bool last_jeStisknuteTlacitko[6] = {0, 0, 0, 0, 0, 0};
    bool jeStisknuteTlacitko[6];

    while (volby_dynamicMenu[2] == 0 ){
      display_wifi_menu(volby_dynamicMenu[1]);

      raw_updateButtons(&jeStisknuteTlacitko[0]); //blok pro update tlačítek
      updateParseInput(&jeStisknuteTlacitko[0], &last_jeStisknuteTlacitko[0], &volby_dynamicMenu[0]);
    }
    if (volby_dynamicMenu[2] == 1){
      Serial.println(volby_dynamicMenu[1]);
      String wifi_ssid = wifi_name[volby_dynamicMenu[1]][0];
      String wifi_password = wifi_name[volby_dynamicMenu[1]][1];
      WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    }
  }

  


  if (DEBUG_MODE) { Serial.println("Inicializace hotova"); }

  if(DEBUG_MODE) {
    display_message("DEBUG_MODE active");
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
    String tagIdString = "00000000000000"; //proměnná, která obsahuje id tagu jako znaky, aby se dala tisknout, posílat, atd...

    for (uint8_t i = 0; i < 7; i++) {
      if(uid[i] < 16) {
        tagIdString.setCharAt(i*2 + 1, String(uid[i], HEX).charAt(0));
        //tagIdString.setCharAt(i*2, '0');
      } else {
        tagIdString.setCharAt(i*2, String(uid[i], HEX).charAt(0));
        tagIdString.setCharAt(i*2 + 1, String(uid[i], HEX).charAt(1));
      }
    }
    
    if (DEBUG_MODE) {
      Serial.println();
      Serial.print("ID_length: "); Serial.print(uidLength, DEC);
      Serial.print(" ID: ");
      Serial.println(tagIdString);
    }

    if(tagIdString == ADMIN_TAG) {
      display_message("ADMIN_TAG");
    } else {
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
          delay(500);
        } else { //pokud projde počáteční request, může začít operátor dělat jeho magii

          if(DEBUG_MODE) {
            Serial.print("Nazev tymu: ");
            Serial.println(nazevTymu);

            Serial.print("Stav účtu: ");
            Serial.println(stavUctu);
          }
          

          bool amIFinished = false;
          
          char* typUlohy = "0";

          // ------------------ akce menu ------------------

          uint8_t volbyUzivatele[3] = {1, 1, 1}; //tato promena uklada volby uzivatele, nemeni se dynamicky jako volby_dynamicMenu, vykresluje se na display

          int8_t volby_dynamicMenu[3] = {1, 0, 0}; //x(sipka doleva/doprava), y(sipka nahoru/dolu), potvrzení(enter/escape), meni se dynamicky funkci updateParseInput  DULEZITE: da se volne upravovat
          int8_t last_volbyY = 0;

          bool last_jeStisknuteTlacitko[6] = {0, 0, 0, 0, 0, 0};
          bool jeStisknuteTlacitko[6] ;
          uint8_t nas_menu[6] = {1, 2, 0, 1, 2, 0};


          display_clear();

          while (!amIFinished) { //smycka kde operator nastavuje hodnoty
            display_cteni_menu(&volby_dynamicMenu[1], &volbyUzivatele[0], nazevTymu, stavUctu);

            raw_updateButtons(&jeStisknuteTlacitko[0]); //blok pro update tlačítek
            // for(int i = 0; i < 6; i++){
            // Serial.print(i);
            // Serial.print(jeStisknuteTlacitko[i]);
            // }
            // Serial.println(volbyUzivatele[2]);
            // Serial.println();

            if (type_of_buttone_menu == 0){
              updateParseInput(&jeStisknuteTlacitko[0], &last_jeStisknuteTlacitko[0], &volby_dynamicMenu[0]);
              
              if(last_volbyY != volby_dynamicMenu[1]) {  //pri prechazeni na ose y aby se spravne nastavovaly volbyUzivatele. Pouziva last_volbyY pro detekci zmeny
                volbyUzivatele[last_volbyY] = volby_dynamicMenu[0];
                volby_dynamicMenu[0] = volbyUzivatele[volby_dynamicMenu[1]];
                last_volbyY = volby_dynamicMenu[1];
              } else {
                volbyUzivatele[volby_dynamicMenu[1]] = volby_dynamicMenu[0];
              }

              if(volby_dynamicMenu[1] > 2) { volby_dynamicMenu[1] = 0; } if(volby_dynamicMenu[1] < 0) { volby_dynamicMenu[1] = 2; } //omezeni jednotlivych vstupnich os
              if((volby_dynamicMenu[0] > 2) && (volby_dynamicMenu[1] != 2)) { volby_dynamicMenu[0] = 0; } if ((volby_dynamicMenu[1] == 2) && (volby_dynamicMenu[0] > 9)) { volby_dynamicMenu[0] = 9; }
              if((volby_dynamicMenu[0] < 0) && (volby_dynamicMenu[1] != 2)) { volby_dynamicMenu[0] = 2; } if ((volby_dynamicMenu[1] == 2) && (volby_dynamicMenu[0] < 1)) { volby_dynamicMenu[0] = 1; }
              // if((volby_dynamicMenu[0] > 2)) { volby_dynamicMenu[0] = 0; } if ((volby_dynamicMenu[0] > 9) && (volby_dynamicMenu[1] = 2)) { volby_dynamicMenu[0] = 9; }
              // if((volby_dynamicMenu[0] < 0)) { volby_dynamicMenu[0] = 2; } if ((volby_dynamicMenu[0] < 1) && (volby_dynamicMenu[1] = 2)) { volby_dynamicMenu[0] = 1; }

            // Serial.print(volby_dynamicMenu[0]);
            // Serial.print(volby_dynamicMenu[1]);
            // Serial.println(volby_dynamicMenu[2]);
              
              delay(10);
            }
            else{
              uint8_t help_buffer[3] = {1,0,0};
              uint8_t game_buffer[6] = {0,0,0,0,0,0};
              for(uint8_t i = 0;i < 6;i++){
                if (jeStisknuteTlacitko[i] == 1){
                  // Serial.print(i);
                  game_buffer[help_buffer[0]] = i;
                  help_buffer[0] += 1;
                  if (i<3){
                    help_buffer[1] += 1;
                  }
                  else{
                    help_buffer[2] += 1;
                  }

                }
              }
              if ((help_buffer[1] == 1) && (help_buffer[2] == 1)){
                for(uint8_t i = 0;i<3; i++){
                  if (jeStisknuteTlacitko[i] == 1){
                    volbyUzivatele[1] = nas_menu[i];
                    // Serial.print(nas_menu[i]);
                  }
                }
                for(uint8_t i = 3;i<6; i++){
                  if (jeStisknuteTlacitko[i] == 1){
                    volbyUzivatele[0] = nas_menu[i];
                    // Serial.print(i);
                    // Serial.println(nas_menu[i]);
                  }
                }
                volby_dynamicMenu[2] = 1;
              }

            // Serial.print(volbyUzivatele[0]);
            // Serial.print(volbyUzivatele[1]);
            // Serial.println(volbyUzivatele[2]);
              bool option[2] = {1,1};
              bool exit[6] = {0,0,0,1,0,1};
              bool back[6] = {0,1,1,0,0,0};
              for (int i = 0;  i < 6; i++) {
                if( jeStisknuteTlacitko[i] != exit[i] ) {
                  option[0] = 0;
                }
                if( jeStisknuteTlacitko[i] != back[i] ) {
                  option[1] = 0;
                } }
              if(option[0]){
                volby_dynamicMenu[2] = -1;
              }
              if(option[1]){
                if(posledniAkce["typ"] == "akce") {

                int16_t httpResponseCode = request_vratit(posledniAkce);
                posledniAkce["typ"] = "";

                EepromStream eepromStream(0, 512);
                serializeJson(posledniAkce, eepromStream);
                EEPROM.commit();
                
                display_clear();
                if(httpResponseCode != 200) {
                  display_message("chyba serveru, neodeslano");
                } else {
                  // display_message("");
                }
                
                }
              }

            // Serial.println();
              }



            if(volby_dynamicMenu[2] >= 1) {
              display_message("posilam data");

              posledniAkce["tym"] = nazevTymu;

              delay(500);

              String response_payload; //odpoved od serveru
              int16_t httpResponseCode = request_akce(&response_payload, tagIdString, volbyUzivatele[1], volbyUzivatele[0], volbyUzivatele[2]);

              EepromStream eepromStream(0, 512);
              serializeJson(posledniAkce, eepromStream);
              EEPROM.commit();

              JsonDocument jsonResponse;
              deserializeJson(jsonResponse, response_payload);

              if (type_of_buttone_menu != 1){
              amIFinished = true;
              }
              else{
                volby_dynamicMenu[2] = 0;
              }

              display_clear();
              
              if(httpResponseCode != 200) {
                display_message("chyba serveru, neodeslano"); // nechceme dat misto neodeslano error code?
              } else if (jsonResponse["key"] == "n") {
                display_message("tym nema dostatek penez");
              } else if (jsonResponse["key"] == "k") {
                // display_message("");
              } else {
                display_message("chyba");
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
  }

  if(isMainMenuActive) { // vstupovani do main menu (pomocí interruptu v input.cpp)
    canBeMainMenuTurnedOn = 0;
    display_clear();

    uint8_t volbyUzivatele[2] = {0, 0}; //tato promena uklada volby uzivatele, nemeni se dynamicky jako volby_dynamicMenu

    int8_t volby_dynamicMenu[3] = {0, 0, 0}; //x(sipka doleva/doprava), y(sipka nahoru/dolu), potvrzení(enter/escape), meni se dynamicky funkci updateParseInput  DULEZITE: da se volne upravovat
    uint8_t menu_uroven = 0;

    bool last_jeStisknuteTlacitko[5];
    bool jeStisknuteTlacitko[5];

    while(isMainMenuActive) {// smycka v main menu
      uint8_t minmax[2] = {0,4}; // vrchní a spodní element listu
      raw_updateButtons(&jeStisknuteTlacitko[0]); //blok pro update tlačítek
      updateParseInput(&jeStisknuteTlacitko[0], &last_jeStisknuteTlacitko[0], &volby_dynamicMenu[0]);
      if(volby_dynamicMenu[1] < minmax[0]) { volby_dynamicMenu[1] = minmax[1]; } if(volby_dynamicMenu[1] > minmax[1]) { volby_dynamicMenu[1] = minmax[0]; } //omezeni os
      if((volby_dynamicMenu[1] > 1)&&(menu_uroven == 4)) { volby_dynamicMenu[1] = 0; }

      if(volby_dynamicMenu[2] >= 1) { //zmacknuti enter
        if(menu_uroven == 0) { //0. uroven - menu
          if(volby_dynamicMenu[1] == 0) {
            isMainMenuActive = 0;
            display_message("");
          } else if(volby_dynamicMenu[1] == 1) {
            if(posledniAkce["typ"] == "akce") {
              menu_uroven = 2;
            } else {
              display_message("posledni akce neexistuje");
              isMainMenuActive = 0;
              menu_uroven = 99;
            }
          } else if(volby_dynamicMenu[1] == 2) {
            volby_dynamicMenu[1] = 0;
            menu_uroven = 1;
          } else if(volby_dynamicMenu[1] == 3) {
            volby_dynamicMenu[1] = 3;
            uint8_t minmax_wifi[2] = {0,3};

            bool vstupf = true;
            while (volby_dynamicMenu[2] == 0 || vstupf || !last_jeStisknuteTlacitko[4]){
              display_wifi_menu(volby_dynamicMenu[1]);

              raw_updateButtons(&jeStisknuteTlacitko[0]); //blok pro update tlačítek
              updateParseInput(&jeStisknuteTlacitko[0], &last_jeStisknuteTlacitko[0], &volby_dynamicMenu[0]);
              vstupf = false;
              if(volby_dynamicMenu[1] < minmax_wifi[0]) { volby_dynamicMenu[1] = minmax_wifi[1]; } if(volby_dynamicMenu[1] > minmax_wifi[1]) { volby_dynamicMenu[1] = minmax_wifi[0]; }
            }
            Serial.println(volby_dynamicMenu[1]);
            String wifi_ssid = wifi_name[volby_dynamicMenu[1]][0];
            String wifi_password = wifi_name[volby_dynamicMenu[1]][1];
            WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
            if(volby_dynamicMenu[1] < minmax[0]) { volby_dynamicMenu[1] = minmax[0]; } if(volby_dynamicMenu[1] > minmax[1]) { volby_dynamicMenu[1] = minmax[1]; }

            isMainMenuActive = 0;
            menu_uroven = 99;
          } else if(volby_dynamicMenu[1] == 4){
            // if type_of_buttone_menu =
            volby_dynamicMenu[1] = 0;
            menu_uroven = 4;
          }
        } else if(menu_uroven == 2 ) { //2. uroven - vraceni akce
          if(posledniAkce["typ"] == "akce") {

            int16_t httpResponseCode = request_vratit(posledniAkce);
            posledniAkce["typ"] = "";

            EepromStream eepromStream(0, 512);
            serializeJson(posledniAkce, eepromStream);
            EEPROM.commit();
            
            if(httpResponseCode != 200) {
              display_message("chyba serveru, neodeslano");
            } else {
              display_message("");
            }
            
            isMainMenuActive = 0;
            menu_uroven = 99;
          }
        }
        else if(menu_uroven == 4){
          EepromStream s(510, 512);
          if (volby_dynamicMenu[1] == 0){
            type_of_buttone_menu = 0;
            s.print("00");
          }
          if (volby_dynamicMenu[1] == 1){
            type_of_buttone_menu = 1;
            s.print("01");
          }
          s.flush();
          Serial.println(type_of_buttone_menu);
          menu_uroven = 0;
          volby_dynamicMenu[1] = 4;

          
          

        }
      } else if(volby_dynamicMenu[2] < 0) { //zmacknuti esc
        if(menu_uroven == 0) {
          isMainMenuActive = 0;
          display_message("");
        } else if(menu_uroven == 1) {
          menu_uroven = 0;
        } else if(menu_uroven == 2) {
          menu_uroven = 0;
        } else if(menu_uroven == 4) {
          menu_uroven = 0;
          volby_dynamicMenu[1] = 4;
        }
      } else {
        display_info_menu(menu_uroven, volby_dynamicMenu[1], posledniAkce["uloha"], posledniAkce["akce"], posledniAkce["tym"], WiFi.localIP().toString(), WiFi.gatewayIP().toString(), serverName);
      }
      volby_dynamicMenu[2] = 0;
    }

    // delay(200);
    canBeMainMenuTurnedOn = 1;
  }
}
