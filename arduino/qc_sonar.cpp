#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <stdint.h>

#include "qc_data.h"
#include "qc_logger.h"
#include "Average.h"

#include "qc_sonar.h"

static Average<uint32_t> heightAvg(QC_SONAR_SAMPLES);

static void readAll();
static void readHeight();
static uint32_t readSonar(uint8_t pin);

void sonar_setup() {
	logger_println("SONAR: Initializing...", QC_LOG_INFO);
	pinMode(QC_HEIGHT_PIN, INPUT);
	
	logger_println("SONAR: Init Samples", QC_LOG_INFO);
	for (int i = 0; i < QC_SONAR_SAMPLES; i++) {
		readAll();
		delay(QC_SONAR_MS);
	}

	logger_println("SONAR: Ready!", QC_LOG_INFO);
}

msg_t sonar_thread_method(void *arg) {
	chRegSetThreadName("Sonar");
	while (!chThdShouldTerminate()) {
		logger_println("Sonar: Start", QC_LOG_VERBOSE);
		readAll();
		logger_println("Sonar: End", QC_LOG_VERBOSE);
		chThdSleepMilliseconds(QC_SONAR_MS);
	}
	logger_println("IMU: Exiting THREAD", QC_LOG_ERROR);
}

static void readAll() {
	readHeight(); // One day we will have more;
}

static void readHeight() {
	uint32_t height = readSonar(QC_HEIGHT_PIN);
	heightAvg.push(height);
	setAttitude(heightAvg.mean());
}

static uint32_t readSonar(uint8_t pin) {
	uint32_t pulse = pulseIn(pin, HIGH);
	uint32_t distance = (pulse / 58);

	logger_print("SONAR: Pulse: ", QC_SONAR_LOG_LEVEL);
	logger_strPrint(String(pulse), QC_SONAR_LOG_LEVEL);
	logger_print(" Distance: ", QC_SONAR_LOG_LEVEL);
	logger_strPrintln(String(distance), QC_SONAR_LOG_LEVEL);

	return distance;
}