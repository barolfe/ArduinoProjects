#include <Solar.h>

#define PI 3.14159265

// Constructor -- all units should be radians
Solar::Solar(float lat, float lon) {
	_lat = lat;
	_lon = lon;
}

void Solar::setDaylightParameters(float hour1, float hour2) {
	_shift_fact = (-90.0/15.0) - hour1;
	_deg_delta  = (15 * 12.0/(hour2 - hour1)) ; /// degrees
	_hour1 = hour1;
	_hour2 = hour2;
}

float Solar::getShiftFact() {
	return _shift_fact;
}

float Solar::getDegDelta() {
	return _deg_delta;
}

// Get solar insolance at a time, 24-hour format
float Solar::getInsolance(float hour) {
	float S = 1; // max solar insolance
	float H = _deg_delta * (hour + _shift_fact) * PI / 180; // hour-angle, radians
	float Z =  acos(sin(_lat)*sin(_lon) + cos(_lat)*cos(_lon)*cos(H)); // Zenith angle, radians
	float I = S * cos(Z);

	// return a value that is greater than zero, else return 0
	return I > 0 ? I : 0;
}


// This is a square-ish shaped sine-wave, it's still a sine at its core, but it gets to its peak faster
float Solar::getMoreSquareSine(float hour) {
	float S = 1;

	if ((hour <= _hour1) || (hour >= _hour2)) {
		return 0;
	}

	float x = PI * (hour - _hour1)/(_hour2 - _hour1); // get a value between 0 and PI

	float I = sin((atanh(x/(PI/2) - 1) + PI)/2);

	return I > 0 ? I : 0;
}

float Solar::getFromSine(float hour) {
	float S = 1;

	if ((hour <= _hour1) || (hour >= _hour2)) {
		return 0;
	}

	float x = PI * (hour - _hour1)/(_hour2 - _hour1); // get a value between 0 and PI

	float I = sin(x);

	return I > 0 ? I : 0;

}