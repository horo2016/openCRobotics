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

 
void stm_cmd(int  arc ,char *argv[]  )
{
	
	float vf = 0.0,af = 0.0;
	sleep(1);
	
	if(arc > 1)
	{
		vf = atof(argv[1]);
		af = atof(argv[2]);
	}
	while(1)
	{
		cmd_send2(vf,af);
	    usleep(200000);
		
	}
}
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


	
    	/*create task STm  com queue */
	pthread_attr_init (&attr);
	pthread_attr_setschedpolicy (&attr, SCHED_RR);
	param.sched_priority = 5;
	pthread_attr_setschedparam (&attr, &param);
	pthread_create (&pthread_id, &attr, &stm_Loop, NULL);
	pthread_attr_destroy (&attr);

    stm_cmd(argc,argv);


	
}
