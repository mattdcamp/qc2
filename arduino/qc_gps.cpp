#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Arduino.h>
#include <TinyGPS++.h>

#include "qc_gps.h"
#include "qc_dataTypes.h"
#include "qc_data.h"

static TinyGPSPlus gps;

static void displayGPS();
static void getField(char* buffer, int index);

void gps_setup() {
	Serial.println("GPS: Initializing...");

	Serial.println("GPS: Begin");	
	Serial1.begin(9600);
	while (!Serial1) {}
	
	delay(500);
	Serial.println("GPS: Ready!");
}
msg_t gps_thread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		while(Serial1.available()) {
			gps.encode(Serial1.read());
		}
		setPosition(gps.location.lat(), gps.location.lng(), gps.course.deg(), gps.altitude.meters());
		setGpsHealth(gps.sentencesWithFix(), gps.failedChecksum(), gps.passedChecksum(), gps.satellites.value());
		chThdSleepMilliseconds(QC_GPS_MS);
	}
}