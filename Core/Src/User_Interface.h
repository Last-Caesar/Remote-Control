#ifndef U_I_H
#define U_I_H
#include "main.h"

class U_I
{
public:
	int Init();
	int Handler();
	int PrintScreen1();
	int print_pack_A(int nSatellite, double latitude, double longitude, double batteryVoltage);
	int print_pack_B(double altitude, double speed, char* timeStr);
	int Print_res_stat(bool stat);
	int Print_tr_stat(bool stat);
	int Print_bat_volt(double volt);
	int PrintScreen2();
	int print_adc_Channel(uint16_t* adcDataChannel);
	int print_buttons_press(uint16_t* lButtonsPress, uint16_t* rButtonsPress);
	int print_buttons_hold(uint16_t* lButtonsHold, uint16_t* rButtonsHold, bool* lButtonsIsHold, bool* rButtonsIsHold);
	int currentScreen = 0;
	int previousScreen = 0;
private:

};



#endif