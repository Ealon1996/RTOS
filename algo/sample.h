#include "arrdef.h"
#include "pconfig.h"

#include "stdio.h"


//this is sensor module for mcu platform


#if USE_OS==1
#include "FreeRTOS.h"
#include "semphr.h"

#elif USE_OS==2

#endif


#ifndef __SENSOR_H
#define __SENSOR_H

#if PLATFORM == 0
#include "..//sensor/LT2445.h"
#include "..//sensor/ASM330LHH.h"
#elif PLATFORM == 1
typedef union
{
	uint8_t  AD_Data32[4];
	int32_t data;
}DATA_32BIT;
#endif

typedef struct sensor_data{
	
	int32_t pressure;
	int32_t accx;
	int32_t accy;
	int32_t accz;
	
}sensor_data_t;

typedef struct sensor_buffer_control{
	uint32_t* start_ptr;
	uint32_t* end_ptr;
	uint32_t* read_ptr;
	uint32_t* write_ptr;
	uint32_t  maxlen;
	uint8_t  word_size;
}sbc_t;

void Sensor_Task(void *p);
BaseType_t Sensor_Task_GiveSemaphore_FromISR();
SemaphoreHandle_t get_sensor_Semaphore();
void Sensor_Task_Init();


#endif
