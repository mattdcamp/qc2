#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <stdint.h>

#include "LSM9DS0_AHRS.h"
#include "qc_data.h"
#include "qc_logger.h"
#include "Average.h"

#include "qc_imu.h"

static Average<float> pitchAvg(QC_IMU_SAMPLES);
static Average<float> rollAvg(QC_IMU_SAMPLES);
static Average<float> headingAvg(QC_IMU_SAMPLES);

static uint32_t lastUpdate;
static LSM9DS0_AHRS* ahrs;

static uint32_t getDeltaTime();
static void imuUpdate();
static void imuUpdateTime(float dt);
static void pushSampleToBuffer(qc_attitude_t newSample);

void imu_setup() {
	logger_println("IMU: Initializing...", QC_LOG_INFO);

	lastUpdate = 0.0f;

	ahrs = new LSM9DS0_AHRS();
	
	logger_println("IMU: Ready!", QC_LOG_INFO);
}

msg_t imu_thread_method(void *arg) {
	chRegSetThreadName("IMU");
	while (!chThdShouldTerminate()) {
		uint32_t start = MS2ST(chTimeNow());
		logger_println("IMU: Start", QC_LOG_VERBOSE);
		imuUpdate();
		logger_println("IMU: End", QC_LOG_VERBOSE);
		uint32_t end = MS2ST(chTimeNow());
		uint32_t t = end - start;
		logger_print("IMU: Update Time: ", QC_THREAD_TIME_LOG_LEVEL);
		logger_strPrintln(String(t), QC_THREAD_TIME_LOG_LEVEL);
		chThdSleepMilliseconds(QC_IMU_MS);
	}
	logger_println("IMU: Exiting THREAD", QC_LOG_ERROR);
}

static void imuUpdate() {
	float dt = (float)getDeltaTime() * 0.001f;
	imuUpdateTime(dt);
}

static void imuUpdateTime(float dt) {
	ahrs->update(dt);

	logger_print("IMU: Update Sample. Delta Time: ", QC_IMU_LOG_LEVEL);
	logger_strPrintln(String(dt, 5), QC_IMU_LOG_LEVEL);

	qc_attitude_t attitude;
	attitude.pitch = -ahrs->pitch;
	attitude.roll = -ahrs->roll;
	attitude.heading = -ahrs->yaw;

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