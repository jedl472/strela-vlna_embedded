//soubor pro definovani systemovych nastaveni

//-------------------------------- obecne ----------------------------------------
#define DEBUG_MODE 1
#define ADMIN_TAG "335aa91a000000"

#define DEVICE_ID 2

inline const char* wifi_ssid = "";
inline const char* wifi_password = "";
// inline const char* wifi_ssid = "GAM2";
// inline const char* wifi_password = "JejTGame123+";
inline String serverName = "https://strela-vlna.gchd.cz/api/cash";
// inline String serverName = "http://192.168.22.7:80";


inline String wifi_name[4][2]{
  {"",""},
  {"",""},
  {"Simon774","skaut123"},
  {"GAM2","JejTGame123+"},
} ;

//--------------------------------- piny -----------------------------------------
#define display_clk 18   // Clock: RS   (označení pinů na samotné desce displeje)
#define display_data 23  // Data:  PW
#define display_cs 5     // CS:    E

#define TL0 27  //doprava
#define TL1 26  //doleva
#define TL2 14  //nahoru
#define TL3 33  //dolu
#define TL4 32  //enter
#define TL5 25  //esc
