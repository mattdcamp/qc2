#include <stdint.h>

void setupData();

/**
*	This structure holds all the information about the angle of the craft.
*/
typedef struct {
	float pitch;		// Angle of rotation about the x axis (forward/aft tilt)
	float roll;			// Angle of rotation about the y axis (left/right tilt)
	float heading;		// Magnetic heading (yaw)
	float height;		// Hight off the ground. Altitidue is caculated by the GPS and stored in position. This is used for low level flight.
} qc_attitude_t;

/**
*	This structure holds all the information about the position of the craft.
*/
typedef struct {
	double lattitude;
	double longitude;
	double course;
	double altitude;
} qc_position_t;

/**
*	This structure holds all the information about the heath of the GPS signal.
*/
typedef struct {
	uint32_t fix;
	uint32_t failure;
	uint32_t success;
	uint32_t satellites;
} qc_gpsHealth;

/**
*	This structure holds all the information about commands being sent to the craft.
*/
typedef struct {
	uint8_t throttle;
	float pitch;
	float roll;
	float heading;
	float height;
	float thrust;
} qc_flightCommand;


typedef struct {
	uint16_t m1;
	uint16_t m2;
	uint16_t m3;
	uint16_t m4;
} qc_motorState;