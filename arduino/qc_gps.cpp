#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Arduino.h>
#include <TinyGPS++.h>

#include "qc_gps.h"
#include "qc_data.h"
#include "qc_logger.h"

static TinyGPSPlus gps;

static void displayGPS();
static void getField(char* buffer, int index);

void gps_setup() {
	logger_println("GPS: Initializing...", QC_LOG_INFO);
	Serial1.begin(9600);
	while (!Serial1) {}
	
	delay(500);
	logger_println("GPS: Ready!", QC_LOG_INFO);
}
msg_t gps_thread_method(void *arg) {
	chRegSetThreadName("GPS");
	while (!chThdShouldTerminate()) {
		logger_println("GPS: Start", QC_LOG_VERBOSE);
		while(Serial1.available()) {
			gps.encode(Serial1.read());
		}
		setPosition(gps.location.lat(), gps.location.lng(), gps.course.deg(), gps.altitude.meters());
		setHealth(gps.sentencesWithFix(), gps.failedChecksum(), gps.passedChecksum(), gps.satellites.value());
		logger_println("GPS: End", QC_LOG_VERBOSE);
		chThdSleepMilliseconds(QC_GPS_MS);
	}
}