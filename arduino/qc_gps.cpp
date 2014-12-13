#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Arduino.h>
#include <TinyGPS++.h>

#include "qc_gps.h"
#include "qc_dataTypes.h"

static Semaphore gps_sem;

static TinyGPSPlus gps;

static void displayGPS();
static void getField(char* buffer, int index);

void gps_setup() {
	chSemInit(&gps_sem, 1);

	Serial.println("GPS: Begin");	
	Serial1.begin(9600);
	while (!Serial1) {}
	
	delay(500);

}
msg_t gps_thread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		chSemWait(&gps_sem);

		while(Serial1.available()) {
			gps.encode(Serial1.read());
		}

		chSemSignal(&gps_sem);

		chThdSleepMilliseconds(QC_GPS_MS);
	}
}

qc_position_t gps_getPosition() {
	qc_position_t position;
	chSemWait(&gps_sem);
	
	position.lattitude = gps.location.lat();
	position.longitude = gps.location.lng();
	position.course = gps.course.deg();
	position.altitude = gps.altitude.meters();
	
	chSemSignal(&gps_sem);
	
	return position;
}

String gps_postion2Csv() {
	qc_position_t position = gps_getPosition();
	String output;
	output += "gps,";
	
	output += String(position.lattitude, 6);
	output += ',';

	output += String(position.longitude, 6);
	output += ',';

	output += String(position.altitude, 6);
	output += ',';

	output += String(position.course, 6);

	return output;
}

qc_gpsHealth gps_getHeath() {
	qc_gpsHealth health;
	chSemWait(&gps_sem);

	health.fix = gps.sentencesWithFix();
	health.failure = gps.failedChecksum();
	health.success = gps.passedChecksum();
	health.satellites = gps.satellites.value();
	
	chSemSignal(&gps_sem);

	return health;
}

String gps_heath2Csv() {
	qc_gpsHealth health = gps_getHeath();
	String output;
	output += "gps,";

	output += String(health.fix);
	output += ',';

	output += String(health.failure);
	output += ',';

	output += String(health.success);
	output += ',';

	output += String(health.satellites);

	return output;
}