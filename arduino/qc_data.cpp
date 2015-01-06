#include "qc_longTimer.h"
#include "qc_data.h"
#include "qc_logger.h"

static Mutex attitude_mutex;
static qc_attitude_t attitude = {};

static Mutex position_mutex;
static qc_position_t position = {};

static Mutex health_mutex;
static qc_gpsHealth gpsHealth = {};

static Mutex command_mutex;
static qc_flightCommand command = {};

static Mutex motor_mutex;
static qc_motorState motorState = {};

void setupData() {
	logger_println("DATA: Setup...", QC_LOG_INFO);
	
	chMtxInit(&attitude_mutex);
	chMtxInit(&position_mutex);
	chMtxInit(&health_mutex);
	chMtxInit(&command_mutex);
	chMtxInit(&motor_mutex);

	logger_println("DATA: Ready!", QC_LOG_INFO);
}

// Methods for working with Attitude
qc_attitude_t getAttitude() {
	qc_attitude_t output;
	chMtxLock(&attitude_mutex);
	output = attitude;
	chMtxUnlock();
	return output;
}

void setAttitude(float pitch, float roll, float heading) {
	chMtxLock(&attitude_mutex);
	attitude.pitch = pitch;
	attitude.roll = roll;
	attitude.heading = heading;
	chMtxUnlock();
}

void setAttitude(float height) {
	chMtxLock(&attitude_mutex);
	attitude.height = height;
	chMtxUnlock();
}

String attitude2Csv() {
	qc_attitude_t a = getAttitude();

	String output = "";
	output += "attitude,";

	output += String(a.pitch, 10);
	output += ",";

	output += String(a.roll, 10);
	output += ",";

	output += String(a.heading, 10);
	output += ",";

	output += String(a.height, 10);
	output += ",";

	output += String(longTimer_getTime());

	return output;
}

// Functions to do with GPS Position
qc_position_t getPosition() {
	qc_position_t pos;
	chMtxLock(&position_mutex);
	pos = position;
	chMtxUnlock();
	return pos;
}

void setPosition(double lat, double lng, double course, double altitiude) {
	chMtxLock(&position_mutex);
	position.lattitude = lat;
	position.longitude = lng;
	position.course = course;
	chMtxUnlock();
}

String postion2Csv() {
	qc_position_t position = getPosition();
	String output;
	output += "gps,";

	output += String(position.lattitude, 6);
	output += ',';

	output += String(position.longitude, 6);
	output += ',';

	output += String(position.altitude, 6);
	output += ',';

	output += String(position.course, 6);
	output += ",";

	output += String(longTimer_getTime());

	return output;
}


// Functions to do with GPS Health
qc_gpsHealth getHealth() {
	qc_gpsHealth health;
	chMtxLock(&health_mutex);
	health = gpsHealth;
	chMtxUnlock();
	return health;
}

void setHealth(uint32_t fix, uint32_t failure, uint32_t success, uint32_t satellites) {
	chMtxLock(&health_mutex);
	gpsHealth.fix = fix;
	gpsHealth.failure = failure;
	gpsHealth.success = success;
	gpsHealth.satellites = satellites;
	chMtxUnlock();
}

String health2Csv() {
	qc_gpsHealth health = getHealth();
	String output;
	output += "gps,";

	output += String(health.fix);
	output += ',';

	output += String(health.failure);
	output += ',';

	output += String(health.success);
	output += ',';

	output += String(health.satellites);
	output += ',';

	output += String(longTimer_getTime());

	return output;
}

// Functions to do with Commands
qc_flightCommand getCommand() {
	qc_flightCommand cmd;
	chMtxLock(&command_mutex);
	cmd = command;
	chMtxUnlock();
	return cmd;
}

void setCommand(float pitch, float roll, float heading, float height, float thrust) {
	chMtxLock(&command_mutex);
	command.pitch = pitch;
	command.roll = roll;
	command.heading = heading;
	command.height = height;
	command.thrust = thrust;
	chMtxUnlock();
}

String command2Csv() {
	qc_flightCommand cmd = getCommand();
	String output;
	output += "cmd,";

	output += String(cmd.pitch);
	output += ',';

	output += String(cmd.roll);
	output += ',';

	output += String(cmd.heading, 6);
	output += ',';

	output += String(cmd.height);
	output += ',';

	output += String(cmd.thrust);
	output += ',';

	output += String(longTimer_getTime());

	return output;
}

// Functions to do with the state of the motors
qc_motorState getMotorState() {
	qc_motorState mc;
	chMtxLock(&motor_mutex);
	mc = motorState;
	chMtxUnlock();
	return mc;
}

void setMotorState(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
	chMtxLock(&motor_mutex);
	motorState.m1 = m1;
	motorState.m2 = m2;
	motorState.m3 = m3;
	motorState.m4 = m4;
	chMtxUnlock();
}

String motorState2Csv() {
	qc_motorState mc = getMotorState();
	String output;
	output += "motor,";

	output += String(mc.m1);
	output += ',';

	output += String(mc.m2);
	output += ',';

	output += String(mc.m3);
	output += ',';

	output += String(mc.m4);
	output += ',';

	output += String(longTimer_getTime());

	return output;
}