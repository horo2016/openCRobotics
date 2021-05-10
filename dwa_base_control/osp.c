#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

/*pthread*/
#include "sys/ipc.h"
#include "sys/msg.h"
#include "pthread.h"
#include <arpa/inet.h>
#include <net/if.h>
/*app*/
#include "stm32_control.h"

/*Mqtt about*/

#include <time.h> 
#include <sys/time.h> 

#include "check_dis_module.h"
#include "dwa_demo.h"


#define USE_ULTRASONIC 1


/*******************************************************************************
* function name	: main
* description	: main function for control_engine
* param[in] 	: none
* param[out] 	: none
* return 		: 0-success,-1-fail
*******************************************************************************/
int  main (int argc, char ** argv)
{
	
	pthread_attr_t attr;
    pthread_t pthread_id = 0 ;
	struct sched_param param;
	/*create task thread */
#if 0
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, IMUThread, NULL);
	pthread_attr_destroy (&attr);
    	/*create task thread */
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, GpsThread, NULL);
	pthread_attr_destroy (&attr);
    
		/*create task thread */
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, &Socket_Task_handle, NULL);
	pthread_attr_destroy (&attr);
	   /*create task mqtt  com queue */
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, &Mqtt_PublishTask, NULL);
	pthread_attr_destroy (&attr);
	  /*create task mqtt  client queue */
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, &Mqtt_ClentTask, NULL);
	pthread_attr_destroy (&attr);
    /*create task cpu system task */
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, &getCPUPercentageThread, NULL);
	pthread_attr_destroy (&attr);
#endif
    	/*create task STm  com queue */
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, &stm_Loop, NULL);
	pthread_attr_destroy (&attr);

	#ifdef USE_ULTRASONIC
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, &getSingleUltrasonicThread, NULL);
	pthread_attr_destroy (&attr);

	#endif
	dwa_main();
	/*create task message queue */
//	navimanage_handle(NULL);
}
