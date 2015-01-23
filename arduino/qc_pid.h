#define QC_PID_MS		50

#define QC_MOTOR_MAX	2000
#define QC_MOTOR_MIN	1000

uint16_t pid_setup();
msg_t pid_thread_method(void *arg);