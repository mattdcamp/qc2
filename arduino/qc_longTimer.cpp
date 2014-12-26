#include <Arduino.h>
#include <math.h>

#include "qc_longTimer.h"

uint32_t seconds;
static uint32_t lastUpdate;

void longTimer_setup() {
	seconds = 0;
	lastUpdate = 0;
}

msg_t longTimer_thread_method(void* arg) {
	while (!chThdShouldTerminate()) {
		uint32_t ticks = chTimeNow();
		uint32_t now = MS2ST(ticks);
		uint32_t next = MS2ST(now + 1000);

		uint32_t millis = now - lastUpdate;
		seconds += roundf((float)millis / 1000.0);

		lastUpdate = now;
		chThdSleepUntil(next);
	}
}

uint32_t longTimer_getTime() {
	return seconds;
}