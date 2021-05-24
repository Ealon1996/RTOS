
/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"		/* RTOS firmware */
#include "task.h"			/* Task */
#include "timers.h"
#include "arm_math.h"
#include "topic.h"
#include "spectrum.h"
#include "stdint.h"
#include "semphr.h"

#include "FreeRTOS_CLI.h"
/* Examples */
/*
void vAssertCalled( unsigned long ulLine, const char * const pcFileName );
void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
    taskENTER_CRITICAL();
    {
        printf("[ASSERT] %s:%lu\n", pcFileName, ulLine);
        fflush(stdout);
    }
    taskEXIT_CRITICAL();
    exit(-1);
}
*/
/* --------------------------------------------- */
#define CH3_TASKMANAGEMENT
#ifdef CH3_TASKMANAGEMENT
void vTask1(void*);
void vTask2(void*);
void vTask3(void*);
void vTask4(void*);

#endif

void vApplicationIdleHook(void);
void vApplicationTickHook(void);


void RTOS_START(){
    xTaskCreate( vTask3, "Task 3", 1000, NULL, 1, NULL );
    xTaskCreate( vTask4, "Task 4", 1000, NULL, 1, NULL );
    vTaskStartScheduler();
}

topic_list_t tls;
uint8_t* topic1 = "/test1";
uint8_t* topic2 = "/test2";
uint8_t* topic3 = "/test3";


uint8_t* task1_name = "task1";
uint8_t* task2_name = "task2";
uint8_t* task3_name = "task3";
topic_list_t ls;
subscriber_list_t sub_ls;

typedef struct node{
    uint8_t id;
    uint32_t value;

    struct node* next;
}node_t;

typedef struct list{
    node_t* head;
    uint32_t len;
}list_t;

node_t* create_node(uint8_t id, uint32_t value){
    node_t* new_node = (node_t*)pvPortMalloc(sizeof(node_t));
    new_node->id = id;
    new_node->value = value;
    new_node->next = NULL;
    return new_node;
}

void add_node(list_t* ls,uint8_t id, uint32_t value){
    node_t* current = ls->head;
    if(ls->head ==NULL){
        ls->head = create_node(id,value);
    }else{
        while (current->next!=NULL) {
            current = current->next;
        }
        current->next = create_node(id,value);
    }
}


void delete_list(list_t* ls){
    node_t* current=ls->head;
    node_t* tmp = NULL;
    while (current!=NULL) {
        tmp=current->next;
        vPortFree(current);
        current = tmp;
    }
    ls->head=NULL;
}

SemaphoreHandle_t xSemaphore=NULL;
int main ( void )
{
    /*
    topic_list_init(&ls);

    add_topic(&ls,0,topic1,sizeof(msg1_t));
    add_topic(&ls,1,topic2,sizeof(msg2_t));
    add_topic(&ls,2,topic3,sizeof(msg3_t));

    add_subs(&ls,topic1,task1_name,0,task1_msg_cb,0);

    add_subs(&ls,topic2,task3_name,0,task3_msg_cb,0);
    add_subs(&ls,topic2,task2_name,0,task2_msg_cb,0);

    printf("ok\n");

    topic_nodes_t* tp = ls.head;

    while (tp!=NULL) {
        subscriber_t* sub=  tp->subscriber_list->head;
        printf("%s\n",tp->name);
        while(sub!=NULL){
            uint8_t test;
            uint32_t len;
            sub->cb(&test,len);

            sub=sub->next;
        }

        tp = tp->next;
    }
*/

xSemaphore = xSemaphoreCreateMutex();
if(xSemaphore == NULL){
printf("error\n");
}
   RTOS_START();
}




#ifdef CH3_TASKMANAGEMENT
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
float32_t a[100];
void vTask3(void* parameter)
{
    uint8_t* ptr=NULL;
    HeapStats_t state;
    TickType_t xLastWaketime = xTaskGetTickCount();
    list_t ls;
    list_t ls1;
    ls.head=NULL;
    //ls1.head = NULL;
    while(1){
    if(xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE ){
        //vPortGetHeapStats(&state);
        printf("----task1-----\n");

        printf("%x\n",xPortGetFreeHeapSize());

        add_node(&ls,1,1);
        add_node(&ls,2,2);


       // add_node(&ls1,1,1);
        //add_node(&ls1,2,2);

        printf("%x\n",xPortGetFreeHeapSize());
        //vPortFree(ptr);
        delete_list(&ls);
        //delete_list(&ls1);
        printf("%x\n",xPortGetFreeHeapSize());
        xSemaphoreGive( xSemaphore );

    }


        vTaskDelayUntil(&xLastWaketime, pdMS_TO_TICKS(1000));
    }
}

void vTask4(void* parameter)
{
    TickType_t xLastWaketime = xTaskGetTickCount();
    list_t lsx;
    lsx.head = NULL;
    while(1){
        if(xSemaphoreTake( xSemaphore, ( TickType_t ) 10 ) == pdTRUE ){
            //vPortGetHeapStats(&state);
            printf("----task2-----\n");

            printf("%x\n",xPortGetFreeHeapSize());

           add_node(&lsx,1,1);
           add_node(&lsx,2,2);

            printf("%x\n",xPortGetFreeHeapSize());
            //vPortFree(ptr);
            delete_list(&lsx);

            printf("%x\n",xPortGetFreeHeapSize());
            xSemaphoreGive( xSemaphore );
        }

        vTaskDelayUntil(&xLastWaketime, pdMS_TO_TICKS(1000));
    }
}

#endif
/* CH3_TASKMANAGEMENT ends */

void vApplicationTickHook(void){

}
void vApplicationIdleHook(void)
{
//	printf("Idle\r\n");
}
/*-----------------------------------------------------------*/
