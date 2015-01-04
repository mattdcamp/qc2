#include <ChibiOS_ARM.h>

#include "qc_logger.h";

static Semaphore serial_sem;

void logger_setup() {
	chSemInit(&serial_sem, 1);
	
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
		chSemWait(&serial_sem);
		if (newLine) {
			Serial.println(msg);
		} else {
			Serial.print(msg);
		}
		chSemSignal(&serial_sem);
	}
}

void logger_strPrint(String msg, qc_logLevel_t level) {
	char* msgA = (char*)calloc(msg.length() + 1, sizeof(char));
	msg.toCharArray(msgA, msg.length());
	logger_print(msgA, level);
	free(msgA);
}

void logger_strPrintln(String msg, qc_logLevel_t level) {
	char* msgA = (char*)calloc(msg.length() + 1, sizeof(char));
	msg.toCharArray(msgA, msg.length());
	logger_println(msgA, level);
	free(msgA);
}
