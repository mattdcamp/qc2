
// Network setup
#define WLAN_SSID       "MC"
#define WLAN_PASS       "1BM1SW1N"
#define WLAN_SECURITY   WLAN_SEC_WPA2

// Pin setup for the wifi
#define WIFI_IRQ   24
#define WIFI_VBAT  26
#define WIFI_CS    22

#define IDLE_TIMEOUT_MS  3000

#define WIFI_MSG_LENGTH 256
#define WIFI_BUFFER_SIZE 256

#define WIFI_DEBUG false

#define QC_WIFI_MSG_CYCLE	100

uint16_t wifi_setup();
msg_t wifiWrite_thread_method(void *arg);
msg_t wifiRead_thread_method(void *arg);
void wifi_sendMsg(String msg);