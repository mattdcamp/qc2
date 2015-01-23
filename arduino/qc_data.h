#include <Arduino.h>

#include "qc_dataTypes.h"

#define QC_MOTOR_MAX	2000
#define QC_MOTOR_MIN	1000

void setupData();

qc_attitude_t getAttitude();
void setAttitude(float pitch, float roll, float heading);
void setAttitude(float height);
String attitude2Csv();

qc_position_t getPosition();
void setPosition(double lat, double lng, double course, double altitiude);
String postion2Csv();

qc_gpsHealth_t getHealth();
void setHealth(uint32_t fix, uint32_t failure, uint32_t success, uint32_t satellites);
String health2Csv();

qc_flightCommand_t getCommand();
void setCommand(float pitch, float roll, float heading, float height, float thrust);
String command2Csv();


qc_motorState_t getMotorState();
void setMotorState(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
String motorState2Csv();

uint32_t getNow();
uint32_t getDeltaTime(uint32_t now, uint32_t then);