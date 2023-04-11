#include "gps.h"


#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include "PID_v1.h"
#include "kalman.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "gps_hal.h"
#include "gps_main.h"

double latitude = 0;
unsigned long latitude_t;
double longitude = 0;
unsigned int gpsheading =0;
unsigned int gpsvelocity =0;

unsigned long longitude_t;
double latitude_error;
double longitude_error;
int gps_fix = -1;



/*******************************************************************************
* function name	: IMUThread
* description	: heartbeat function ,if receive new data ,clear counter,or,
*				  call heatbeat main func
* param[in] 	: task_table[4]
* param[out] 	: none
* return 		: none
*******************************************************************************/

//gps_data_t gpsData;



