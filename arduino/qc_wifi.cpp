#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Adafruit_CC3000_Library\Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>

#include "qc_wifi.h"

Adafruit_CC3000 wifi = Adafruit_CC3000(WIFI_CS, WIFI_IRQ, WIFI_VBAT, SPI_CLOCK_DIVIDER);
Adafruit_CC3000_Client connection;

char** msgBuffer;
uint8_t writeMsgIndex;
uint8_t readMsgIndex;

static void writeMessage(char* msg);
static boolean hasMsgToWrite();
static uint8_t updateIndex(uint8_t i);

uint16_t wifi_setup() {
	msgBuffer = (char**)calloc(WIFI_BUFFER_SIZE, sizeof(char*));
	writeMsgIndex = 0;
	readMsgIndex = 0;

	Serial.println("WIFI: Initializing...");
	if (!wifi.begin()) {
		Serial.println("WIFI: Failure to start");
		return -1;
	}

	Serial.println("WIFI: Connect to ap");
	if (!wifi.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
		Serial.println("WIFI: Failure to connect to ap");
		return -2;
	}

	Serial.println("WIFI: check DHCP");
	while (!wifi.checkDHCP()) {
		delay(100);
	}

	uint32_t ip = wifi.IP2U32(192, 168, 1, 108);

	Serial.println("WIFI: connect");
	connection = wifi.connectTCP(ip, 4242);

	Serial.println("WIFI: Ready!");
	return 1;
}

msg_t wifi_thread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		while (!hasMsgToWrite()) {
			chThdSleepSeconds(1);
		}

		char* msg = msgBuffer[readMsgIndex];
		readMsgIndex = updateIndex(readMsgIndex);

		writeMessage(msg);
		free(msg);

		chThdSleepMilliseconds(10);
	}
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
	if (connection.connected()) {
		connection.write(msg, strlen(msg));
		if (WIFI_DEBUG) { Serial.println(msg); }
	}
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

