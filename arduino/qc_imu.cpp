#include <ChibiOS_ARM\ChibiOS_ARM.h>
#include <stdint.h>

#include "LSM9DS0_AHRS.h"
#include "qc_dataTypes.h" 
#include "qc_longTimer.h"
#include "qc_imu.h"

static Semaphore imu_sem;

static qc_attitude_t* attitudeBuffer;
static uint8_t attitudeBufferIndex;

static uint32_t lastUpdate;

static LSM9DS0_AHRS* ahrs;

static uint32_t getDeltaTime();

static void imuUpdate();
static void pushSampleToBuffer(qc_attitude_t newSample);

void imu_setup() {
	chSemInit(&imu_sem, 1);

	attitudeBuffer = (qc_attitude_t*)calloc(QC_IMU_SAMPLES, sizeof(qc_attitude_t));
	attitudeBufferIndex = 0;

	lastUpdate = 0.0f;

	ahrs = new LSM9DS0_AHRS();

	for (uint8_t i = 0; i < QC_IMU_SAMPLES; i++) {
		imuUpdate();
	}
}

msg_t imu_thread_method(void *arg) {
	while (!chThdShouldTerminate()) {
		imuUpdate();
		chThdSleepMilliseconds(QC_IMU_MS);
	}
}

qc_attitude_t imu_getAttitude() {
	qc_attitude_t sum;
	sum.pitch = 0;
	sum.roll = 0;
	sum.heading = 0;

	chSemWait(&imu_sem);
	for (uint8_t i = 0; i < QC_IMU_SAMPLES; i++) {
		qc_attitude_t* cur = attitudeBuffer + i;
		sum.pitch += cur->pitch;
		sum.roll += cur->roll;
		sum.heading += cur->heading;
	}
	chSemSignal(&imu_sem);

	sum.pitch /= QC_IMU_SAMPLES;
	sum.roll /= QC_IMU_SAMPLES;
	sum.heading /= QC_IMU_SAMPLES;

	return sum;
}

String imu_attitude2Csv() {
	qc_attitude_t a = imu_getAttitude();

	String output = "";
	output += "attitude,";

	output += String(a.pitch, 10);
	output += ",";

	output += String(a.roll, 10);
	output += ",";

	output += String(a.heading, 10);
	output += ",";

	output += String(longTimer_getTime());

	return output;
}

static void imuUpdate() {
	float dt = (float)getDeltaTime() * 0.001f;

	ahrs->update(dt);

	qc_attitude_t attitude;
	attitude.pitch = -ahrs->pitch;
	attitude.roll = ahrs->roll;
	attitude.heading = ahrs->yaw;

	chSemWait(&imu_sem);
	pushSampleToBuffer(attitude);
	chSemSignal(&imu_sem);
}

static void pushSampleToBuffer(qc_attitude_t newSample) {
	qc_attitude_t* sample = attitudeBuffer + attitudeBufferIndex;
	sample->pitch = newSample.pitch;
	sample->roll = newSample.roll;
	sample->heading = newSample.heading;

	attitudeBufferIndex++;
	if (attitudeBufferIndex >= QC_IMU_SAMPLES) {
		attitudeBufferIndex = 0;
	}
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