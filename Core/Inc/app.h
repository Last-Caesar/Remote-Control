extern "C" int app();
#include "stdint.h"

class CurrentData
{
public:
	bool lock = 0;
	uint16_t adcCurrentData[3];

private:

};

