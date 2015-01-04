#include <Arduino.h>

#include "qc_dataTypes.h"

void setupData();

qc_attitude_t getAttitude();
void setAttitude(float pitch, float roll, float heading);
void setAttitude(float height);
String attitude2Csv();

qc_position_t getPosition();
void setPosition(double lat, double lng, double course, double altitiude);
String postion2Csv();

qc_gpsHealth getHealth();
void setHealth(uint32_t fix, uint32_t failure, uint32_t success, uint32_t satellites);
String health2Csv();

qc_flightCommand getCommand();
void setCommand(float pitch, float roll, float heading, float height, float thrust);
String command2Csv();


qc_motorState getMotorState();
void setMotorState(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4);
String motorState2Csv();