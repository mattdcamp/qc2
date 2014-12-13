#include <TinyGPS++.h>
#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Wire\Wire.h>
#include <SPI\SPI.h>
#include <SFE_LSM9DS0.h>
#include <Adafruit_CC3000_Library\Adafruit_CC3000.h>
#include <math.h>

#include "qc_imu.h"
#include "qc_gps.h"
#include "qc_wifi.h"
#include "qc_longTimer.h"

#include "qc_dataTypes.h"

#include <Servo\src\Servo.h>

#define THREAD_DEBUG false

static WORKING_AREA(debug_thread, 64);
static WORKING_AREA(ctrl_thread, 1024);
static WORKING_AREA(imu_thread, 1024);
static WORKING_AREA(gps_thread, 1024);
static WORKING_AREA(wifi_thread, 1024);
static WORKING_AREA(longTimer_thread, 64);

void printThreadInfo() {
	if (THREAD_DEBUG) {
		Serial.print("Unused Stack debug:");
		Serial.print(chUnusedStack(debug_thread, sizeof(debug_thread)));
		Serial.print(" imu:");
		Serial.print(chUnusedStack(imu_thread, sizeof(imu_thread)));
		Serial.print(" gps:");
		Serial.print(chUnusedStack(gps_thread, sizeof(gps_thread)));
		Serial.print(" wifi:");
		Serial.print(chUnusedStack(wifi_thread, sizeof(wifi_thread)));
		Serial.print(" longTimer:");
		Serial.print(chUnusedStack(longTimer_thread, sizeof(longTimer_thread)));
		Serial.print(" main:");
		Serial.println(chUnusedHeapMain());
	}
}

msg_t debug_thread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		printThreadInfo();
		chThdSleepSeconds(1);
	}
}

void loop() {}

void mainThread() {
	const tprio_t ctrlPrio = NORMALPRIO + 10;
	const tprio_t sensorPrio = ctrlPrio - 1;
	const tprio_t communicationPrio = sensorPrio - 1;

	chThdCreateStatic(longTimer_thread, sizeof(longTimer_thread), NORMALPRIO + 100, longTimer_thread_method, NULL);
	chThdCreateStatic(debug_thread, sizeof(debug_thread), NORMALPRIO - 100, debug_thread_method, NULL);
	chThdCreateStatic(ctrl_thread, sizeof(ctrl_thread), ctrlPrio, controlThread_method, NULL);
	chThdCreateStatic(imu_thread, sizeof(imu_thread), sensorPrio, imu_thread_method, NULL);
	chThdCreateStatic(gps_thread, sizeof(gps_thread), sensorPrio, gps_thread_method, NULL);	 
	chThdCreateStatic(wifi_thread, sizeof(wifi_thread), communicationPrio, wifi_thread_method, NULL);
}

void setup()
{
	Serial.begin(115200);
	while (!Serial) {}
	delay(500);

	imu_setup();
	gps_setup();
	wifi_setup();
	longTimer_setup();

	chBegin(mainThread);
}

msg_t controlThread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		
		wifi_sendMsg(imu_attitude2Csv());
		wifi_sendMsg(gps_postion2Csv());
		wifi_sendMsg(gps_heath2Csv());

		chThdSleepMilliseconds(100);
	}
}