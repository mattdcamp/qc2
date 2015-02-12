#include <Arduino.h>

#define QC_THREAD_LOG_LEVEL			QC_LOG_NONE
#define QC_THREAD_TIME_LOG_LEVEL	QC_LOG_NONE
#define QC_SONAR_LOG_LEVEL			QC_LOG_NONE
#define QC_IMU_LOG_LEVEL			QC_LOG_NONE
#define QC_GPS_LOG_LEVEL			QC_LOG_NONE
#define QC_DATA_LOG_LEVEL			QC_LOG_NONE
#define QC_WIFI_READ_LOG_LEVEL		QC_LOG_NONE
#define QC_WIFI_WRITE_LOG_LEVEL		QC_LOG_NONE


#define QC_LOG_LEVEL			QC_LOG_INFO

typedef enum {
	QC_LOG_NONE = -1,
	QC_LOG_VERBOSE = 0,
	QC_LOG_DEBUG = 1,
	QC_LOG_INFO = 2,
	QC_LOG_WARN = 3,
	QC_LOG_ERROR = 4
} qc_logLevel_t;

void logger_setup();
void logger_print(char* msg, qc_logLevel_t level);
void logger_println(char* msg, qc_logLevel_t level);
void logger_write(bool newLine, char* msg, qc_logLevel_t level);

void logger_strPrint(String msg, qc_logLevel_t level);
void logger_strPrintln(String msg, qc_logLevel_t level);