#ifndef APP_H
#define APP_H

extern "C" int app();
#include "stdint.h"

int map(int x, int in_min, int in_max, int out_min, int out_max);
double map(double x, double in_min, double in_max, double out_min, double out_max);

class CurrentData
{
public:
	bool lock = 0;
	uint16_t adcCurrentData[3];

private:

};

#endif // !APP_H