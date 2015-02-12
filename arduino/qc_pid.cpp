#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <stdint.h>
#include <Servo.h>

#include "qc_data.h"

#include "qc_pid.h"

typedef struct {
	float errorSum;
	float previousError;
	float lastUpdate;
	float p;
	float i;
	float d;
} qc_pid_t;


static qc_pid_t pitchPid;
static qc_pid_t rollPid;
static qc_pid_t headingPid;
static qc_pid_t heightPid;

static Servo m1;
static Servo m2;
static Servo m3;
static Servo m4;

static float rearPitchBias;

static void initPid(qc_pid_t* pid, uint16_t p, uint16_t i, uint16_t d);
static void doPid();
static void doMotor();
static float updatePid(qc_pid_t* pid, float target, float actual, float thrust);
static float fmap(float x, float in_min, float in_max, float out_min, float out_max);
static float update(float thrust, float pitch, float roll);

uint16_t pid_setup() {
	setCommand(0, 0, 0, 0, 0);

	initPid(&pitchPid, 5, 0.5, 0);
	initPid(&rollPid, 0, 0, 0);
	initPid(&headingPid, 0, 0, 0);
	initPid(&heightPid, 0, 0, 0);

	rearPitchBias = 89.75 / 222.68;
}

uint16_t motor_setup() {
	m1.attach(2);
	m2.attach(3);
	m3.attach(4);
	m4.attach(5);

	m1.writeMicroseconds(QC_MOTOR_MIN);
	m2.writeMicroseconds(QC_MOTOR_MIN);
	m3.writeMicroseconds(QC_MOTOR_MIN);
	m4.writeMicroseconds(QC_MOTOR_MIN);
}

msg_t pid_thread_method(void *arg) {
	chRegSetThreadName("PID");
	chThdSleepMilliseconds(30000);	// Delay to let sensors init

	while (!chThdShouldTerminate()) {
		doPid();
		doMotor();
		chThdSleepMilliseconds(QC_PID_MS);
	}
}

static void doPid() {
	qc_attitude_t attitude = getAttitude();
	qc_flightCommand_t command = getCommand();

	uint16_t m1 = 0;
	uint16_t m2 = 0;
	uint16_t m3 = 0;
	uint16_t m4 = 0;

	float pitch = updatePid(&pitchPid, command.pitch, attitude.pitch, command.thrust);
	//float roll = updatePid(&rollPid, command.roll, attitude.roll, command.thrust);
	//float heading = updatePid(&headingPid, command.heading, attitude.heading);
	//float thrust = updatePid(&heightPid, command.height, attitude.height);

	/*
	uint16_t m1 = QC_MOTOR_MIN + command.thrust + pitch - roll - heading;					// Front (positive pitch), Left (negative roll), Turns Clockwise (negitive yaw)
	uint16_t m2 = QC_MOTOR_MIN + command.thrust + pitch + roll + heading;					// Front (positive pitch), Right (positive roll), Turns Counter Clockwise (positive yaw)
	uint16_t m3 = QC_MOTOR_MIN + command.thrust - (pitch * rearPitchBias) + roll - heading;	// Rear (negitive pitch), Right (positive roll), Turns Clockwise (negitive yaw)
	uint16_t m4 = QC_MOTOR_MIN + command.thrust - (pitch * rearPitchBias) - roll + heading;	// Rear (negitive pitch), Left (negitive roll), Turns Counter Clockwise (positive yaw)
	*/

	m1 = update(command.thrust, pitch, 0);						// Front (positive pitch), Left (negative roll), Turns Clockwise (negitive yaw)
	m2 = update(command.thrust, pitch, 0);						// Front (positive pitch), Right (positive roll), Turns Counter Clockwise (positive yaw)
	m3 = update(command.thrust, -(pitch * rearPitchBias), 0);	// Rear (negitive pitch), Right (positive roll), Turns Clockwise (negitive yaw)
	m4 = update(command.thrust, -(pitch * rearPitchBias), 0);	// Rear (negitive pitch), Left (negitive roll), Turns Counter Clockwise (positive yaw)

	setMotorState(m1, m2, m3, m4);
}

static void initPid(qc_pid_t* pid, uint16_t p, uint16_t i, uint16_t d) {
	pid->p = p;
	pid->i = i;
	pid->d = d;
	pid->errorSum = 0;
	pid->lastUpdate = 0;
	pid->previousError = 0;
}

static float updatePid(qc_pid_t* pid, float target, float actual, float thrust) {
	float now = getNow();
	float dt = (float)getDeltaTime(now, pid->lastUpdate) * 0.001f;
	pid->lastUpdate = now;

	float error = (thrust > 0) ? target - actual : 0;

	float p = error;
	
	pid->errorSum += (error * dt);
	float i = pid->errorSum;

	float deltaError = pid->previousError - error;
	float d = deltaError / dt;


	pid->previousError = error;

	float output = (p * pid->p) + (i * pid->i) + (d * pid->d);

	Serial.print(p);
	Serial.print(",");
	Serial.print(i);
	Serial.print(",");
	Serial.print(d);
	Serial.print(",");
	Serial.println(output);

	return output;
}

static float update(float thrust, float pitch, float roll) {
	float output = thrust + pitch + roll;
	if (thrust == 0 || output < 50) {
		output = 0;
	}
	if (output > thrust*2) {
		output = thrust*2;
	}

	return output;
}

void resetPid() {
	pitchPid.errorSum = 0;
	rollPid.errorSum = 0;
	headingPid.errorSum = 0;
	heightPid.errorSum = 0;
}

static void doMotor() {
	qc_motorState_t motorState = getMotorState();

	m1.writeMicroseconds(motorState.m1 + QC_MOTOR_MIN);
	m2.writeMicroseconds(motorState.m2 + QC_MOTOR_MIN);
	m3.writeMicroseconds(motorState.m3 + QC_MOTOR_MIN);
	m4.writeMicroseconds(motorState.m4 + QC_MOTOR_MIN);
}