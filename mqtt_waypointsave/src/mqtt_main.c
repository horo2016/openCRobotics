#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <signal.h>
#include <unistd.h>

#include <time.h> 
#include <sys/time.h> 

#include "cJSON.h"
#include <pthread.h>
#include "client_pub_sub.h"

#include <time.h>
#include <sys/time.h>

// int mysock = 0;
#define FILEBUFFER_LENGTH 5000

#define TURNROUND "turnround"
#define RUNFORWARD "runforward"
#define AVOIDANCE "avoidance"
//需要的时间
#define MINS 2 //3分钟
#define DELAY_TIME (60*MINS)

char chargename[17]="1234567890123456";


int toStop = 0;




static int GetNextPackID(void);


 char  send_buf[0xff]={0};
int Creatstatejson(float head,float roll,float pitch)
{
	
#if 0
    cJSON * root =  cJSON_CreateObject();
   if(!root) {
         printf("get root faild !\n");
     }

  //  cJSON_AddItemToObject(root, "\"devid\"", cJSON_CreateString(chargename));
    cJSON_AddItemToObject(root, "heading", cJSON_CreateNumber((short)head));
    cJSON_AddItemToObject(root, "roll", cJSON_CreateNumber((short)roll));
    cJSON_AddItemToObject(root, "pitch",cJSON_CreateNumber((short)pitch));
    cJSON_AddItemToObject(root, "cpuload", cJSON_CreateNumber((char)cpuPercentage));
    cJSON_AddItemToObject(root, "cputemp", cJSON_CreateNumber((char)cpuTemperature));
    cJSON_AddItemToObject(root, "wifisignal", cJSON_CreateNumber(wifiSignalStrength));
    cJSON_AddItemToObject(root, "velspeed", cJSON_CreateNumber((int)velspeed));
    cJSON_AddItemToObject(root, "angspeed", cJSON_CreateNumber(angspeed));
   /* cJSON_AddItemToObject(root, "targetheading", cJSON_CreateNumber((int)targetHeading));
    cJSON_AddItemToObject(root, "distance", cJSON_CreateNumber((int)waypointRange));
    cJSON_AddItemToObject(root, "nextwaypoint_lon", cJSON_CreateNumber(waypointlongitude));
    cJSON_AddItemToObject(root, "nextwaypoint_lat", cJSON_CreateNumber(waypointlatitude));
    */
    memcpy(send_buf,cJSON_Print(root),strlen(cJSON_Print(root)));
      
    
    
    cJSON_Delete(root);
#endif 
    return 0;
}
/*******************************************************************************
 * function name	: get_strchr_len
 * description	: Get the migration length of the characters in a string
 * param[in] 	: str-string, c-char
 * param[out] 	: none
 * return 		: >0-len, -1-fail
 *******************************************************************************/
int get_strchr_len(char *str, char c)
{
	int i = 0;
	int len = 0;

	if(!str)
		return -1;

	while(*(str+i++) != c)
		len ++;

	return len;
}


//打开fileName指定的文件，从中读取第lineNumber行
//返回值：成功返回1，失败返回0
int get_file_line(char *fileName,int lineNumber)
{
    FILE *filePointer;
    int i=0,j=0;
    char buffer[FILEBUFFER_LENGTH];
    char *temp;
    char *sub_str = NULL ;

    if((fileName==NULL))
    {
        return 0;
    }

    if(!(filePointer=fopen(fileName,"rb")))
    {return 0;}


    while((!feof(filePointer))&&(i<lineNumber))
    {
        if(!fgets(buffer,FILEBUFFER_LENGTH,filePointer))
        {
            return 0;
        }
        i++;//差点又忘记加这一句了
    }



    fgets(buffer,sizeof(buffer),filePointer); //读第bai10行
    printf("%s", buffer );
    sub_str = strstr(buffer, "Serial");

    if(sub_str != NULL){
        sub_str += (strlen("Serial") + 4); //
        printf("%s\n", sub_str);
       int value_len = get_strchr_len(sub_str, '\n');
      

         for(j=0;j<value_len;j++){
             chargename[j]= *(sub_str+j);
            }
         chargename[16]='\0';


    }


    fclose(filePointer);
    return 1;
}

/*******************************************************************************
 * function name	: get_value_from_config_file
 * description	: get value from config
 * param[in] 	: file-config file, key-dest string, value-key value
 * param[out] 	: none
 * return 		: 0-exist, -1-not exist
 *******************************************************************************/
int get_value_from_cmdline( char* buf, char *key, char *value)
{
	int value_len = 0 ;

	char *sub_str = NULL ;
	size_t line_len = 0 ;
	size_t len = 0;


		//printf("zdf config file : %s\n", buf);
		sub_str = strstr(buf, key);

		if(sub_str != NULL){//video="/home/linaro/video"
			sub_str += (strlen(key) + 1); //=" length is 2 bytes
		//	printf("zdf sub_str : %s\n", sub_str);
			value_len = get_strchr_len(sub_str, '\n');
		//	printf("zdf value len = %d\n", value_len);
			memcpy(value, sub_str, value_len);
			
			return 0;
		}

	


	return -1;
}
void cfinish(int sig)
{
    signal(SIGINT, NULL);
	toStop = 1;
}

void stop_init(void)
{
	signal(SIGINT, cfinish);
	signal(SIGTERM, cfinish);
}
#if 0
void User_MsgCtl(MQTT_USER_MSG  *msg)
{
	char tmp_value[36];
	switch(msg->msgqos)
	{
		case 0:
			printf("MQTT>>娑堟伅璐ㄩ噺: QoS0\n");
			break;
		case 1:
			printf("MQTT>>娑堟伅璐ㄩ噺: QoS1\n");
			break;
		case 2:
			printf("MQTT>>娑堟伅璐ㄩ噺: QoS2\n");
			break;
		default:
			printf("MQTT>>error quantity\n");
			break;
	}
	printf("MQTT>>msg topic %s\r\n",msg->topic);	
	printf("MQTT>>msg: %s\r\n",msg->msg);	
	printf("MQTT>>msg length: %d\r\n",msg->msglenth);	 
#ifdef _LOCAL_SERVER	
      if( get_value_from_cmdline((char *)msg->msg,RUNFORWARD,tmp_value )==0){
            printf("runforward %d \n",atoi(tmp_value));
			GLOBAL_STATUS = MOVE_STATUS;
			MoveDistance(atoi(tmp_value));
	}
      else  if( get_value_from_cmdline((char *)msg->msg,TURNROUND,tmp_value )==0)
	{
	   GLOBAL_STATUS = MANUAL_STATUS;
           RotateDegreesByManual(atoi(tmp_value));
	} 
	    else  if( get_value_from_cmdline((char *)msg->msg,AVOIDANCE,tmp_value )==0)
	{
	        GLOBAL_STATUS = MANUAL_STATUS;
			MoveDistanceDwa(atoi(tmp_value));
			
               // system("sudo ./dwa_control ");
	}else  if( get_value_from_cmdline((char *)msg->msg,"start",tmp_value )==0)
	{
           GLOBAL_STATUS = STANDBY_STATUS;
	   GLOBAL_SWITCH = 1;
	}else  if( get_value_from_cmdline((char *)msg->msg,"stop",tmp_value )==0)
        {
           GLOBAL_STATUS = STOP_STATUS;
           GLOBAL_SWITCH = 0;
        }
#else 


#endif
	// 澶勭悊鍚庨攢姣佹暟鎹�
	
	msg->valid  = 0;
}
#endif


/* 日志时间戳,精确到毫秒 */
char* get_stime(void)
{ 
    static char timestr[200] ={0};
    struct tm * pTempTm;
    struct timeval time;
        
    gettimeofday(&time,NULL);
    pTempTm = localtime(&time.tv_sec);
    if( NULL != pTempTm )
    {
        snprintf(timestr,199,"%04d-%02d-%02d %02d:%02d:%02d.%03ld",
            pTempTm->tm_year+1900,
            pTempTm->tm_mon+1, 
            pTempTm->tm_mday,
            pTempTm->tm_hour, 
            pTempTm->tm_min, 
            pTempTm->tm_sec,
            time.tv_usec/1000);
    }
    return timestr;
}
char  read_ID_fromSn(char *sn)
{
    
     FILE *filePointer;
    int i=0,j=0;
    char buffer[0xff];
     
   

   if(!(filePointer=fopen("sn.txt","rb")))
    {return 0;}

  
    fread(buffer,sizeof(buffer),1,filePointer);  
   	memcpy(sn , buffer,17); 
    fclose(filePointer);
	return buffer;

}

char init_config_ID()
{
 
     FILE *filePointer;
    int i=0,j=0;
    char buffer[FILEBUFFER_LENGTH];
    
    

   if(!(filePointer=fopen("/etc/os-release","rb")))
    {return 0;}

    char typeBuffer[0xff];
    fread(buffer,sizeof(buffer),1,filePointer);  
   get_value_from_cmdline(buffer,"NAME",typeBuffer);
   printf("type:%s \n",typeBuffer);
	  if(strstr("\"Raspbian GNU/Linux\"",typeBuffer)!= NULL)//ubuntu
   	{
		 
        fclose(filePointer);
              return 0;//存在树莓派
   }
  else  if(strstr("\"Ubuntu\"",typeBuffer)!= NULL)//ubuntu
   	{
		  if(!access("sn.txt",R_OK))//已经存在序列号 不能在生成序列号了
		  	{
		  	  fclose(filePointer);
              return 1;//存在	 
		  	}
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		memset(typeBuffer,0xff,0);
		sprintf(typeBuffer,"%ld%ld",tv.tv_sec,tv.tv_usec);
		printf("tv_sec %s\n", typeBuffer);
		memset(buffer,FILEBUFFER_LENGTH,0);
		sprintf(buffer,"echo %s > sn.txt",typeBuffer);
		system(buffer);

   }
    fclose(filePointer);
	return 2;

}
static int GetNextPackID(void)
{
	 static unsigned int pubpacketid = 0;
	 return pubpacketid++;
}
int CreatOnlinejson()
{
	 
	char topic_buf[0xff]={0};
	unsigned char  value_buf[0xff]={0};
	char  send_buf[0xff]={0};
    char namebuffer[0xff]={0};
	
	
	memcpy(topic_buf,"online",sizeof("online"));
    sprintf(namebuffer,"\"%s\"",chargename);

    cJSON * root =  cJSON_CreateObject();
    if(!root) {
         printf("get root faild !\n");
     }else printf("get root success!\n");

    cJSON_AddItemToObject(root, "\"type\"", cJSON_CreateString("\"1\""));
    cJSON_AddItemToObject(root, "\"sn\"", cJSON_CreateString(namebuffer));
    
    
    memcpy(value_buf,cJSON_Print(root),strlen(cJSON_Print(root)));
      
	sprintf(send_buf,"mosquitto_pub -h www.woyilian.com -t %s  -m \"%s\"  ",topic_buf,value_buf);
	system(send_buf);

    cJSON_Delete(root);

    return 0;
}
void send_Online2Monitor()
{
   CreatOnlinejson();
}

void *Mqtt_ClentTask(void *argv)
{
  if(0 ==  init_config_ID()){
      memset(chargename,0,16);
      get_file_line("/proc/cpuinfo",42);//树莓派专用
      char _buffer[0xff]={0};
 	  sprintf(_buffer,"echo \"%s\" > sn.txt",chargename);
 	  system(_buffer);
  	}else {
  	  char _buffer[0xff]={0};
  	   memset(_buffer,0xff,0);
	   read_ID_fromSn(_buffer);
	   memcpy(chargename,_buffer,16);
       chargename[16]='\0';
	}
     
    printf("device id name:%s\n",chargename);
	if(strlen(chargename)<8){
		 printf("error id \n");
		exit(0);
	}
	
    mainSub(chargename);

}
void *Mqtt_PublishTask(void *argv)
{
	
	//stop_init();
	

    sleep(3);
	while (1)
	{
		
		send_Online2Monitor();
	    sleep(DELAY_TIME);

	}

	return 0;
}
