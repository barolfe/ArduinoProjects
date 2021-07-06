// Header File

// Class to connect to raspberry pi and provide methods for communication and sending data of http

#ifndef Solar_H
#define Solar_H

#include <stdio.h>
#include <math.h>

class Solar 
{
public:
	Solar(float lat, float lon);
	float getInsolance(float hour);
	void setDaylightParameters(float hour1, float hour2);
	float getShiftFact();
	float getDegDelta();
	float getMoreSquareSine(float hour);
	float getFromSine(float hour);
protected:
	float _lat;
	float _lon;
	float _shift_fact = -12;
	float _deg_delta = 15; 
	float _hour1 = 7;
	float _hour2 = 12 + 7;

};

#endif // Solar_H