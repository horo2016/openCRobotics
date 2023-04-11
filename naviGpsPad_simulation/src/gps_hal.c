#include "gps_hal.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h> 
#include <sys/time.h> 
#include <sys/shm.h>
#include<pthread.h>
#include"osp_syslog.h"
#include "gps.h"
#include "cJSON.h"
#include "cpu_sys.h"
#include "gps_main.h"
#include "mqtt_main.h"
/*******************************************************************************
* function name	: IMUThread
* description	: heartbeat function ,if receive new data ,clear counter,or,
*				  call heatbeat main func
* param[in] 	: task_table[4]
* param[out] 	: none
* return 		: none
*******************************************************************************/

//gps_data_t gpsData;
#define MYKEY 1234
#define BUF_SIZE 1024




typedef struct
{    
    char isvalid;
    Location gpsInf;
    float gpsheading;
    float gpsvelocity;
}use_shared;
int CreatstateGpsjson(Location gpsval,float h,float vel)
{
	char tmp_buf[0xff]={0};
	char topic_buf[0xff]={0};
	unsigned char  value_buf[0xff]={0};
	 char  send_buf[0xff]={0};

    memcpy(topic_buf,chargename,sizeof(chargename));
	memcpy(topic_buf+strlen(chargename),"/state/gps",sizeof("/state/gps"));
printf("topic:%s \n",topic_buf);

    cJSON * root =  cJSON_CreateObject();
  if(!root) {
         printf("get root faild !\n");
     }
 //   cJSON_AddItemToObject(root, "\"type\"", cJSON_CreateNumber(0));//?�?��???��?�
  //  cJSON_AddItemToObject(root, "\"devid\"", cJSON_CreateString(chargename));
    cJSON_AddItemToObject(root, "\"isvalid\"", cJSON_CreateNumber(1));
    cJSON_AddItemToObject(root, "\"lonti\"", cJSON_CreateNumber(gpsval.lng));//��?�name?��?
    cJSON_AddItemToObject(root, "\"lati\"",cJSON_CreateNumber(gpsval.lat));//��?�name?��?
    cJSON_AddItemToObject(root, "\"gpsheading\"", cJSON_CreateNumber(h));//
    cJSON_AddItemToObject(root, "\"gpsvelocity\"",cJSON_CreateNumber(vel)); 
   // mqtt_publish(tmp_buf,cJSON_Print(root));
    memcpy(value_buf,cJSON_Print(root),strlen(cJSON_Print(root)));
      

	sprintf(send_buf,"mosquitto_pub -h www.woyilian.com -t %s  -m \"%s\"",topic_buf,value_buf);
	system(send_buf);
	// printf("%s\n", tmp_buf);
        //printf("%s\n", publishstring.pMessage);

    cJSON_Delete(root);

    return 0;
}


