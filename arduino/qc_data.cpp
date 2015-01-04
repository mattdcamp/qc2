#include "qc_longTimer.h"
#include "qc_data.h"
#include "qc_logger.h"

static Semaphore attitude_sem;
static qc_attitude_t attitude;

static Semaphore position_sem;
static qc_position_t position;

static Semaphore health_sem;
static qc_gpsHealth gpsHealth;

static Semaphore command_sem;
static qc_flightCommand command;

static Semaphore motorState_sem;
static qc_motorState motorState;

void setupData() {
	logger_println("DATA: Setup...", QC_LOG_INFO);
	chSemInit(&attitude_sem, 1);
	setAttitude(0, 0, 0);

	chSemInit(&position_sem, 1);
	setPosition(0, 0, 0, 0);

	chSemInit(&health_sem, 1);
	setHealth(0, 0, 0, 0);

	chSemInit(&command_sem, 1);
	setCommand(0, 0, 0, 0, 0);

	chSemInit(&motorState_sem, 1);
	setMotorState(0, 0, 0, 0);
	logger_println("DATA: Ready!", QC_LOG_INFO);
}

// Methods for working with Attitude
qc_attitude_t getAttitude() {
	qc_attitude_t output;
	chSemWait(&attitude_sem);
	output = attitude;
	chSemSignal(&attitude_sem);
	return output;
}

void setAttitude(float pitch, float roll, float heading) {
	chSemWait(&attitude_sem);
	attitude.pitch = pitch;
	attitude.roll = roll;
	attitude.heading = heading;
	chSemSignal(&attitude_sem);
	
	logger_print("DATA: Setting Attitude: ", QC_DATA_LOG_LEVEL);
	logger_strPrintln(attitude2Csv(), QC_DATA_LOG_LEVEL);
}

void setAttitude(float height) {
	chSemWait(&attitude_sem);
	attitude.height = height;
	chSemSignal(&attitude_sem);

	logger_print("DATA: Setting Attitude: ", QC_DATA_LOG_LEVEL);
	logger_strPrintln(attitude2Csv(), QC_DATA_LOG_LEVEL);
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
	chSemWait(&position_sem);
	pos = position;
	chSemSignal(&position_sem);
	return pos;
}

void setPosition(double lat, double lng, double course, double altitiude) {
	chSemWait(&position_sem);
	position.lattitude = lat;
	position.longitude = lng;
	position.course = course;
	chSemSignal(&position_sem);

	logger_print("DATA: Setting Position: ", QC_DATA_LOG_LEVEL);
	logger_strPrintln(postion2Csv(), QC_DATA_LOG_LEVEL);
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
	chSemWait(&health_sem);
	health = gpsHealth;
	chSemSignal(&health_sem);
	return health;
}

void setHealth(uint32_t fix, uint32_t failure, uint32_t success, uint32_t satellites) {
	chSemWait(&health_sem);
	gpsHealth.fix = fix;
	gpsHealth.failure = failure;
	gpsHealth.success = success;
	gpsHealth.satellites = satellites;
	chSemSignal(&health_sem);

	logger_print("DATA: Setting Health: ", QC_DATA_LOG_LEVEL);
	logger_strPrintln(health2Csv(), QC_DATA_LOG_LEVEL);
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
	chSemWait(&command_sem);
	cmd = command;
	chSemSignal(&command_sem);
	return cmd;
}

void setCommand(float pitch, float roll, float heading, float height, float thrust) {
	chSemWait(&command_sem);
	command.pitch = pitch;
	command.roll = roll;
	command.heading = heading;
	command.height = height;
	command.thrust = thrust;
	chSemSignal(&command_sem);
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
	chSemWait(&motorState_sem);
	mc = motorState;
	chSemSignal(&motorState_sem);
	return mc;
}

void setMotorState(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4) {
	chSemWait(&motorState_sem);
	motorState.m1 = m1;
	motorState.m2 = m2;
	motorState.m3 = m3;
	motorState.m4 = m4;
	chSemSignal(&motorState_sem);

	logger_print("DATA: Setting Motor: ", QC_DATA_LOG_LEVEL);
	logger_strPrintln(motorState2Csv(), QC_DATA_LOG_LEVEL);
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