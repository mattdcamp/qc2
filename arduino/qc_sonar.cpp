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

	logger_println("SONAR: Ready!", QC_LOG_INFO);
}

msg_t sonar_thread_method(void *arg) {
	chRegSetThreadName("Sonar");
	while (!chThdShouldTerminate()) {
		uint32_t start = MS2ST(chTimeNow());

		logger_println("Sonar: Start", QC_LOG_VERBOSE);
		readAll();
		logger_println("Sonar: End", QC_LOG_VERBOSE);

		uint32_t end = MS2ST(chTimeNow());
		uint32_t t = end - start;
		logger_print("Sonar: Update Time: ", QC_THREAD_TIME_LOG_LEVEL);
		logger_strPrintln(String(t), QC_THREAD_TIME_LOG_LEVEL);

		chThdSleepMilliseconds(QC_SONAR_MS);
	}
	logger_println("Sonar: Exiting THREAD", QC_LOG_ERROR);
}

static void readAll() {
	logger_println("Sonar: readAll", QC_SONAR_LOG_LEVEL);
	readHeight(); // One day we will have more;
}

static void readHeight() {
	logger_println("Sonar: readHeight", QC_SONAR_LOG_LEVEL);
	uint32_t height = readSonar(QC_HEIGHT_PIN);
	heightAvg.push(height);
	setAttitude(heightAvg.mean());
}

static uint32_t readSonar(uint8_t pin) {
	logger_print("Sonar: readSonar: ", QC_SONAR_LOG_LEVEL);
	logger_strPrintln(String(pin), QC_SONAR_LOG_LEVEL);
	uint32_t pulse = pulseIn(pin, HIGH);
	uint32_t distance = (pulse / 58);

	logger_print("Sonar: Pulse: ", QC_SONAR_LOG_LEVEL);
	logger_strPrint(String(pulse), QC_SONAR_LOG_LEVEL);
	logger_print(" Distance: ", QC_SONAR_LOG_LEVEL);
	logger_strPrintln(String(distance), QC_SONAR_LOG_LEVEL);

	return distance;
}