#define QC_PID_MS		100

uint16_t pid_setup();
uint16_t motor_setup();
void resetPid();
msg_t pid_thread_method(void *arg);