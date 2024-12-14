#include "ui.h"

#include "input.h"
#include <Vector.h>


struct Option {
  String name;
  void (*linkFunc)();
}

class ui_staticMenu() {
  private:
    uint8_t numberOfOptions = 0;
    typedef Vector<Option> options;
    void (*displayFunction)(*Option, uint8_t);

  public:
    ui_staticMenu((*_displayFunction)(*Option, uint8_t), uint8_t _numberOfOptions) { //constructor
      numberOfOptions = _numberOfOptions;
      displayFunction = _displayFunction;
    }

    void addOption(Option *option) {
      
      options.push_back(option])
    }

    void enterMenu() {
      int8_t buttonAxis[3] = {0, 0, 0}; //x(sipka doleva/doprava), y(sipka nahoru/dolu), potvrzení(enter/escape)

      bool last_isButtonPressed[5] = {0, 0, 0, 0, 0};
      bool isButtonPressed[5];

      while(true) {
        raw_updateButtons(&isButtonPressed[0]); //blok pro update tlačítek
        updateParseInput(&isButtonPressed[0], &last_isButtonPressed[0], &buttonAxis[0]);

        if(buttonAxis[2] == -1) {
          buttonAxis[2] = 0;
          break;
        }

        if(buttonAxis[2] == 1) {
          buttonAxis[2] = 0;

          (*options[buttonAxis[1]].linkFunc)()
        }

      }
    }
}