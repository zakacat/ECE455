/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
#include <stdlib.h> //Added for random function
#include <inttypes.h>
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"



/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 3

#define amber  	0
#define green  	1
#define red  	2
#define blue  	3

#define amber_led	LED3
#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6


#define TASK_STACK_SIZE    256
#define TASK_PRIORITY      tskIDLE_PRIORITY + 1

#define TASK_1_PERIOD			250
#define TASK_1_EXECUTION_TIME 	95

#define TASK_2_PERIOD			500
#define TASK_2_EXECUTION_TIME	150

#define TASK_3_PERIOD			750
#define TASK_3_EXECUTION_TIME	250

#define TASK_HYPERPERIOD		1500

#define TIMER_PERIOD_MS_1    pdMS_TO_TICKS(TASK_1_PERIOD) // .5-second timer period
#define TIMER_PERIOD_MS_2    pdMS_TO_TICKS(TASK_2_PERIOD) // .5-second timer period
#define TIMER_PERIOD_MS_3    pdMS_TO_TICKS(TASK_3_PERIOD) // .75-second timer period
#define TIMER_PERIOD_MS_4    pdMS_TO_TICKS(TASK_HYPERPERIOD) //


/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );


static void Scheduler_DD( void *pvParameters );
static void User_Defined_Task_1( void *pvParameters );
static void User_Defined_Task_2( void *pvParameters );
static void User_Defined_Task_3( void *pvParameters );
static void Generator_DD_1( void *pvParameters );
static void Generator_DD_1_Callback( );
static void Generator_DD_2( void *pvParameters );
static void Generator_DD_2_Callback( );
static void Generator_DD_3( void *pvParameters );
static void Generator_DD_3_Callback( );
static void Monitor_Task( void *pvParameters );
static void Monitor_Callback( );

xQueueHandle xQueue_handle1 = 0; //The queue for sending new tasks
xQueueHandle xQueue_handle2 = 0; //The queue for completing tasks
xQueueHandle xQueue_handle3 = 0; //The queue for pinging the DDS to print the list lengths

static QueueSetHandle_t xQueueSet = NULL;

typedef enum {PERIODIC,APERIODIC} task_type ;//Should they all be in real time?

typedef struct dd_task { //A dd_task which will likely be food
	TaskHandle_t t_handle;
	task_type type;
	uint32_t task_id; //Can be in real time
	uint32_t release_time; //Should be in real time
	uint32_t absolute_deadline; //Should be in real time
	uint32_t completion_time; //Should be in real time
	uint32_t execution_time;//Execution time
} dd_task;

typedef struct dd_task_node{ // Probably this will be represented by the chef's cooking queue
	struct dd_task *task;
	struct dd_task_node *prev_task_node;
	struct dd_task_node *next_task_node;
} dd_task_node;

dd_task activeList;
dd_task completedList;
dd_task overdueList;

//This function receives all of the information necessary to create a
//new dd_taskstruct(excluding the release time and completion time.
static void release_dd_task_func(TaskHandle_t, task_type, uint32_t, uint32_t, uint32_t); //The is the same as the create

//This function receives the ID of the DD-Task which has completed its execution.
//The ID is packaged as a message and sent to a queue for the DDS to receive
static void complete_dd_task_func(uint32_t); //This is the same as delete

//This function sends a message to a queue requesting the Active Task List from the DDS.
//Once a response is received from the DDS, the function returns the list.
static void print_all_dd_task_list_lengths();


/*-----------------------------------------------------------*/

TaskHandle_t scheduler_handle;
TaskHandle_t t_handle_1;
TaskHandle_t t_handle_2;
TaskHandle_t t_handle_3;
TaskHandle_t t_handle_4;

int main(void)
{
	/* Initialize LEDs */

	STM_EVAL_LEDInit(amber_led);
	STM_EVAL_LEDInit(green_led);
	STM_EVAL_LEDInit(red_led);
	STM_EVAL_LEDInit(blue_led);


	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
	prvSetupHardware();


	/* Create the queue used by the queue send and queue receive tasks.
	http://www.freertos.org/a00116.html */


	xQueue_handle1 = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( struct dd_task ) );	/* The size of each item the queue holds. */

	xQueue_handle2 = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
								sizeof( uint32_t ) );	/* The size of each item the queue holds. */

	xQueue_handle3 = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
								sizeof( uint32_t ) );	/* The size of each item the queue holds. */


	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( xQueue_handle1, "Generator tasks" );

	vQueueAddToRegistry( xQueue_handle2, "Complete" );

	vQueueAddToRegistry( xQueue_handle3, "Monitor print all tasks." );

	xQueueSet = xQueueCreateSet( 3 * 3);

	xQueueAddToSet( xQueue_handle1, xQueueSet );
	xQueueAddToSet( xQueue_handle2, xQueueSet );
	xQueueAddToSet( xQueue_handle3, xQueueSet );



	//All these priorities are currently the same
	scheduler_handle = xTaskCreate( Scheduler_DD, "Scheduler_DD", configMINIMAL_STACK_SIZE, NULL, 6, NULL);

	xTaskCreate( Generator_DD_1, "Generator_DD_1", configMINIMAL_STACK_SIZE, NULL, 4, &t_handle_1);

	//printf ("Heap size %u \n" , (unsigned int)xPortGetFreeHeapSize() );

	xTaskCreate( Generator_DD_2, "Generator_DD_2", configMINIMAL_STACK_SIZE, NULL, 4, &t_handle_2);

	//printf ("Heap size %u \n" , (unsigned int)xPortGetFreeHeapSize() );

	xTaskCreate( Generator_DD_3, "Generator_DD_3", configMINIMAL_STACK_SIZE, NULL, 4, &t_handle_3);

	//printf ("Heap size %u \n" , (unsigned int)xPortGetFreeHeapSize() );

	xTaskCreate( Monitor_Task, "Monitor_Task", configMINIMAL_STACK_SIZE, NULL, 4, &t_handle_4);

	//printf ("Heap size %u \n" , (unsigned int)xPortGetFreeHeapSize() );

	TimerHandle_t xTaskCreationTimer_1;
	xTaskCreationTimer_1 = xTimerCreate("TaskTimer_1", TIMER_PERIOD_MS_1, pdTRUE, NULL, Generator_DD_1_Callback);

	if (xTaskCreationTimer_1 != NULL && xTimerStart(xTaskCreationTimer_1, 0) == pdPASS)
	{
		//printf("Timer started successfully.\n");
	}
	else
	{
		printf("Timer creation or start failed!\n");
	}

	TimerHandle_t xTaskCreationTimer_2;
	xTaskCreationTimer_2 = xTimerCreate("TaskTimer_2", TIMER_PERIOD_MS_2, pdTRUE, NULL, Generator_DD_2_Callback);

	if (xTaskCreationTimer_2 != NULL && xTimerStart(xTaskCreationTimer_2, 0) == pdPASS)
	{
		//printf("Timer started successfully.\n");
	}
	else
	{
		printf("Timer creation or start failed!\n");
	}

	TimerHandle_t xTaskCreationTimer_3;
	xTaskCreationTimer_3 = xTimerCreate("TaskTimer_3", TIMER_PERIOD_MS_3, pdTRUE, NULL, Generator_DD_3_Callback);

	if (xTaskCreationTimer_3 != NULL && xTimerStart(xTaskCreationTimer_3, 0) == pdPASS)
	{
		//printf("Timer started successfully.\n");
	}
	else
	{
		printf("Timer creation or start failed!\n");
	}

	TimerHandle_t xTaskCreationTimer_4;
	xTaskCreationTimer_4 = xTimerCreate("TaskTimer_4", TIMER_PERIOD_MS_4, pdTRUE, NULL, Monitor_Callback);

	if (xTaskCreationTimer_4 != NULL && xTimerStart(xTaskCreationTimer_4, 0) == pdPASS)
	{
		//printf("Timer started successfully.\n");
	}
	else
	{
		printf("Timer creation or start failed!\n");
	}
//	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

static void Generator_DD_1_Callback(TimerHandle_t xTimer){
	//printf("Gen 1 successfully called.\n");
	vTaskResume(t_handle_1);
}
static void Generator_DD_2_Callback(TimerHandle_t xTimer){
	//printf("Gen 2 successfully called.\n");
	vTaskResume(t_handle_2);
}
static void Generator_DD_3_Callback(TimerHandle_t xTimer){
	//printf("Gen 3 successfully called.\n");
	vTaskResume(t_handle_3);
}
static void Monitor_Callback(TimerHandle_t xTimer){
	//printf("Monitor successfully called.\n");
	vTaskResume(t_handle_4);
}


//This function receives all of the information necessary to create a
//new dd_taskstruct(excluding the release time and completion time.
static void release_dd_task_func(TaskHandle_t t_handle, task_type type, uint32_t task_id, uint32_t execution_time ,uint32_t absolute_deadline){//The is the same as the create


	struct dd_task *newTask = malloc(sizeof(dd_task));

	newTask->t_handle = t_handle;
	newTask->type = type;
	newTask->task_id = task_id;
	newTask->absolute_deadline = absolute_deadline;
	//newTask->execution_time = execution_time;
//	printf("Task Sent to DDS!: %u ON!\n", (int)task_id);

	if( xQueueSend(xQueue_handle1, &newTask, 100)) // Try to put the task into the queue
	{
		//printf("Task Sent to DDS!: %u ON!\n", (int)newTask->task_id);

	}

	//vTaskResume(scheduler_handle);

	//This one creates the_dd_task and then adds it to the activeList
}

//This function receives the ID of the DD-Task which has completed its execution.
//The ID is packaged as a message and sent to a queue for the DDS to receive
static void complete_dd_task_func(uint32_t task_id){//This is the same as delete

	if( xQueueSend(xQueue_handle2, task_id, 100));

}

//This function sends a message to a queue requesting the Active Task List from the DDS.
//Once a response is received from the DDS, the function returns the list.
static void print_all_dd_task_list_lengths(){

	uint32_t count = 1;

	if( xQueueSend(xQueue_handle3,&count,100)) // Try to put the task into the queue
	{
		//printf("Active List Length Request Sent to DDS!: %d ON!\n", (int)count);
	}


}



/*-----------------------------------------------------------*/

static void Scheduler_DD( void *pvParameters )
{
	//This will be EDF, earliest deadline first
	//This is the only thing that touches the list
	dd_task* newTask;
	dd_task_node* activeHead = malloc(sizeof(dd_task_node));
	dd_task_node* activeTail = malloc(sizeof(dd_task_node));
	dd_task_node* activeCurr = malloc(sizeof(dd_task_node));

	dd_task_node* completeHead = malloc(sizeof(dd_task_node));
	dd_task_node* completeTail = malloc(sizeof(dd_task_node));
	dd_task_node* completeCurr = malloc(sizeof(dd_task_node));

	dd_task_node* overdueHead = malloc(sizeof(dd_task_node));
	dd_task_node* overdueTail = malloc(sizeof(dd_task_node));


	uint32_t activeCount = 0;
	uint32_t completeCount = 0;
	uint32_t overdueCount = 0;

	dd_task* dumbTask = malloc(sizeof(dd_task));
	dumbTask->task_id = 0;

	activeHead->task = dumbTask;
	activeHead->prev_task_node = NULL;
	activeHead->next_task_node = activeTail;

	activeTail->task = dumbTask;
	activeTail->next_task_node = NULL;
	activeTail->prev_task_node = activeHead;

	completeHead->task = dumbTask;
	completeHead->prev_task_node = NULL;
	completeHead->next_task_node = completeTail;

	completeTail->task = dumbTask;
	completeTail->next_task_node = NULL;
	completeTail->prev_task_node = completeHead;

	overdueHead->task = dumbTask;
	overdueHead->prev_task_node = NULL;
	overdueHead->next_task_node = completeTail;

	overdueTail->task = dumbTask;
	overdueTail->next_task_node = NULL;
	overdueTail->prev_task_node = completeHead;


	TickType_t currentTime;

	xQueueHandle dummyqueue;

	dd_task* priorityTask = malloc(sizeof(dd_task_node));

	int shortestDeadline;

	while(1)
	{
		//printf("INSIDE DDS \n ");
		//printf ("Heap size %u \n" , (unsigned int)xPortGetFreeHeapSize() );
		//if there is a new task waiting to be added to the active task list


		dummyqueue = ( QueueHandle_t ) xQueueSelectFromSet (xQueueSet, portMAX_DELAY );

		if (dummyqueue == xQueue_handle1)
		{
			if(xQueueReceive(xQueue_handle1, &newTask, 0))
			{

				currentTime = xTaskGetTickCount();

				printf("Task ID: %u Release Time: %u \n", (int)newTask->task_id, (int)currentTime);

				newTask->release_time = currentTime;
				//-----------------
				int deadline = 0;

				int period = newTask->absolute_deadline;

				deadline += period * 3;

				deadline /= 4;

				deadline = deadline + currentTime;

				newTask->absolute_deadline = deadline;

				dd_task_node* newNode = malloc(sizeof(dd_task_node));

				newNode->task = newTask;

				activeTail->prev_task_node->next_task_node = newNode;

				newNode->prev_task_node = activeTail->prev_task_node;

				newNode->next_task_node = activeTail;

				activeTail->prev_task_node = newNode;

				activeCount++ ;

			}
		}


		//-----------------

		uint32_t task_id;

		//if a task is completed, will be removed from active list and sent to completed
		if (dummyqueue == xQueue_handle2) //DATA PRESENT AT Q2
		{
			if(xQueueReceive(xQueue_handle2, &task_id, 0))
			{

				//printf("DDS Q2 add task to complete list %u \n", (int)task_id);

				currentTime = xTaskGetTickCount();

				printf("Task ID: %u Complete Time: %u \n", (int)task_id, (int)currentTime);

				activeCurr = activeHead;

				while (activeCurr->task->task_id != task_id)
				{
					//printf("Inside while loop Q2 %u \n", (int)activeCurr->task->task_id);
					activeCurr = activeCurr->next_task_node;
				}

				//printf("LOOKING FOR %d  FOUND %d  \n", (int)task_id, (int)activeCurr->task->task_id );

				//printf("CHECKING CURRENT TIME %d \n", (int)currentTime);
				//printf("CHECKING DEADLINE TIME %d \n", (int)activeCurr->task->absolute_deadline);

				if(  currentTime < activeCurr->task->absolute_deadline  ) //TASK COMPLETED ON TIME
				{

					activeCurr->prev_task_node->next_task_node = activeCurr->next_task_node;

					//printf("ID %d \n", (int)task_id, (int)activeCurr->next_task_node->task->task_id );

					activeCurr->next_task_node->prev_task_node = activeCurr->prev_task_node;

					//printf("LOOKING FOR %d  FOUND %d  \n", (int)task_id, (int)activeCurr->prev_task_node->task->task_id );

					activeCurr->prev_task_node = completeTail->prev_task_node;

					//printf("LOOKING FOR %d  FOUND %d  \n", (int)task_id, (int)completeTail->prev_task_node->task->task_id );

					activeCurr->next_task_node = completeTail;

					//printf("LOOKING FOR %d  FOUND %d  \n", (int)task_id, (int)completeTail->task->task_id );

					completeTail->prev_task_node = activeCurr;

					//printf("LOOKING FOR %d  FOUND %d  \n", (int)task_id, (int)activeCurr->task->task_id );

					completeCount++;
				}

				else //TASK NOT COMPLETED ON TIME
				{

					activeCurr->prev_task_node->next_task_node = activeCurr->next_task_node;

					activeCurr->next_task_node->prev_task_node = activeCurr->prev_task_node;

					activeCurr->prev_task_node = overdueTail->prev_task_node;

					activeCurr->next_task_node = overdueTail;

					overdueTail->prev_task_node = activeCurr;

					overdueCount++;
				}


				activeCount--;


				activeCurr = activeHead->next_task_node;

				//printf("LOOKING FOR %d  FOUNDDDD %d  \n", (int)task_id, (int)activeHead->next_task_node->task->task_id );
				//printf("LOOKING FOR %d  FOUNDDDD %d  \n", (int)task_id, (int)activeCurr->task->task_id );
				shortestDeadline = 2147483647;

				while (activeCurr->task->task_id != 0)
				{
					if(shortestDeadline > activeCurr->task->absolute_deadline){
						//printf("Inside while loop: shortestDeadline  %u \n", (int)shortestDeadline);
						//printf("Inside while loop: absolute_deadline  %u \n", (int)activeCurr->task->absolute_deadline);
						shortestDeadline = activeCurr->task->absolute_deadline;
						priorityTask = activeCurr->task;
					}
					//printf("Inside while loop %u \n", (int)activeCurr->task->task_id);
					activeCurr = activeCurr->next_task_node;
				}

				if(priorityTask->task_id != 0){
					vTaskPrioritySet(priorityTask->t_handle, 2);
				}

			}
		}

		uint32_t checkTasks = 0;

		//if the monitor task wants to know how many active tasks there are
		if(xQueueReceive(xQueue_handle3, &checkTasks, 0))
		{
			//printf("DDS Q3 \n" );
			if(checkTasks == 1){

				printf("The amount of active tasks are %d \n", (int)activeCount);
				printf("The amount of completed tasks are %d \n", (int)completeCount);
				printf("The amount of overdue tasks are %d \n", (int)overdueCount);

			}
		}

		//printf("Scheduler_DD DONE !\n");
	}
}


/*-----------------------------------------------------------*/

static void User_Defined_Task_1( void *pvParameters )
{
	STM_EVAL_LEDOn(amber_led);

	//printf("User_Defined_Tasks !\n");
	TickType_t startTime;

	startTime = xTaskGetTickCount();

	TickType_t endTime;

	endTime = startTime + (pdMS_TO_TICKS(TASK_1_EXECUTION_TIME));

	while(xTaskGetTickCount()<endTime)
	{
		//A User-Defined Task would normally perform something useful such as a calculation;
		//for this project, however, it may simply execute an empty loop for the duration of its execution time.
		//It is recommended to make use of the LEDs on the discovery board during development in order to provide
		//a visual indication of which User-Defined Task is currently executing

	}

	complete_dd_task_func( pvParameters);
	//printf("called complete(), now back in USER DEFINED TASK\n");

	STM_EVAL_LEDOff(amber_led);
	vTaskDelete(NULL);

}
/*-----------------------------------------------------------*/

static void User_Defined_Task_2( void *pvParameters )
{

	STM_EVAL_LEDOn(green_led);

	//printf("User_Defined_Tasks !\n");
	TickType_t startTime;

	startTime = xTaskGetTickCount();

	TickType_t endTime;

	endTime = startTime + (pdMS_TO_TICKS(TASK_2_EXECUTION_TIME));

	while(xTaskGetTickCount()<endTime)
	{
		//A User-Defined Task would normally perform something useful such as a calculation;
		//for this project, however, it may simply execute an empty loop for the duration of its execution time.
		//It is recommended to make use of the LEDs on the discovery board during development in order to provide
		//a visual indication of which User-Defined Task is currently executing
	}

	complete_dd_task_func( pvParameters);
	//printf("called complete(), now back in USER DEFINED TASK\n");

	STM_EVAL_LEDOff(green_led);
	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

static void User_Defined_Task_3( void *pvParameters )
{

	STM_EVAL_LEDOn(blue_led);
	//printf("User_Defined_Tasks !\n");
	TickType_t startTime;

	startTime = xTaskGetTickCount();

	TickType_t endTime;

	endTime = startTime + (pdMS_TO_TICKS(TASK_3_EXECUTION_TIME));

	while(xTaskGetTickCount()<endTime)
	{
		//A User-Defined Task would normally perform something useful such as a calculation;
		//for this project, however, it may simply execute an empty loop for the duration of its execution time.
		//It is recommended to make use of the LEDs on the discovery board during development in order to provide
		//a visual indication of which User-Defined Task is currently executing
	}

	complete_dd_task_func( pvParameters);
	//printf("called complete(), now back in USER DEFINED TASK\n");
	STM_EVAL_LEDOff(blue_led);
	vTaskDelete(NULL);
}


/*-----------------------------------------------------------*/

static void Generator_DD_1( void *pvParameters)
{
	uint32_t taskCount = 100;
	while(1)
	{
		vTaskSuspend(NULL);

		TaskHandle_t t_handle;

		uint32_t task_id = taskCount;
		taskCount ++;

		xTaskCreate( User_Defined_Task_1, "User_Defined_Task_1", configMINIMAL_STACK_SIZE, &task_id, 1, &t_handle);

		task_type type = PERIODIC;

		uint32_t execution_time;
		uint32_t absolute_deadline;



		execution_time = TASK_1_EXECUTION_TIME;
		absolute_deadline = TASK_1_PERIOD;

		//printf("DD task created %u !\n", task_id);

		release_dd_task_func(t_handle, type, task_id, execution_time, absolute_deadline);

	}
}

/*-----------------------------------------------------------*/

static void Generator_DD_2( void *pvParameters )
{
	uint32_t taskCount = 200;
	while(1)
	{
		vTaskSuspend(NULL);

		TaskHandle_t t_handle;

		uint32_t task_id = taskCount;
		taskCount ++;

		xTaskCreate( User_Defined_Task_2, "User_Defined_Task_2", configMINIMAL_STACK_SIZE, &task_id, 1, &t_handle);

		task_type type = PERIODIC;

		uint32_t execution_time;
		uint32_t absolute_deadline;

		execution_time = TASK_2_EXECUTION_TIME;
		absolute_deadline = TASK_2_PERIOD;

		//printf("DD task created %u !\n", task_id);

		release_dd_task_func(t_handle, type, task_id, execution_time, absolute_deadline);
	}
}

/*-----------------------------------------------------------*/

static void Generator_DD_3( void *pvParameters )
{
	uint32_t taskCount = 300;
	while(1)
	{
		vTaskSuspend(NULL);

		TaskHandle_t t_handle;

		uint32_t task_id = taskCount;
		taskCount ++;

		xTaskCreate( User_Defined_Task_3, "User_Defined_Task_3", configMINIMAL_STACK_SIZE, &task_id, 1, &t_handle);

		task_type type = PERIODIC;

		uint32_t execution_time;
		uint32_t absolute_deadline;


		execution_time = TASK_3_EXECUTION_TIME;
		absolute_deadline = TASK_3_PERIOD;


		//printf("DD task created %u !\n", task_id);

		release_dd_task_func(t_handle, type, task_id, execution_time, absolute_deadline);

	}
}


/*-----------------------------------------------------------*/

static void Monitor_Task( void *pvParameters )
{

	while(1)
	{
		vTaskSuspend(NULL);

		print_all_dd_task_list_lengths();

	}
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	printf("FUCK MEMORY ERROR NOOOO!!! \n");
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}
