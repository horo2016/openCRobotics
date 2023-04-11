/*
 *
* serial port programming
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <iostream>

#include <sys/time.h>
#include <time.h>	
#include <sys/ipc.h>
#include <sys/shm.h>
#include "gps_apply.h"

#include "coordinate_sys.h"
#include "cJSON.h"
#include "serial.h"
#include "osp_syslog.h"
#include "cpu_sys.h"
#include "gps_main.h"
#include "imu.h"
#include "gps.h"
#include "mqtt_main.h"
#include "navi_manage.h"

// void gps_init(double noise);
// void gps_update(double lat, double lon, double seconds_since_last_timestep);
// void gps_read(double* lat, double* lon);
// bool gpsparser(char* data, double* lon, double* lat, double* HDOP, int* numSV)
#define MQTT_SIP "gpscar.xiaovdiy.cn"

#define MYKEY 1234
#define BUF_SIZE 1024


using namespace std;
const double timestep = 0.100;
const double noise = 1.000;
typedef struct
{    
    char isvalid;
    Location gpsInf;
    float gpsheading;
    float gpsvelocity;
}use_shared;
unsigned short point1 = 0;
unsigned short  point_start = 0;

_SaveData Save_Data;


int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop) {
	struct termios newtio, oldtio;
	if (tcgetattr(fd, &oldtio) != 0) {
		perror("SetupSerial 1");
		return -1;
	}
	memset(&newtio, 0, sizeof(newtio));

	/*
	* Enable the receiver and set local mode...
	*/
	newtio.c_cflag |= CLOCAL | CREAD;

	/*
	* Set Data Bits
	*/
	newtio.c_cflag &= ~CSIZE;
	switch (nBits) {
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	/*
	* Set Parity Bit
	*/
	switch (nEvent) {
	case 'O':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E':
		newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':
		newtio.c_cflag &= ~PARENB;
		break;
	}

	/*
	*	Set Stop Bit
	*/
	if (nStop == 1)
		newtio.c_cflag &= ~CSTOPB;
	else if (nStop == 2)
		newtio.c_cflag |= CSTOPB;
	/*
	*	Set BaudRate
	*/
	switch (nSpeed) {
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	case 460800:
		cfsetispeed(&newtio, B460800);
		cfsetospeed(&newtio, B460800);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}

	/*
	* Disable Hardware Flow Control
	*/
	newtio.c_cflag &= ~CRTSCTS;

	/*
	* Disable Software Flow Control
	*/
	newtio.c_iflag &= ~(IXON | IXOFF | IXANY);

	/******************** Noncanonical Mode***********************/
	// 
	// // Set Min Character & Waiting Time
	// 
	// newtio.c_cc[VTIME] = 1; //segement group
	// newtio.c_cc[VMIN] = 68;

	
	// // use raw input and output
	
	// newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
	// newtio.c_oflag &= ~OPOST; /*Output*/
	/********************************************/

	/******************** Canonical Mode***********************/
	/*
	 Raw 妯″紡杈撳嚭.
	*/
 	newtio.c_oflag = 0;
 
	/*
	  ICANON  : 鑷磋兘鏍囧噯杈撳叆, 浣挎墍鏈夊洖搴旀満鑳藉仠鐢�, 骞朵笉閫佸嚭淇″彿浠ュ彨鐢ㄧ▼搴�
	*/
 	newtio.c_lflag = ICANON;
 	/********************************************/

	/*
	* Clear Input Queue
	*/
	tcflush(fd, TCIFLUSH); //TCOFLUSH,TCIOFLUSH

	/*
	* Enforce Now
	*/
	if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) {
		perror("com set error");
		return -1;
	}

	printf("set done!\n");
	return 0;
}

int open_port(int fd, int comport) {
	//char* dev[] = {"/dev/ttyS0","/dev/ttyS1","/dev/ttyUSB0"};
	//long vdisable;
	if (comport == 1) {
		fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
		if (-1 == fd) {
			perror("Can't Open Serial Port");
			return (-1);
		}
		else
			printf("open ttyAMA0 ......\n");
	}
	else if (comport == 2) {
		fd = open("/dev/ttyS5", O_RDWR | O_NOCTTY | O_NDELAY);
		if (-1 == fd) {
			perror("Can't Open Serial Port");
			return (-1);
		}
		else
			printf("open ttygps ......\n");
	}
	else if (comport == 3) {
		fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
		if (-1 == fd) {
			perror("Can't Open Serial Port");
			return (-1);
		}
		else
			printf("open ttygps ......\n");
	}
	else if (comport == 4) {
		fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
		if (-1 == fd) {
			perror("Can't Open Serial Port");
			return (-1);
		}
		else
			printf("open ttygps ......\n");
	}
	/*
	*	Block the serial port
	*/
	if (fcntl(fd, F_SETFL, 0) < 0)
		printf("fcntl failed!\n");
	else
		printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));
	
	/*
	*	Test whether port is attached to terminal device
	*/
	if (isatty(STDIN_FILENO) == 0)
		printf("standard input is not a terminal device\n");
	else
		printf("isatty success!\n");
	
	printf("fd-open=%d\n", fd);
	return fd;
}

int Creatstatejson(Location gpsval,float h,float vel)
{
	char tmp_buf[0xff]={0};
	char topic_buf[0xff]={0};
	unsigned char  value_buf[4096]={0};
	 char  send_buf[4096]={0};

	//memcpy(topic_buf,"/state/gps",sizeof("/state/gps"));


    cJSON * root =  cJSON_CreateObject();
  if(!root) {
         printf("get root faild !\n");
     } 

    cJSON_AddItemToObject(root, "\"isvalid\"", cJSON_CreateNumber(1));
    cJSON_AddItemToObject(root, "\"lonti\"", cJSON_CreateNumber(gpsval.lng));//ìí?óname?úμ?
    cJSON_AddItemToObject(root, "\"lati\"",cJSON_CreateNumber(gpsval.lat));//ìí?óname?úμ?
    cJSON_AddItemToObject(root, "\"gpsheading\"", cJSON_CreateNumber(h));//
    cJSON_AddItemToObject(root, "\"gpsvelocity\"",cJSON_CreateNumber(vel)); 
	cJSON_AddItemToObject(root, "\"heading\"", cJSON_CreateNumber(h));
    cJSON_AddItemToObject(root, "\"roll\"", cJSON_CreateNumber(0.0));
    cJSON_AddItemToObject(root, "\"pitch\"",cJSON_CreateNumber(0.0));
    cJSON_AddItemToObject(root, "\"cpuload\"", cJSON_CreateNumber((char)cpuPercentage));
    cJSON_AddItemToObject(root, "\"cputemp\"", cJSON_CreateNumber((char)cpuTemperature));
  //  cJSON_AddItemToObject(root, "\"wifisignal\"", cJSON_CreateNumber(wifiSignalStrength));
  //  cJSON_AddItemToObject(root, "\"velspeed\"", cJSON_CreateNumber((int)velspeed));
  //  cJSON_AddItemToObject(root, "\"angspeed\"", cJSON_CreateNumber(angspeed));
   // cJSON_AddItemToObject(root, "\"targetheading\"", cJSON_CreateNumber((int)targetHeading));
   // cJSON_AddItemToObject(root, "\"distance\"", cJSON_CreateNumber((int)waypointRange));
     cJSON_AddItemToObject(root, "\"nextwaypoint_lon\"", cJSON_CreateNumber(waypointlongitude));
     cJSON_AddItemToObject(root, "\"nextwaypoint_lat\"", cJSON_CreateNumber(waypointlatitude));
   // mqtt_publish(tmp_buf,cJSON_Print(root));
    memcpy(value_buf,cJSON_Print(root),strlen(cJSON_Print(root)));
      
	sprintf(topic_buf,"%s/state/gps",chargename);

#if defined MQTT_REMOTE_SERVER 
		sprintf(send_buf,"mosquitto_pub -h www.woyilian.com -t %s  -m \"%s\"",topic_buf,value_buf);
#elif defined MQTT_TERMINAL_SERVER
			sprintf(send_buf,"mosquitto_pub -t %s  -m \"%s\"",topic_buf,value_buf);
#endif



	system(send_buf);
	


    cJSON_Delete(root);

    return 0;
}

/*******************************************************************************
* function name	: parseGpsBuffer
* description	: 
*
* param[in] 	: 
* param[out] 	: none
* return 		: none
*******************************************************************************/
void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
	//	printf("**************\r\n");
		//printf(Save_Data.GPS_Buffer);

		for (i = 0 ; i <= 9; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					printf("error");	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取经度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W
						case 7:memcpy(Save_Data.earthSpeed, subString, subStringNext - subString);
						//	  DEBUG(LOG_DEBUG, "speed:%s \n",Save_Data.earthSpeed);
						break;
						case 8:memcpy(Save_Data.earthHeading, subString, subStringNext - subString);
							//  DEBUG(LOG_DEBUG, "head:%s \n",Save_Data.earthHeading);
						break;
						case 9:memcpy(Save_Data.UTCDate, subString, subStringNext - subString);
						//	DEBUG(LOG_DEBUG, "UTCDate:%s \n",Save_Data.UTCDate);
						break;
						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
						//解析错误
				}
			}


		}
	}
}
/*******************************************************************************
* function name	: parseGpsBuffer
* description	: 
*
* param[in] 	: 
* param[out] 	: none
* return 		: none
*******************************************************************************/
int gps_Data_Deal(unsigned char *datv,int length)
{
	unsigned char Res;
	unsigned char i=0;
	
    //printf("igps rec len:%d  \n",length);
    for(i=0;i<length;i++)
   	{       
	//	printf("%c",datv[i]);
	    if(datv[i]=='\n')	
			break;
    }
	point1 =i;
	//DEBUG(LOG_DEBUG,"gps rec total:%d \n",i);
	 for(i=0;i<length;i++)
   	{
        if(datv[i]=='G')
        	{
		 	  point_start=i;
	//		DEBUG(LOG_DEBUG,"gps rec start:%d  \n",point_start);
			break;
        	}
    }

       if((datv[point_start] == 'G') && (datv[3+point_start] == 'M') && (datv[point_start+4] == 'C'))//GPRMC/GNRMC

	if((datv[point_start] == 'G') && (datv[3+point_start] == 'M') && (datv[point_start+4] == 'C'))//确定是否收到"GPRMC/GNRMC"这一帧数据
	{
		 
	 //   DEBUG(LOG_DEBUG,"gps head analysis ok  \n");
		if(datv[point1] == '\n')									   
		{
		 //   DEBUG(LOG_DEBUG,"gps rec end:%d  \n",point1);
			memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);      //清空
			memcpy(Save_Data.GPS_Buffer, datv+point_start, point1); 	//保存数据
			Save_Data.isGetData = true;
			point1 = 0;
			point_start =0;
			//memset(USART_RX_BUF, 0, USART_REC_LEN);      //清空				
		}	
				
	}

     parseGpsBuffer();
    // printGpsBuffer();


}
Location gps_random_location_simutaion(Location l)
{
	Location _r;
	int ret = rand() % 180 + 1;//生成1~180度的随机数
    int dis = rand() % 30 + 20;//生成1~10米的随机数
	printf("generate ang:%d,dis:%d  \n",ret,dis);
 	float x=dis * cos(ret*M_PI/180);
 	float y=dis * sin(ret*M_PI/180);
	printf("generate x,y:(%f,%f)  \n",x,y);
	//1米大约0.000004
	_r.lat = l.lat +  0.000003*x;
	_r.lng = l.lng +  0.000004*y;
	printf("generate new locatation  x,y:(%f,%f)  \n",_r.lat,_r.lng);
	return _r;
}


Location gps_vstep_location_simutaion(Location l)
{
	Location _r = l;
	 
	if(car_run_simulation < 0.1) 
	 return _r;
 	float x=car_run_simulation * cos(car_heading_simulation*M_PI/180);
 	float y=car_run_simulation * sin(car_heading_simulation*M_PI/180);
	printf("generate x,y:(%f,%f)  \n",x,y);
	//1米大约0.000004
	_r.lat = l.lat +  0.000059*x;
	_r.lng = l.lng +  0.000059*y;
	printf("generate new locatation  x,y:(%f,%f)  \n",_r.lat,_r.lng);
	return _r;
}
double getDistancebaidu(Location startCoord, Location endCoord)
{
    double distance = acos(sin(startCoord.lat) * sin(endCoord.lat) + cos(startCoord.lat) * cos(endCoord.lat) * cos(endCoord.lng - startCoord.lng) ) * 6371;

    return distance;
}
int gps_task(void) {
	
	printf("gps stask Starting! ......\n\n");
  	FILE *waypointFile = fopen("waypoints.csv", "r");
    if (waypointFile == NULL)
     {
	 DEBUG(LOG_ERR,"waypoints read err \n");	
	 return 0;
	}
   Location first_waypoint;
    while (!feof(waypointFile))
    {
	char line[256];
	fgets(line, 256, waypointFile);
	const char *wpLat  = strtok(line, ",");
	const char *wpLong= strtok(0, ",");
	
	if (wpLat && wpLong)
	{
	 first_waypoint.lat =  atof(wpLat);
	 first_waypoint.lng = atof(wpLong);
	 break ;
	   
	}
    }
    printf("the first latitude:%f longititude %f \n",first_waypoint.lat , first_waypoint.lng);
    waypointlongitude =first_waypoint.lng;
	waypointlatitude =first_waypoint.lat;
    fclose(waypointFile);
	 
	Location loc_coor = gps_random_location_simutaion(first_waypoint);
	printf("new locatation  x,y:(%f,%f)  \n",loc_coor.lat,loc_coor.lng);
	 Location loc_coor_current;
   	while(1){
   		 
			printf("current locatation  x,y:(%f,%f)  \n",loc_coor.lat,loc_coor.lng);
			 loc_coor_current = gps_vstep_location_simutaion(loc_coor);
			 double d = getDistancebaidu(loc_coor,loc_coor_current);
			printf("curredis :(%f)  \n",d);
			Creatstatejson(loc_coor_current,(float)car_heading_simulation,d);	
			
			latitude =loc_coor_current.lat ;
			longitude 	=loc_coor_current.lng;		
			loc_coor = loc_coor_current;
			sleep(3);			
							 
						 
	
		
	}



	exit(EXIT_SUCCESS);

	return 0;
}
void *GpsThread(void *)
{
	
    // GpsHandle();
	 gps_task();
}

