#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <stdint.h>

#include "qc_dataTypes.h" 
#include "qc_data.h"
#include "Average.h"

#include "qc_sonar.h"

static Average<uint32_t> heightAvg(QC_SONAR_SAMPLES);

static void readAll();
static void readHeight();
static uint32_t readSonar(uint8_t pin);

void sonar_setup() {
	Serial.println("SONAR: Initializing");
	pinMode(QC_HEIGHT_PIN, INPUT);

	Serial.println("SONAR: Init Samples");
	for (int i = 0; i < QC_SONAR_SAMPLES; i++) {
		readAll();
		delay(QC_SONAR_MS);
	}

	Serial.println("SONAR: Ready!");
}

msg_t sonar_thread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		readAll();
		chThdSleepMilliseconds(QC_SONAR_MS);
	}
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
	return distance;
}