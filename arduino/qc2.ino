#include <TinyGPS++.h>
#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Wire\Wire.h>
#include <SPI\SPI.h>
#include <SFE_LSM9DS0.h>
#include <Adafruit_CC3000_Library\Adafruit_CC3000.h>
#include <math.h>
#include <Servo\src\Servo.h>

#include "qc_data.h"
#include "qc_imu.h"
#include "qc_sonar.h"
//#include "qc_gps.h"
#include "qc_wifi.h"
#include "qc_longTimer.h"
#include "qc_logger.h"

#define THREAD_DEBUG true

static WORKING_AREA(debug_thread, 64);
static WORKING_AREA(ctrl_thread, 1024);
static WORKING_AREA(imu_thread, 1024);
static WORKING_AREA(sonar_thread, 1024);
//static WORKING_AREA(gps_thread, 1024);
static WORKING_AREA(wifiWrite_thread, 1024);
static WORKING_AREA(wifiRead_thread, 1024);
static WORKING_AREA(longTimer_thread, 64);

void printThreadInfo() {
	logger_print("debug:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedStack(debug_thread, sizeof(debug_thread))), QC_THREAD_LOG_LEVEL);
	logger_print(" imu:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedStack(imu_thread, sizeof(imu_thread))), QC_THREAD_LOG_LEVEL);
	logger_print(" sonar:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedStack(sonar_thread, sizeof(sonar_thread))), QC_THREAD_LOG_LEVEL);
//	logger_print(" gps:", QC_THREAD_LOG_LEVEL);
//	logger_print(String(chUnusedStack(gps_thread, sizeof(gps_thread))), QC_THREAD_LOG_LEVEL);
	logger_print(" wifiWrite:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedStack(wifiWrite_thread, sizeof(wifiWrite_thread))), QC_THREAD_LOG_LEVEL);
	logger_print(" wifiRead:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedStack(wifiRead_thread, sizeof(wifiRead_thread))), QC_THREAD_LOG_LEVEL);
	logger_print(" longTimer:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedStack(longTimer_thread, sizeof(longTimer_thread))), QC_THREAD_LOG_LEVEL);
	logger_print(" main:", QC_THREAD_LOG_LEVEL);
	logger_strPrintln(String(chUnusedHeapMain()), QC_THREAD_LOG_LEVEL);
	logger_println("-------", QC_THREAD_LOG_LEVEL);
	Thread* t = chRegFirstThread();
	while (t) {
		const char* name = chRegGetThreadName(t);
		uint8_t state = t->p_state;
		logger_strPrint(String(name), QC_THREAD_LOG_LEVEL);
		logger_print(":", QC_THREAD_LOG_LEVEL);
		logger_strPrint(String(state), QC_THREAD_LOG_LEVEL);
		logger_print(" ", QC_THREAD_LOG_LEVEL);
		t = chRegNextThread(t);
	}
	Serial.println();

}

msg_t debug_thread_method(void *arg) {
	chRegSetThreadName("Debug");
	while (!chThdShouldTerminate()) {
		printThreadInfo();
		chThdSleepSeconds(10);
	}
}

void loop() {}

void mainThread() {
	const tprio_t ctrlPrio = NORMALPRIO + 100;
	const tprio_t sensorPrio = ctrlPrio - 10;
	const tprio_t communicationPrio = sensorPrio;

	chThdCreateStatic(longTimer_thread, sizeof(longTimer_thread), ctrlPrio + 10, longTimer_thread_method, NULL);
	chThdCreateStatic(debug_thread, sizeof(debug_thread), NORMALPRIO - 100, debug_thread_method, NULL);
	chThdCreateStatic(ctrl_thread, sizeof(ctrl_thread), ctrlPrio, controlThread_method, NULL);
	chThdCreateStatic(imu_thread, sizeof(imu_thread), sensorPrio, imu_thread_method, NULL);
	chThdCreateStatic(sonar_thread, sizeof(sonar_thread), sensorPrio, sonar_thread_method, NULL);
//	chThdCreateStatic(gps_thread, sizeof(gps_thread), sensorPrio, gps_thread_method, NULL);	 
	chThdCreateStatic(wifiWrite_thread, sizeof(wifiWrite_thread), communicationPrio, wifiWrite_thread_method, NULL);
	chThdCreateStatic(wifiRead_thread, sizeof(wifiRead_thread), communicationPrio, wifiRead_thread_method, NULL);
}

void setup()
{
	logger_setup();

	setupData();

	imu_setup();
	sonar_setup();
//	gps_setup();
	wifi_setup();
	longTimer_setup();

	chBegin(mainThread);
}

msg_t controlThread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		wifi_sendMsg(attitude2Csv());
//		wifi_sendMsg(postion2Csv());
//		wifi_sendMsg(gpsHeath2Csv());
		wifi_sendMsg(command2Csv());
//		wifi_sendMsg(motorState2Csv());

		chThdSleepMilliseconds(500);
	}
}