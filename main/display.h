#include <sys/_stdint.h>
/*
V tomto souboru jsou všechny grafické funkce, jakékoliv přímé vykreslování na display (s knihovnou u8g2) se děje tady.
Pokud chcete přidat nějakou obrazovku, aby se něco vykreslovalo, tak založit tady jako funkci nebo přidat do nějaké stávající.

Také když chcete upravit vzhled (ne funkci, to se děje v main.ino) něčeho, tak v tomto souboru. 

Funkce nepřistupují ke globálním proměnným, všechny zobrazující se informace se vkládájí při zavolání funkcí.
Funkce se volají stále dokola, tzn neni nijak řešené hlídání jestli se něco změnilo a také je důležité, 
aby žádná z nich nepoužívala u8g2.clear()

Zde se také importuje xbm.h, kde je všechna grafika.
*/

#include "Arduino.h"


void init_display();
/*
Inicializuje knihovnu u8g2 a nastaví font. Volá se v setup().
*/

void display_cteni_menu(int8_t *_pozice, uint8_t *_data, String _nazevTymu, String _stavUctu);
/*
Stará se o display při načtení tagu.
_pozice je pozice cursoru
_data je le list o 2 prvcích obsahující aktuálně uživatelem navolenou úlohu a akci
_nazevTymu a _stavUctu jsou ze serveru přijaté hodnoty, které se zobrazí nahoře na displeji
*/

void display_message(String msg);
/*
jednoduchá funkce, která zobrazí logo střely-vlny a pod ním text v proměnné msg
pokud msg = "", tak se uprostřed zobrazí pouze logo, používá se na "reset" displeje
aktuálně se díky ní zobrazují všechny informace o stavu zařízení a občas debug

Volá se pouze jednou
*/


void display_info_menu(uint8_t display_page, uint8_t cursor_position, String posledniAkce_uloha, String posledniAkce_typ, String posledniAkce_tym, String ip, String gw, String srv);
/*
Stará se o display v hlavním menu.

Má několik "módů", display_page určuje jaký se zobrazuje. O přepínání mezi nimi se stará logika v main.ino.
display_page -> 0: přepínací obrazovka mezi exit, vrátit poslední akci, system info
             -> 1: system info
             -> 2: vracení poslední akce

cursor_position je pozice kurzoru, v každém "módu" se chová jinak

Zbytek proměnných jsou jenom informace, co se zobrazují. Mohly by být globální, ale toto mi přijde hezčí. 
*/

void display_wifi_menu(uint8_t cursor_position);

//neni cas

void display_clear();
/*
Když je někde potřeba vyčistit display, tak toto zavolá u8g2.clear(). Existuje protože se u8g2 neimportuje do celého programu.
*/

String ulohaToString(String uloha);
String akceToString(String akce);
/*
Překonvertuje String typ akce na String název akce ("1" -> "Koupit") nebo String číslo úlohy na String název úlohy ("1" -> "B")
Input je String bere protože se používá při parsování json kde se jako string ukládá.
Jsou v display.h protože se používají pouze kvůli zobrazování. Jinak se data ukládají jako čísla.
*/