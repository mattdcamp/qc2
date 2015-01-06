#include <ChibiOS_ARM.h>

#include "qc_logger.h";

static Mutex serial_mutex;

void logger_setup() {
	chMtxInit(&serial_mutex);

	Serial.begin(115200);
	while (!Serial) {}
	delay(500);

	Serial.println("LOGGER: Ready!");
}

void logger_print(char* msg, qc_logLevel_t level) {
	logger_write(false, msg, level);
}

void logger_println(char* msg, qc_logLevel_t level) {
	logger_write(true, msg, level);
}

void logger_write(bool newLine, char* msg, qc_logLevel_t level) {
	if (level >= QC_LOG_LEVEL) {
		chMtxLock(&serial_mutex);
		if (newLine) {
			Serial.println(msg);
		} else {
			Serial.print(msg);
		}
		chMtxUnlock();
	}
}

void logger_strPrint(String msg, qc_logLevel_t level) {
	char* msgA = (char*)calloc(msg.length() + 1, sizeof(char));
	msg.toCharArray(msgA, msg.length() + 1);
	logger_print(msgA, level);
	free(msgA);
}

void logger_strPrintln(String msg, qc_logLevel_t level) {
	char* msgA = (char*)calloc(msg.length() + 1, sizeof(char));
	msg.toCharArray(msgA, msg.length() + 1);
	logger_println(msgA, level);
	free(msgA);
}
