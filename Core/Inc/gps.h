#ifndef GPS_H
#define GPS_H
#include "stdint.h"

class GPS
{
public:
	int Init();
	int Handler();
	double latitude = 0;   //широта
	double longitude = 0;   //долгота
	int nSatellite = 0; //кол. спутников
	double altitude = 0; //высота над уровнем моря
	double speed = 0;
	char timeStr[15] = { 0 }; //время
	uint8_t hours = 0;
	uint8_t minutes = 0;
	uint8_t seconds = 0;
private:

};


#endif // !GPS_H
