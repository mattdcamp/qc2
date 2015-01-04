#include <Arduino.h>
#include <math.h>

#include "qc_longTimer.h"
#include "qc_logger.h"

uint32_t seconds;
static uint32_t lastUpdate;

void longTimer_setup() {
	seconds = 0;
	lastUpdate = 0;
}

msg_t longTimer_thread_method(void* arg) {
	chRegSetThreadName("LongTimer");
	while (!chThdShouldTerminate()) {
		logger_println("LongTime: Start", QC_LOG_VERBOSE);
		uint32_t ticks = chTimeNow();
		uint32_t now = MS2ST(ticks);
		uint32_t next = MS2ST(now + TICK_LENGTH);

		uint32_t millis = now - lastUpdate;
		seconds += roundf((float)millis / (float)TICK_LENGTH);

		lastUpdate = now;
		logger_println("LongTimer: End", QC_LOG_VERBOSE);
		chThdSleepUntil(next);
	}
}

uint32_t longTimer_getTime() {
	return seconds;
}