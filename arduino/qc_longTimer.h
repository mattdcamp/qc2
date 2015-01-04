#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <Arduino.h>

#define TICK_LENGTH	500

void longTimer_setup();
msg_t longTimer_thread_method(void* arg);
uint32_t longTimer_getTime();
String longTimer_getTimeStr();