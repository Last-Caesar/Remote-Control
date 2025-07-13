#include "gps.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

extern UART_HandleTypeDef huart2; // GPS

extern char Gps_IncomByte;
extern char Gps_RsStr[83];
extern bool Gps_IsStrComplete;

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
		//printf("%s", Gps_RsStr);
		if (!strncmp(Gps_RsStr, "$GPGGA", 6))
		{
			char latitudeStr[15] = { 0 }; //������
			char longitudeStr[15] = { 0 }; //�������
			char altitudeStr[15] = { 0 }; //������ ��� ������� ����

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
			char speedStr[15] = { 0 };
			sscanf(Gps_RsStr, "%*[^,],%[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%[^,]", timeStr, speedStr);

			double speedInKnots = 0; //�������� � �����
			speedInKnots = atof(speedStr);
			speed = speedInKnots * 1.852;

			char hoursStr[3];
			char minutesStr[3];
			char secondsStr[3];

			hoursStr[0] = timeStr[0];
			hoursStr[1] = timeStr[1];
			hoursStr[2] = '0';

			minutesStr[0] = timeStr[2];
			minutesStr[1] = timeStr[3];
			minutesStr[2] = '0';

			secondsStr[0] = timeStr[4];
			secondsStr[1] = timeStr[5];
			secondsStr[2] = '0';

			hours = atof(hoursStr);
			minutes = atof(minutesStr);
			seconds = atof(secondsStr);
		}

	}
	return 0;
}
