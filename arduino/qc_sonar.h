#define QC_SONAR_MS				10
#define QC_SONAR_SAMPLES		10

#define QC_HEIGHT_PIN			12

void sonar_setup();
msg_t sonar_thread_method(void *arg);