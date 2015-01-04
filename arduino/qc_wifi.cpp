#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Adafruit_CC3000_Library\Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>

#include "qc_data.h"
#include "qc_wifi.h"
#include "qc_logger.h"

static Semaphore wifi_sem;
Adafruit_CC3000 wifi = Adafruit_CC3000(WIFI_CS, WIFI_IRQ, WIFI_VBAT, SPI_CLOCK_DIVIDER);
Adafruit_CC3000_Client connection;

char** msgBuffer;
uint8_t writeMsgIndex;
uint8_t readMsgIndex;

static void writeMessage(char* msg);
static boolean hasMsgToWrite();
static uint8_t updateIndex(uint8_t i);

uint16_t wifi_setup() {
	chSemInit(&wifi_sem, 1);

	msgBuffer = (char**)calloc(WIFI_BUFFER_SIZE, sizeof(char*));
	writeMsgIndex = 0;
	readMsgIndex = 0;

	logger_println("WIFI: Initializing...", QC_LOG_INFO);
	if (!wifi.begin()) {
		logger_println("WIFI: Failure to start", QC_LOG_INFO);
		return -1;
	}

	logger_println("WIFI: Connect to ap", QC_LOG_INFO);
	if (!wifi.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
		logger_println("WIFI: Failure to connect to ap", QC_LOG_INFO);
		return -2;
	}

	logger_println("WIFI: check DHCP", QC_LOG_INFO);
	while (!wifi.checkDHCP()) {
		delay(100);
	}

	uint32_t ip = wifi.IP2U32(192, 168, 1, 108);

	logger_println("WIFI: connect", QC_LOG_INFO);
	connection = wifi.connectTCP(ip, 4242);

	logger_println("WIFI: Ready!", QC_LOG_INFO);
	return 1;
}

msg_t wifiWrite_thread_method(void *arg) {
	chRegSetThreadName("WifiWrite");
	while (!chThdShouldTerminate()) {
		logger_println("WifiWrite: Start", QC_LOG_VERBOSE);
		while (hasMsgToWrite()) {
			logger_println("WifiWrite: Has Message", QC_WIFI_WRITE_LOG_LEVEL);
			char* msg = msgBuffer[readMsgIndex];
			readMsgIndex = updateIndex(readMsgIndex);
			writeMessage(msg);
			free(msg);
			chThdSleepMilliseconds(QC_WIFI_MSG_CYCLE);
		}
		logger_println("WifiWritee: End", QC_LOG_VERBOSE);
		chThdSleepMilliseconds(QC_WIFI_WRITE_CYCLE);
	}
	logger_println("WifiWrite: Exiting THREAD", QC_LOG_ERROR);
}

msg_t wifiRead_thread_method(void *arg) {
	chRegSetThreadName("WifiRead");
	while (!chThdShouldTerminate()) {
		logger_println("WifiRead: Start", QC_LOG_VERBOSE);
		chSemWait(&wifi_sem);
		if (connection.connected() && connection.available()) {
			char* msg = (char*)calloc(256, sizeof(char));
			for (uint8_t i = 0; connection.available(); i++) {
				char c = connection.read();
				msg[i] = c;
				if (i == 255) {
					break;
				}
			}

			logger_print("WifiRead: Received: ", QC_WIFI_READ_LOG_LEVEL);
			logger_println(msg, QC_WIFI_READ_LOG_LEVEL);

			char* values[6];
			uint8_t v = 0;
			for (char* m = strtok(msg, ","); m != 0; m = strtok(0, ",")) {
				values[v++] = m;
				if (v == 6) {
					break;
				}
			}
			
			if (v == 6 && strcmp(values[0], "cmd") == 0) {	// only process messages that are cmds and have all the values present
				float pitch = atof(values[1]);
				float roll = atof(values[2]);
				float heading = atof(values[3]);
				float height = atof(values[4]);
				float thrust = atof(values[5]);
				setCommand(pitch, roll, heading, height, thrust);
			}
		}
		chSemSignal(&wifi_sem);
		logger_println("WifiRead: End", QC_LOG_VERBOSE);
		chThdSleepMilliseconds(QC_WIFI_READ_CYCLE);
	}
	logger_println("WIFI READ: Exiting THREAD", QC_LOG_ERROR);
}

void wifi_sendMsg(String msgStr) {
	uint8_t length = msgStr.length() + 1;
	char* msg = (char*)calloc(length, sizeof(char));
	msgStr.toCharArray(msg, length);

	msgBuffer[writeMsgIndex] = msg;

	writeMsgIndex = updateIndex(writeMsgIndex);
	if (writeMsgIndex == readMsgIndex) {
		free(msgBuffer[readMsgIndex]);
		readMsgIndex = updateIndex(writeMsgIndex);
	}
}

static void writeMessage(char* msg) {
	chSemWait(&wifi_sem);
	if (connection.connected()) {
		connection.write(msg, strlen(msg));

		logger_print("WIFI Send: ", QC_WIFI_WRITE_LOG_LEVEL);
		logger_println(msg, QC_WIFI_WRITE_LOG_LEVEL);
	}
	chSemSignal(&wifi_sem);
}

static boolean hasMsgToWrite() {
	return (readMsgIndex != writeMsgIndex);
}

static uint8_t updateIndex(uint8_t i) {
	if (++i >= WIFI_BUFFER_SIZE) {
		i = 0;
	}
	return i;
}

