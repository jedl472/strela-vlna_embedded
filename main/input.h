/*
Tady jsou všechny funkce starající se o vstupy z tlačítek. Také je zde pár stavových proměnných, protože se používají hlavně u interruptu.

Je zde jeden interrupt, který hlídá skok do halvního menu (a taky wifi bypass, ale to je jenom DEBUG_MODE). Je to hack, ale 
nfc knihovna zastaví všechno když hledá tagy. Pro to je nastavená, ať se přeruší každých 200 ms a mezi tím se zkontroluje jestli proběhl interrupt.
Důležité je, aby v interruptu nebylo nic velkého, jinak to může rozbít nfc nepředvídatelným způsobem. 
*/

#include "Arduino.h"


extern bool isMainMenuActive; //je čtečka v módu hlavního menu?
extern bool wifiSetupBypass; //pokud se zapne, přeskočí se setup wifi, existuje kvůli DEBUG_MODE
extern bool canBeMainMenuTurnedOn; //dá se zapnout hlavní menu? (aby se nemohlo zapnout třeba během čtení tagu nebo během setupu)

void init_input();
/*
Samotný setup tlačítek, jsou zde pinMode funkce.
*/

void raw_updateButtons(bool *_input);
/*
Vezme vstupy z tlačítek (přes DigitalRead) a umístí je do proměnné _input (bool[6])
*/


void updateParseInput(bool *_inputButtons, bool *_lastInputButtons, int8_t *_output);
/*
Funkce převádějící vstup z tlačítek do použitelné formy. Vytvoří tři osy, jejichž hodnoty se posouvají pomocí stisků.
_output[0]: osa x, hodnoty na ní se posouvají pomocí šipky doleva a doprava
       [1]: osa y, hodnoty na ní se posouvají pomocí šipky nahoru a dolu
       [2]: osa enter/escape, hodnota se ubere při stisknutí escape a přidá při stisknutí enter
Toto chování os se definuje pomocí menuInputPattern (kam a o kolik se osy posouvají) menuOutputPattern (jaké osy). Prvky v těchto proměnných jsou jednotlivá tlačítka

_inputButtons jsou raw data z tlačítek, používá se zde výstup z funkce raw_updateButtons (bool[6])

_lastInputButtons je kvůli "debounce", aktuálně dělá pouze to, že detekuje náběžnou hranu a pak osu posune. Pokud by se implementoval debounce, TAK TADY.
*/