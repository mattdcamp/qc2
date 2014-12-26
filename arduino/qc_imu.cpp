#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <stdint.h>

#include "LSM9DS0_AHRS.h"
#include "qc_dataTypes.h" 
#include "qc_data.h"
#include "Average.h"

#include "qc_imu.h"

static Average<float> pitchAvg(QC_IMU_SAMPLES);
static Average<float> rollAvg(QC_IMU_SAMPLES);
static Average<float> headingAvg(QC_IMU_SAMPLES);

static uint32_t lastUpdate;
static LSM9DS0_AHRS* ahrs;

static uint32_t getDeltaTime();
static void imuUpdate();
static void pushSampleToBuffer(qc_attitude_t newSample);

void imu_setup() {
	Serial.println("IMU: Initializing");

	lastUpdate = 0.0f;

	ahrs = new LSM9DS0_AHRS();

	Serial.println("IMU: Init Samples");
	for (uint8_t i = 0; i < QC_IMU_SAMPLES*5; i++) {
		imuUpdate();
		delay(QC_IMU_MS);
	}
	Serial.println("IMU: Ready!");
}

msg_t imu_thread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		imuUpdate();
		chThdSleepMilliseconds(QC_IMU_MS);
	}
}

static void imuUpdate() {
	float dt = (float)getDeltaTime() * 0.001f;
	
	ahrs->update(dt);

	qc_attitude_t attitude;
	attitude.pitch = -ahrs->pitch;
	attitude.roll = ahrs->roll;
	attitude.heading = ahrs->yaw;
	
	pushSampleToBuffer(attitude);
	setAttitude(pitchAvg.mean(), rollAvg.mean(), headingAvg.mean());
}

static void pushSampleToBuffer(qc_attitude_t newSample) {
	pitchAvg.push(newSample.pitch);
	rollAvg.push(newSample.roll);
	headingAvg.push(newSample.heading);
}

static uint32_t getDeltaTime() {
	uint32_t now = MS2ST(chTimeNow());

	uint32_t dt = 0;
	if (now < lastUpdate) {
		dt = now + (INT32_MAX - lastUpdate);
	} else {
		dt = now - lastUpdate;
	}

	lastUpdate = now;
	return dt;
}