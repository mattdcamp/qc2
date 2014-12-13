
// Network setup
#define WLAN_SSID       "MC"
#define WLAN_PASS       "1BM1SW1N"
#define WLAN_SECURITY   WLAN_SEC_WPA2

// Pin setup for the wifi
#define WIFI_IRQ   23
#define WIFI_VBAT  24
#define WIFI_CS    22

#define IDLE_TIMEOUT_MS  3000

#define WIFI_MSG_LENGTH 256
#define WIFI_BUFFER_SIZE 256

#define WIFI_DEBUG true

uint16_t wifi_setup();
msg_t wifi_thread_method(void *arg);
void wifi_sendMsg(String msg);