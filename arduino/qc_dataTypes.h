#include <stdint.h>

/**
*	This structure holds all the information about the angle of the craft.
*/
typedef struct {
	float pitch;		// Angle of rotation about the x axis (forward/aft tilt)
	float roll;			// Angle of rotation about the y axis (left/right tilt)
	float heading;
} qc_attitude_t;
qc_attitude_t imu_getAttitude();
String imu_attitude2Csv();

/**
*	This structure holds all the information about the position of the craft.
*/
typedef struct {
	double lattitude;
	double longitude;
	double course;
	double altitude;
} qc_position_t;
qc_position_t gps_getPosition();
String gps_postion2Csv();

/**
*	This structure holds all the information about the heath of the GPS signal.
*/
typedef struct {
	uint32_t fix;
	uint32_t failure;
	uint32_t success;
	uint32_t satellites;
} qc_gpsHealth;
qc_gpsHealth gps_getHeath();
String gps_heath2Csv();

/**
*	This structure holds all the information about commands being sent to the craft.
*/
typedef struct {
	uint8_t throttle;
	float heading;
} qc_flightCommand;
qc_flightCommand wifi_readCommand();


/**
*	This structure holds all the information about commands being sent to the motors PWM.
*/
typedef struct {
	uint8_t motor1;
	uint8_t motor2;
	uint8_t motor3;
	uint8_t motor4;
} qc_motorCommand;
qc_motorCommand ctrl_getMotorCmd();
