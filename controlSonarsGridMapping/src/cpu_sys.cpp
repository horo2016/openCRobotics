#include "cpu_sys.h"
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
//#include "PID_v1.h"
#include "kalman.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "config_conf.h"



char chargename[17]="1234567890123456";

Kalman cpuLoadFilter(0.125, 4, 1, 0);


double cpuPercentage = 0.0;
int cpuTemperature = 0;
int wifiSignalStrength = 0;

#define FILEBUFFER_LENGTH 5000
#define EMPTY_STR " "


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
* function name	: main
* description	: main function for control_engine
* param[in] 	: none
* param[out] 	: none
* return 		: 0-success,-1-fail
*******************************************************************************/

void GetWifiSignalStrength()
{
    FILE *fp = popen("/sbin/iwconfig wlan0", "r");
    if (fp != NULL)
    {
	char output[1024] = {0};
	int charCount = 0;
	do
	{
	    charCount = fread(output, 1, 1024, fp);
	    if (charCount == 0)
		break;
	    char *pos = strstr(output, "Signal level=");
	    if (pos != NULL)
	    {
		pos += 13;
		char *endPos = strchr(pos, ' ');
		char valStr[32] = {0};
		strncpy(valStr, pos, endPos - pos);
		wifiSignalStrength = atoi(valStr);
	    }
	}
	while (charCount > 0);
	pclose(fp);
    }
}


/*******************************************************************************
* function name	: getCPUPercentageThread
* description	: cpu system param
* param[in] 	: none
* param[out] 	: none
* return 		: 0-success,-1-fail
*******************************************************************************/

void *getCPUPercentageThread(void *)
{

    memset(chargename,0,16);
    get_file_line("/proc/cpuinfo",42);
    printf("device id name:%s\n",chargename);
    while (1)
    {
        char buffer[256];
        long values[2][16] = {0,0};
        for (int n = 0;n < 2; n++)
        {
            FILE *fp = fopen("/proc/stat", "r");
            fgets(buffer, 256, fp);
	        fclose(fp);

            char *t = strtok(buffer, " ");
            int idx = 0;
            while (t)
            {
                t = strtok(0, " ");
                if (t)
                {
                    values[n][idx++] = atol(t);
                }
            }
            usleep(100000);
        }
    long total1 = 0;
	long work1 = 0;
    long total2 = 0;
	long work2 = 0;
	for (int idx = 0; idx < 10; idx++)
	{
	    total1 += values[0][idx];
	    total2 += values[1][idx];
	}

	for (int idx = 0; idx <= 2; idx++)
	{
	    work1 += values[0][idx];
	    work2 += values[1][idx];
	}
	long workForPeriod = work2 - work1;
	long totalForPeriod = total2 - total1;
	double cpuPercentageNow = (double)workForPeriod / (double)totalForPeriod * 100.0;
	cpuLoadFilter.update(cpuPercentageNow);
	cpuPercentage = cpuLoadFilter.GetValue();
	usleep(50000);

    // Get CPU temperature
    FILE *temperatureFile = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    char temperatureStr[32];
    fgets(temperatureStr, sizeof(temperatureStr), temperatureFile);
    fclose (temperatureFile);
    cpuTemperature = atoi(temperatureStr) / 1000;
  //  printf("CPU Temperature: %dC\n", cpuTemperature);

    // Get Wifi signal strength
    GetWifiSignalStrength();
  //  printf("Wifi Signal Strength: %ddBm\n", wifiSignalStrength);

    
    usleep(50000);
    }
}


