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

/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
//Set up the GPIO pins for the LEDs
static void Setup_GPIO_Traffic();

static void Setup_GPIO_Pot();
//Serial to Parallel Converter
static void Setup_SPC();
//Set up the ADC peripheral for potentiometer
static void Setup_ADC();



static void Deadline_Driven_Scheduler( void *pvParameters );
static void User_Defined_Task( void *pvParameters );
static void Deadline_Driven_Task_Generator( void *pvParameters );
static void Monitor_Task( void *pvParameters );


xQueueHandle xQueue_handle = 0; //The queue for sending new tasks
xQueueHandle xQueue_handle2 = 0; //The queue for how many active tasks there are
xQueueHandle xQueue_handle3 = 0;

typedef enum {PERIODIC,APERIODIC} task_type ;//Should they all be in real time?

typedef struct dd_task { //A dd_task which will likely be food
	TaskHandle_t t_handle;
	task_type type;
	uint32_t task_id; //Can be in real time
	uint32_t release_time; //Should be in real time
	uint32_t absolute_deadline; //Should be in real time
	uint32_t completion_time; //Should be in real time
	uint32_t execution_time;//Execution time

	struct dd_task *next_task;
} dd_task;

//typedef struct dd_task_list{ // Probably this will be represented by the chef's cooking queue
//	struct dd_task task;
//	struct dd_task_list *next_task;
//} dd_task_list;

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
static void get_active_dd_task_list();

//This function sends a message to a queue requesting the Completed Task List from the DDS.
//Once a response is received from the DDS, the function returns the list
static void get_completed_dd_task_list();

//This function sends a message to a queue requesting the Overdue Task List from the DDS.
//Once a response is received from the DDS, the function returns the list.
static void get_overdue_dd_task_list();

/*-----------------------------------------------------------*/

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
	xQueue_handle = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( struct dd_task ) );	/* The size of each item the queue holds. */

	xQueue_handle2 = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
								sizeof( uint32_t ) );	/* The size of each item the queue holds. */

	xQueue_handle3 = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
								sizeof( uint32_t ) );	/* The size of each item the queue holds. */

	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( xQueue_handle, "Generator tasks" );

	vQueueAddToRegistry( xQueue_handle2, "Active list length request" );

	vQueueAddToRegistry( xQueue_handle3, "Active list length value" );

	//All these priorities are currently the same
	xTaskCreate( Deadline_Driven_Scheduler, "Deadline_Driven_Scheduler", configMINIMAL_STACK_SIZE, NULL, 10, NULL);
	//xTaskCreate( User_Defined_Task, "User_Defined_Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Deadline_Driven_Task_Generator, "Deadline_Driven_Task_Generator", configMINIMAL_STACK_SIZE, NULL, 9, NULL);
	//xTaskCreate( Monitor_Task, "Monitor_Task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	Setup_GPIO_Traffic();

	Setup_GPIO_Pot();

	Setup_ADC();

	Setup_SPC();

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

//This function receives all of the information necessary to create a
//new dd_taskstruct(excluding the release time and completion time.
static void release_dd_task_func(TaskHandle_t t_handle, task_type type, uint32_t task_id, uint32_t execution_time ,uint32_t absolute_deadline){//The is the same as the create


	struct dd_task *newTask = malloc(sizeof(dd_task));

	newTask->t_handle = t_handle;
	newTask->type = type;
	newTask->task_id = task_id;
	newTask->absolute_deadline = absolute_deadline;
	newTask->execution_time = execution_time;
	printf("Task Sent to DDS!: %u ON!\n", (int)task_id);

	if( xQueueSend(xQueue_handle, &newTask, 100)) // Try to put the task into the queue
	{
		printf("Task Sent to DDS!: %u ON!\n", (int)newTask->task_id);
		//vTaskDelay(200);//Add a delay to allow other tasks to go
	}


	//This one creates the_dd_task and then adds it to the activeList
}

//This function receives the ID of the DD-Task which has completed its execution.
//The ID is packaged as a message and sent to a queue for the DDS to receive
static void complete_dd_task_func(uint32_t task_id){//This is the same as delete


}

//This function sends a message to a queue requesting the Active Task List from the DDS.
//Once a response is received from the DDS, the function returns the list.
static void get_active_dd_task_list(){

	uint32_t count = 1;

	if( xQueueSend(xQueue_handle2,&count,100)) // Try to put the task into the queue
	{
		printf("Active List Length Request Sent to DDS!: %d ON!\n", (int)count);
		//vTaskDelay(200);//Add a delay to allow other tasks to go
	}

	//wait for something else to be added to the queue
//	vtaskdelay(100);
//
//	if(xQueueReceive(xQueue_handle3, &count, 100))
//	{
//		printf("Active List Length Recevied! \n");
//	}

//	return count;


}

//This function sends a message to a queue requesting the Completed Task List from the DDS.
//Once a response is received from the DDS, the function returns the list
//static dd_task_list get_completed_dd_task_list(){
//
//	dd_task_list completedList;
//
//	return completedList;
//}

//This function sends a message to a queue requesting the Overdue Task List from the DDS.
//Once a response is received from the DDS, the function returns the list.
//static dd_task_list get_overdue_dd_task_list(){
//
//	dd_task_list overdueList;
//
//	return overdueList;
//}


//Set up the GPIO pins for the LEDs
static void Setup_GPIO_Traffic(){

	printf("Setup_GPIO_Traffic enter\n");

	//Enable the RCC peripheral clock port C GPIOs
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);

	//Create intialization struct
	GPIO_InitTypeDef GPIO_InitStruct;

	//Configure Pin 0 = RED Light, PIN 1 = YELLER, PIN 2 = GREEN
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ;
	//Set Mode to output
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	//Set to Push-Pull
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//Set speed to slowest speed to avoid noise
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	//Set to no pull
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	//Initialize the port
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	printf("Setup_GPIO_Traffic exit\n");

}

static void Setup_GPIO_Pot(){

	printf("Setup_GPIO_Pot enter\n");
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);

	//Create intialization struct
	GPIO_InitTypeDef GPIO_InitStruct;
	//Isolate Pin C3
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
	//Set Mode to analog in
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	//Set to no pull
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

	//Initialize the port
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	printf("Setup_GPIO_Pot exit\n");

}
//Serial to Parallel Converter
static void Setup_SPC(){

	printf("Setup_SPC enter\n");
	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE);

	//Create intialization struct
	GPIO_InitTypeDef GPIO_InitStruct;

	//Isolate Pin C8 = shift reset, C7 = shift clock, C6 = shift data, C4 = 2nd shift clock
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_7| GPIO_Pin_6 | GPIO_Pin_4;
	//Set Mode to output
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	//Set to Push-Pull
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//Set speed to slowest speed to avoid noise
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	//Set to no pull
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;



	//Initialize the port
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	//1 means not reset
	GPIO_SetBits(GPIOC, GPIO_Pin_8);
	//Turn off the first SPC clock
	GPIO_ResetBits(GPIOC, GPIO_Pin_7);
	printf("Setup_SPC exit\n");

}

//Set up the ADC peripheral for potentiometer
static void Setup_ADC(){

	printf("Setup_ADC enter\n");

	//Enable the RCC ADC 1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_ADC1EN, ENABLE);

	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;

	//I don't think that we care about the data alignment as long as it is consistent
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;

	ADC_InitStruct.ADC_ExternalTrigConv = DISABLE;

	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;

	ADC_InitStruct.ADC_NbrOfConversion = 1;

	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;

	ADC_InitStruct.ADC_ScanConvMode = DISABLE;

	ADC_Init(ADC1, &ADC_InitStruct);

	//Set the ADC command
	ADC_Cmd(ADC1, ENABLE);

	//Config the channel
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_84Cycles);

	printf("Setup_ADC exit\n");
}


/*-----------------------------------------------------------*/

static void Deadline_Driven_Scheduler( void *pvParameters )
{
	//This will be EDF, earliest deadline first
	//This is the only thing that touches the list
	dd_task* newTask;
	dd_task* activeHead;
	dd_task* activeTail;
	uint32_t count;

	while(1)
	{

		//if there is a new task waiting to be added to the active task list
		if(xQueueReceive(xQueue_handle, &newTask, 500))
		{

			printf("DDS add task to active list %u \n", (int)newTask->task_id);
			if(activeTail == NULL){
				activeTail = newTask;
				activeHead = newTask;
			}
			else{
				activeTail->next_task = newTask;
			}
			//activeTail->task = newTask;
			//activeTail = activeTail->next_task;

			//include logic for rearranging the list
			//check for earliest deadline
			//start the first task using the handle

			printf("DDS add task to active list pointer %p \n", (void *) &activeList);
		}


		//if the monitor task wants to know how many active tasks there are
		if(xQueueReceive(xQueue_handle2, &count, 500))
		{
			printf("The amount of active tasks are %d \n", (int)count);
			if(count == 1){

				count = 0;

				dd_task* countList = activeHead;

				//printf("DDS count list pointer %p \n", (void *) countList);
				while(countList->next_task != NULL){
					count ++;

					countList = countList->next_task;
				}

			}
			printf("The amount of active tasks are %u", count);

		}

		printf("Deadline_Driven_Scheduler !\n");


		vTaskDelay(500); //Pass off the CPU to any task that needs it

	}
}

/*-----------------------------------------------------------*/

static void User_Defined_Task( void *pvParameters )
{



	while(1)
	{
		//A User-Defined Task would normally perform something useful such as a calculation;
		//for this project, however, it may simply execute an empty loop for the duration of its execution time.
		//It is recommended to make use of the LEDs on the discovery board during development in order to provide
		//a visual indication of which User-Defined Task is currently executing

		//STM_EVAL_LEDOn(amber_led);

		//STM_EVAL_LEDOff(amber_led);

		//printf("User_Defined_Tasks !\n");
		vTaskDelay(500);

	}
}


/*-----------------------------------------------------------*/

static void Deadline_Driven_Task_Generator( void *pvParameters )
{
	uint32_t taskCount = 1;
	while(1)
	{
		uint32_t now = (unsigned)time(NULL);
		//Prepare task specific information create an instance of a DD task
		//by calling release_dd_task()

		TaskHandle_t t_handle;

		xTaskCreate( User_Defined_Task, "User_Defined_Task", configMINIMAL_STACK_SIZE, NULL, 1, &t_handle);
		//vTaskSuspend(t_handle);

		task_type type = PERIODIC;

		uint32_t task_id = taskCount;
		taskCount ++;

		uint32_t execution_time; // or 150 or 250
		uint32_t absolute_deadline;

		//task 1
		if(taskCount % 1 == 0){

			execution_time = 95; // or 150 or 250
			absolute_deadline = now + 500; //Deadline is the now plus period
		}
		else if (taskCount % 2 == 0){ //task 2
			execution_time = 150; // or 150 or 250
			absolute_deadline = now + 500; //Deadline is the now plus period
		}
		else { //task 3
			execution_time = 250; // or 150 or 250
			absolute_deadline = now + 750; //Deadline is the now plus period
		}

		printf("DD task created %u !\n", task_id);

		release_dd_task_func(t_handle, type, task_id, execution_time, absolute_deadline);


		vTaskDelay(2000);

	}
}

/*-----------------------------------------------------------*/

static void Monitor_Task( void *pvParameters )
{
	uint32_t numberOfActiveTasks;
	uint32_t numberOfCompletedTasks;
	uint32_t numberOfOverdueTasks;

	while(1)
	{
		//Collects information from the DDS using the get_active_dd_task_list,
		//get_complete_dd_task_list, and get_overdue_dd_task_listfunctions.

		//Report the number of tasks in each list to the user (e.g.print text to the console)

		//Priority: Must be allowed to execute even if there are active or overdue tasks so that it can
		//continue to collect information and report system information

		//Would be nice to hook this up to the user button
		get_active_dd_task_list();


		printf("Monitor_Task!\n");
		vTaskDelay(5000);
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

