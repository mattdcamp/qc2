#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <stdint.h>

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

static float rearPitchBias;

static void initPid(qc_pid_t* pid, uint16_t p, uint16_t i, uint16_t d);
static float updatePid(qc_pid_t* pid, float target, float actual);
static float fmap(float x, float in_min, float in_max, float out_min, float out_max);

uint16_t pid_setup() {
	setCommand(0, 0, 0, 0, 0);

	initPid(&pitchPid, 3, 0, 0);
	initPid(&rollPid, 3, 0, 0);
	initPid(&headingPid, 1, 0, 0);
	initPid(&heightPid, 3, 0, 0);

	rearPitchBias = 89.75 / 222.68;
}

msg_t pid_thread_method(void *arg) {
	chRegSetThreadName("PID");
	
	chThdSleepMilliseconds(30000);	// Delay to let sensors init

	while (!chThdShouldTerminate()) {
		qc_attitude_t attitude = getAttitude();
		qc_flightCommand_t command = getCommand();

		float pitch = updatePid(&pitchPid, command.pitch, attitude.pitch);
		float roll = updatePid(&rollPid, command.roll, attitude.roll);
		float heading = updatePid(&headingPid, command.heading, attitude.heading);
		//float thrust = updatePid(&heightPid, command.height, attitude.height);

		/*
		uint16_t m1 = QC_MOTOR_MIN + command.thrust + pitch - roll - heading;					// Front (positive pitch), Left (negative roll), Turns Clockwise (negitive yaw)
		uint16_t m2 = QC_MOTOR_MIN + command.thrust + pitch + roll + heading;					// Front (positive pitch), Right (positive roll), Turns Counter Clockwise (positive yaw)
		uint16_t m3 = QC_MOTOR_MIN + command.thrust - (pitch * rearPitchBias) + roll - heading;	// Rear (negitive pitch), Right (positive roll), Turns Clockwise (negitive yaw)
		uint16_t m4 = QC_MOTOR_MIN + command.thrust - (pitch * rearPitchBias) - roll + heading;	// Rear (negitive pitch), Left (negitive roll), Turns Counter Clockwise (positive yaw)
		*/

		uint16_t m1 = QC_MOTOR_MIN + command.thrust + pitch - roll;										// Front (positive pitch), Left (negative roll), Turns Clockwise (negitive yaw)
		uint16_t m2 = QC_MOTOR_MIN + command.thrust + pitch + roll;										// Front (positive pitch), Right (positive roll), Turns Counter Clockwise (positive yaw)
		uint16_t m3 = QC_MOTOR_MIN + command.thrust - (pitch * rearPitchBias) + roll;					// Rear (negitive pitch), Right (positive roll), Turns Clockwise (negitive yaw)
		uint16_t m4 = QC_MOTOR_MIN + command.thrust - (pitch * rearPitchBias) - roll;					// Rear (negitive pitch), Left (negitive roll), Turns Counter Clockwise (positive yaw)

		setMotorState(m1, m2, m3, m4);

		chThdSleepMilliseconds(QC_PID_MS);
	}
}

static void initPid(qc_pid_t* pid, uint16_t p, uint16_t i, uint16_t d) {
	pid->p = p;
	pid->i = i;
	pid->d = d;
	pid->errorSum = 0;
	pid->lastUpdate = 0;
	pid->previousError = 0;
}

static float updatePid(qc_pid_t* pid, float target, float actual) {
	float now = getNow();
	float dt = (float)getDeltaTime(now, pid->lastUpdate) * 0.001f;
	pid->lastUpdate = now;

	float error = target - actual;

	float p = error;
	
	pid->errorSum += (error * dt);
	float i = pid->errorSum;

	float deltaError = pid->previousError - error;
	float d = deltaError / dt;


	pid->previousError = error;

	float output = (p * pid->p) + (i * pid->i) + (d * pid->d);
	return output;
}