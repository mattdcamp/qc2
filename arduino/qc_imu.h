
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW

#define QC_IMU_MS				100
#define QC_IMU_SAMPLES			5

void imu_setup();
msg_t imu_thread_method(void *arg);