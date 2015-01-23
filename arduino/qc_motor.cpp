#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <stdint.h>
#include <Servo.h>

#include "qc_data.h"

#include "qc_motor.h"

static void setThrottle(Servo motor, uint16_t t);

static Servo m1;
static Servo m2;
static Servo m3;
static Servo m4;

uint16_t motor_setup() {
	m1.attach(2);
	m2.attach(3);
	m3.attach(4);
	m4.attach(5);

	setThrottle(m1, QC_MOTOR_MIN);
	setThrottle(m2, QC_MOTOR_MIN);
	setThrottle(m3, QC_MOTOR_MIN);
	setThrottle(m4, QC_MOTOR_MIN);
}

msg_t motor_thread_method(void *arg) {
	chRegSetThreadName("Motor");

	while (!chThdShouldTerminate()) {
		qc_motorState_t motorState = getMotorState();

		setThrottle(m1, motorState.m1);
		setThrottle(m2, motorState.m2);
		setThrottle(m3, motorState.m3);
		setThrottle(m4, motorState.m4);

		chThdSleepMilliseconds(QC_MOTOR_MS);
	}
}

static void setThrottle(Servo motor, uint16_t t) {
	Serial.print("Setting Output: ");
	Serial.println(t);

	m1.writeMicroseconds(t);
	m2.writeMicroseconds(t);
	m3.writeMicroseconds(t);
	m4.writeMicroseconds(t);
}