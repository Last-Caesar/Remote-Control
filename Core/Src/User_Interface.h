#ifndef U_I_H
#define U_I_H
#include "main.h"

class U_I
{
public:
	int Init();
	int Handler();
	int PrintScreen1();
	int PrintScreen2();
	int print_adc_Channel(uint16_t *adcDataChannel);
	int print_buttons_press(uint8_t* lButtonsPress, uint8_t* rButtonsPress);
	int print_buttons_hold(uint8_t *lButtonsHold, uint8_t *rButtonsHold);
	int currentScreen = 0;
	int previousScreen = 0;
private:

};



#endif