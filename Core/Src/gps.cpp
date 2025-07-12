#include "gps.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

extern UART_HandleTypeDef huart2; // GPS

extern char Gps_IncomByte;
extern char Gps_RsStr[83];
extern bool Gps_IsStrComplete;

static char latitudeStr[15] = { 0 }; //широта
static char longitudeStr[15] = { 0 }; //долгота
static char altitudeStr[15] = { 0 }; //высота над уровнем моря
static double speedInKnots = 0; //скорость в узлах
static char speedStr[15] = { 0 };
static char time[15] = { 0 }; //время

int GPS::Init()
{
	HAL_UART_Receive_IT(&huart2, (uint8_t*)&Gps_IncomByte, 1);
	return 0;
}

int GPS::Handler()
{
	if (Gps_IsStrComplete)
	{
		Gps_IsStrComplete = 0;
		printf("%s", Gps_RsStr);
		if (!strncmp(Gps_RsStr, "$GPGGA", 6))
		{
			sscanf(Gps_RsStr, "%*[^,],%*[^,],%[^,],%*[^,],%[^,],%*[^,],%*[^,],%d,%*[^,],%[^,]", latitudeStr, longitudeStr, &nSatellite, altitudeStr);
			latitude = atof(latitudeStr);
			latitude = (int)latitude / 100 + (double)(latitude - ((int)latitude / 100) * 100) / 60;
			longitude = atof(longitudeStr);
			longitude = (int)longitude / 100 + (double)(longitude - ((int)longitude / 100) * 100) / 60;
			altitude = atof(altitudeStr);
			//				float b_lat = 57.881857;
			//				float b_long = 45.769519;
			//				float distance = map.findDistance(gpsOut.latitude, gpsOut.longitude, b_lat, b_long);

			/*char str[45];
			sprintf(str, "$%d.%d,%d.%d,%d.%d,%d,%d.%d\n",
				(int)gpsOut.latitude, (int)((gpsOut.latitude - (int)gpsOut.latitude) * 100000),
				(int)gpsOut.longitude, (int)((gpsOut.longitude - (int)gpsOut.longitude) * 100000),
				(int)gpsOut.altitude, (int)((gpsOut.altitude - (int)gpsOut.altitude) * 100),
				gpsOut.nSatellite,
				(int)gpsOut.speed, (int)((gpsOut.speed - (int)gpsOut.speed) * 100));

			HAL_UART_Transmit_IT(&huart2, (uint8_t*)str, strlen(str));*/
		}
		else if (!strncmp(Gps_RsStr, "$GPRMC", 6))
		{
			sscanf(Gps_RsStr, "%*[^,],%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,]", time, speedStr);
			speedInKnots = atof(speedStr);
			speed = speedInKnots * 1.852;

		}

	}
	return 0;
}
