#include <TinyGPS++.h>
#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Wire\Wire.h>
#include <SPI\SPI.h>
#include <SFE_LSM9DS0.h>
#include <Adafruit_CC3000_Library\Adafruit_CC3000.h>
#include <math.h>
#include <Servo.h>

#include "qc_data.h"
#include "qc_imu.h"
#include "qc_sonar.h"
//#include "qc_gps.h"
#include "qc_wifi.h"
#include "qc_logger.h"
#include "qc_pid.h"
#include "qc_motor.h"

static WORKING_AREA(debug_thread, 64);
static WORKING_AREA(ctrl_thread, 1024);
static WORKING_AREA(imu_thread, 1024);
static WORKING_AREA(sonar_thread, 1024);
//static WORKING_AREA(gps_thread, 1024);
static WORKING_AREA(wifiWrite_thread, 1024);
static WORKING_AREA(wifiRead_thread, 1024);
static WORKING_AREA(pid_thread, 1024);
static WORKING_AREA(motor_thread, 1024);

static char* states[] = { THD_STATE_NAMES };

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
	logger_print(" PID:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedStack(pid_thread, sizeof(pid_thread))), QC_THREAD_LOG_LEVEL);
	logger_print(" MOTOR:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedStack(motor_thread, sizeof(motor_thread))), QC_THREAD_LOG_LEVEL);
	logger_print(" main:", QC_THREAD_LOG_LEVEL);
	logger_strPrint(String(chUnusedHeapMain()), QC_THREAD_LOG_LEVEL);
	
	logger_print(" -- ", QC_THREAD_LOG_LEVEL);

	Thread* tp = chRegFirstThread();
	while (tp) {
		String name = String(tp->p_name);
		String state = states[tp->p_state];
		tp = chRegNextThread(tp);

		logger_strPrint(name, QC_THREAD_LOG_LEVEL);
		logger_print(":", QC_THREAD_LOG_LEVEL);
		logger_strPrint(state, QC_THREAD_LOG_LEVEL);
		logger_print(" ", QC_THREAD_LOG_LEVEL);
	}
	logger_println("", QC_THREAD_LOG_LEVEL);
}

msg_t debug_thread_method(void *arg) {
	chRegSetThreadName("Debug");
	while (!chThdShouldTerminate()) {
		printThreadInfo();
		chThdSleepSeconds(2);
	}
}

void loop() {}

void mainThread() {
	logger_setup();
	setupData();
	pid_setup();
	motor_setup();
	imu_setup();
	sonar_setup();
//	gps_setup();
	wifi_setup();

	uint8_t sensorPrio = NORMALPRIO + 10;
	uint8_t communicationPrio = sensorPrio - 5;
	uint8_t cmdPrio = sensorPrio + 5;

	chThdCreateStatic(debug_thread, sizeof(debug_thread), cmdPrio + 1, debug_thread_method, NULL);
	chThdCreateStatic(pid_thread, sizeof(pid_thread), cmdPrio, pid_thread_method, NULL);
	chThdCreateStatic(motor_thread, sizeof(motor_thread), cmdPrio, motor_thread_method, NULL);
	chThdCreateStatic(ctrl_thread, sizeof(ctrl_thread), cmdPrio, controlThread_method, NULL);
	chThdCreateStatic(imu_thread, sizeof(imu_thread), sensorPrio, imu_thread_method, NULL);
	chThdCreateStatic(sonar_thread, sizeof(sonar_thread), sensorPrio, sonar_thread_method, NULL);
//	chThdCreateStatic(gps_thread, sizeof(gps_thread), sensorPrio, gps_thread_method, NULL);	 
	chThdCreateStatic(wifiWrite_thread, sizeof(wifiWrite_thread), communicationPrio, wifiWrite_thread_method, NULL);
	chThdCreateStatic(wifiRead_thread, sizeof(wifiRead_thread), communicationPrio, wifiRead_thread_method, NULL);

	while (true) {}
}

void setup()
{
	chBegin(mainThread);
}

msg_t controlThread_method(void *arg) {
	chRegSetThreadName("Control");
	while (!chThdShouldTerminate()) {
		String csv = attitude2Csv();
		wifi_sendMsg(csv);
//		wifi_sendMsg(postion2Csv());
//		wifi_sendMsg(gpsHeath2Csv());
		wifi_sendMsg(command2Csv());
		wifi_sendMsg(motorState2Csv());

		chThdSleepMilliseconds(500);
	}
}