#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"		/* RTOS firmware */



////////////////////////////////////////////////
#ifdef __linux__
#include "arm_math.h"
#include "stdint.h"
#include "semphr.h"

#define CH3_TASKMANAGEMENT
#ifdef CH3_TASKMANAGEMENT
void vTask1(void*);
void vTask2(void*);
void vTask3(void*);
void vTask4(void*);
#endif(CH3_TASKMANAGEMENT)
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void RTOS_START(void);
SemaphoreHandle_t xSemaphore=NULL;
#endif
////////////////////////////////////////////////


////////////////////////////////////////////////
#ifdef __MINGW32__
#include <conio.h>
#include "task.h"
#define mainREGION_1_SIZE	10801
#define mainREGION_2_SIZE	29905
#define mainREGION_3_SIZE	6007
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY	0
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

extern void main_blinky( void );
extern void main_full( void );
void vFullDemoTickHookFunction( void );
void vFullDemoIdleFunction( void );
static void prvExerciseHeapStats( void );
static void prvSaveTraceFile( void );
StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];
static BaseType_t xTraceRunning = pdTRUE;
BaseType_t task_ret;
void ss_task(void);
void RTOS_START(void);
#endif
////////////////////////////////////////////////


int main( void )
{
    /* This demo uses heap_5.c, so start by defining some heap regions.  heap_5
    is only used for test and example reasons.  Heap_4 is more appropriate.  See
    http://www.freertos.org/a00111.html for an explanation. */
    RTOS_START();
    return 0;
}
/*-----------------------------------------------------------*/
//////////////////////////////////////////////////
#ifdef __MINGW32__
void ss_task(void){
    while(1){
        printf("test\n");
        vTaskDelay(1000);
    }
}
void RTOS_START(void){
    task_ret = xTaskCreate(ss_task, "ss task", 100, NULL, 1, NULL );
    if(task_ret !=pdTRUE){
        printf("sensor task create failed\n");
        while(1);
    }
     vTaskStartScheduler();
}

void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c, heap_2.c or heap_4.c is being used, then the
    size of the	heap available to pvPortMalloc() is defined by
    configTOTAL_HEAP_SIZE in FreeRTOSConfig.h, and the xPortGetFreeHeapSize()
    API function can be used to query the size of free heap space that remains
    (although it does not provide information on how the remaining heap might be
    fragmented).  See http://www.freertos.org/a00111.html for more
    information. */
    vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{

}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.  This function is
    provided as an example only as stack overflow checking does not function
    when running the FreeRTOS Windows port. */
    vAssertCalled( __LINE__, __FILE__ );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    /* This function will be called by each tick interrupt if
    configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
    added here, but the tick hook is called from an interrupt context, so
    code must not attempt to block, and only the interrupt safe FreeRTOS API
    functions can be used (those that end in FromISR()). */

    #if ( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
    {
        //vFullDemoTickHookFunction();
    }
    #endif /* mainCREATE_SIMPLE_BLINKY_DEMO_ONLY */
}
/*-----------------------------------------------------------*/

void vApplicationDaemonTaskStartupHook( void )
{
    /* This function will be called once only, when the daemon task starts to
    execute	(sometimes called the timer task).  This is useful if the
    application includes initialisation code that would benefit from executing
    after the scheduler has been started. */
}
/*-----------------------------------------------------------*/

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
static BaseType_t xPrinted = pdFALSE;
volatile uint32_t ulSetToNonZeroInDebuggerToContinue = 0;

    /* Called if an assertion passed to configASSERT() fails.  See
    http://www.freertos.org/a00110.html#configASSERT for more information. */

    /* Parameters are not used. */
    ( void ) ulLine;
    ( void ) pcFileName;


    taskENTER_CRITICAL();
    {
        /* Stop the trace recording. */
        if( xPrinted == pdFALSE )
        {
            xPrinted = pdTRUE;
            if( xTraceRunning == pdTRUE )
            {
                prvSaveTraceFile();
            }
        }

        /* You can step out of this function to debug the assertion by using
        the debugger to set ulSetToNonZeroInDebuggerToContinue to a non-zero
        value. */
        while( ulSetToNonZeroInDebuggerToContinue == 0 )
        {
            __asm volatile( "NOP" );
            __asm volatile( "NOP" );
        }
    }
    taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static void prvSaveTraceFile( void )
{
    /* Tracing is not used when code coverage analysis is being performed. */
    #if( projCOVERAGE_TEST != 1 )
    {
        FILE* pxOutputFile;

        vTraceStop();

        pxOutputFile = fopen( "Trace.dump", "wb");

        if( pxOutputFile != NULL )
        {
            fwrite( RecorderDataPtr, sizeof( RecorderDataType ), 1, pxOutputFile );
            fclose( pxOutputFile );
            printf( "\r\nTrace output saved to Trace.dump\r\n" );
        }
        else
        {
            printf( "\r\nFailed to create trace dump file\r\n" );
        }
    }
    #endif
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

static void prvExerciseHeapStats( void )
{
HeapStats_t xHeapStats;
size_t xInitialFreeSpace = xPortGetFreeHeapSize(), xMinimumFreeBytes;
size_t xMetaDataOverhead, i;
void *pvAllocatedBlock;
const size_t xArraySize = 5, xBlockSize = 1000UL;
void *pvAllocatedBlocks[ xArraySize ];

    /* Check heap stats are as expected after initialisation but before any
    allocations. */
    vPortGetHeapStats( &xHeapStats );

    /* Minimum ever free bytes remaining should be the same as the total number
    of bytes as nothing has been allocated yet. */
    configASSERT( xHeapStats.xMinimumEverFreeBytesRemaining == xHeapStats.xAvailableHeapSpaceInBytes );
    configASSERT( xHeapStats.xMinimumEverFreeBytesRemaining == xInitialFreeSpace );

    /* Nothing has been allocated or freed yet. */
    configASSERT( xHeapStats.xNumberOfSuccessfulAllocations == 0 );
    configASSERT( xHeapStats.xNumberOfSuccessfulFrees == 0 );

    /* Allocate a 1000 byte block then measure what the overhead of the
    allocation in regards to how many bytes more than 1000 were actually
    removed from the heap in order to store metadata about the allocation. */
    pvAllocatedBlock = pvPortMalloc( xBlockSize );
    configASSERT( pvAllocatedBlock );
    xMetaDataOverhead = ( xInitialFreeSpace - xPortGetFreeHeapSize() ) - xBlockSize;

    /* Free the block again to get back to where we started. */
    vPortFree( pvAllocatedBlock );
    vPortGetHeapStats( &xHeapStats );
    configASSERT( xHeapStats.xAvailableHeapSpaceInBytes == xInitialFreeSpace );
    configASSERT( xHeapStats.xNumberOfSuccessfulAllocations == 1 );
    configASSERT( xHeapStats.xNumberOfSuccessfulFrees == 1 );

    /* Allocate blocks checking some stats value on each allocation. */
    for( i = 0; i < xArraySize; i++ )
    {
        pvAllocatedBlocks[ i ] = pvPortMalloc( xBlockSize );
        configASSERT( pvAllocatedBlocks[ i ] );
        vPortGetHeapStats( &xHeapStats );
        configASSERT( xHeapStats.xMinimumEverFreeBytesRemaining == ( xInitialFreeSpace - ( ( i + 1 ) * ( xBlockSize + xMetaDataOverhead ) ) ) );
        configASSERT( xHeapStats.xMinimumEverFreeBytesRemaining == xHeapStats.xAvailableHeapSpaceInBytes );
        configASSERT( xHeapStats.xNumberOfSuccessfulAllocations == ( 2Ul + i ) );
        configASSERT( xHeapStats.xNumberOfSuccessfulFrees == 1 ); /* Does not increase during allocations. */
    }

    configASSERT( xPortGetFreeHeapSize() == xPortGetMinimumEverFreeHeapSize() );
    xMinimumFreeBytes = xPortGetFreeHeapSize();

    /* Free the blocks again. */
    for( i = 0; i < xArraySize; i++ )
    {
        vPortFree( pvAllocatedBlocks[ i ] );
        vPortGetHeapStats( &xHeapStats );
        configASSERT( xHeapStats.xAvailableHeapSpaceInBytes == ( xInitialFreeSpace - ( ( ( xArraySize - i - 1 ) * ( xBlockSize + xMetaDataOverhead ) ) ) ) );
        configASSERT( xHeapStats.xNumberOfSuccessfulAllocations == ( xArraySize + 1 ) ); /* Does not increase during frees. */
        configASSERT( xHeapStats.xNumberOfSuccessfulFrees == ( 2UL + i ) );
    }

    /* The minimum ever free heap size should not change as blocks are freed. */
    configASSERT( xMinimumFreeBytes == xPortGetMinimumEverFreeHeapSize() );
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

#endif
/////////////////////////////////////////////////
/////////////////////////////////////////////////
#ifdef __linux__
void RTOS_START(void){
    xTaskCreate( vTask3, "Task 3", 1000, NULL, 1, NULL );
    xTaskCreate( vTask4, "Task 4", 1000, NULL, 1, NULL );
    vTaskStartScheduler();
}
#ifdef CH3_TASKMANAGEMENT
float32_t a[100];
void vTask1(void* parameter)
{
    while(1){
        printf("Task 1\n");
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
void vTask2(void* parameter)
{
    while(1){
        printf("Task 2\n");
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
void vTask3(void* parameter)
{
    TickType_t xLastWaketime = xTaskGetTickCount();
    //ls1.head = NULL;
    while(1){
        printf("hello\n");
        vTaskDelayUntil(&xLastWaketime, pdMS_TO_TICKS(1000));
    }
}

void vTask4(void* parameter)
{
    while(1){
        vTaskDelay(100);
    }
}
#endif(CH3_TASKMANAGEMENT)
/* CH3_TASKMANAGEMENT ends */

void vApplicationTickHook(void){}
void vApplicationIdleHook(void)
{
//	printf("Idle\r\n");
}
#endif



