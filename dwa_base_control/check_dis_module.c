#include "check_dis_module.h"
#include<stdio.h>
#include"wiringPi.h"
#include"osp_syslog.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/select.h>
#include <fcntl.h>   //low_level i/o
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define pwmpin 1
#define mode PWM_MODE_MS

float  global_dis = -1;

void gpio_init()//echo--27    trig--28  pwm--27
{
   wiringPiSetup();  
	pinMode (27, INPUT) ; //echo
	pinMode (28, OUTPUT) ; //trig
	//pinMode (27, OUTPUT) ; //pwm sg09

/*
	pinMode (22, OUTPUT); //in1-in4 L298N
	pinMode (23, OUTPUT) ;
	pinMode (25, OUTPUT) ; //火焰传感器
	pinMode (24, OUTPUT) ; //火焰
	pinMode(pwmpin,PWM_OUTPUT);
    	pwmSetMode(mode);
//	pwmSetRange(600);//600/60 =10khz
	//启动物理开关
    pullUpDnControl(28,PUD_DOWN);
	
	//pullUpDnControl(27,PUD_DOWN);
	//pullUpDnControl(26,PUD_UP);//PULL UP3.3V
	
	digitalWrite(23,0);
	digitalWrite(24, 0) ; //
	digitalWrite(25,0);
	digitalWrite (22, 0) ;
	*/
}
//60 full;30-50%
void set_duty_speed(int duty)
{
	pwmWrite(pwmpin,duty);

}
//让多级旋转的角度
//angel  0 45 90 180
int turn_angel(int angl)
{
	int angle=angl;
	int i=0;
	float x=0;
	int k=180;//180次循环的时间够了
	while(k--)
	{
		x=11.11*i;
		digitalWrite(27,HIGH);
		delayMicroseconds(500+x);
		digitalWrite(27,LOW);
		delayMicroseconds(19500-x);
		if(i==angle)
		break;
		i++;
	}
}
int turn_angel_0()
{
	int angle;
	int i=0;
	float x=0;
	int k=50;//50次循环的时间够了
  while(k--)
	{
	
		digitalWrite(27,1);
		delayMicroseconds(1500);
		digitalWrite(27,0);
		delayMicroseconds(18500);
	
	}
	printf("turn ang over \n");
}

int turn_angel_R90()
{
	int angle;
	int i=0;
	float x=0;
	int k=50;//180次循环的时间够了
  while(k--)
	{
	
		digitalWrite(27,1);
		delayMicroseconds(2500);
		digitalWrite(27,0);
		delayMicroseconds(17500);
	
	}
	
	printf("turn ang over \n");
}
int turn_angel_L90()
{
	int angle;
	int i=0;
	float x=0;
	int k=50;//180次循环的时间够了
  while(k--)
	{
	
		digitalWrite(27,1);
		delayMicroseconds(500);
	//delayMicroseconds(2000);
		digitalWrite(27,0);
		delayMicroseconds(19500);
	//delayMicroseconds(18000);
	
	}
	printf("turn ang over \n");
}
int turn_angel_L45()
{
	int angle;
	int i=0;
	float x=0;
	int k=50;//180次循环的时间够了
  while(k--)
	{
	
		digitalWrite(27,1);
		delayMicroseconds(1000);
		digitalWrite(27,0);
		delayMicroseconds(19000);
	
	}
	printf("turn ang over \n");
}


float disMeasure(void)
{
	struct timeval tv1;
	struct timeval tv2;
	long start, stop;
	int dis;
	digitalWrite(28, LOW);
	delayMicroseconds(2);
	digitalWrite(28, HIGH);//trig
	delayMicroseconds(10);
	//发出超声波脉冲
	digitalWrite(28, LOW);
	while(!(digitalRead(27) == 1));
	gettimeofday(&tv1, NULL);
	//获取当前时间
	while(!(digitalRead(27) == 0));
	gettimeofday(&tv2, NULL);
	//获取当前时间
	start = tv1.tv_sec * 1000000 + tv1.tv_usec;
//	DEBUG(LOG_DEBUG, "start:%d \n",start);
	//微秒级的时间
	stop = tv2.tv_sec * 1000000 + tv2.tv_usec;
//	DEBUG(LOG_DEBUG, "stop:%d \n",stop);
//	DEBUG(LOG_DEBUG, "stop-start:%d \n",(stop-start));//34cm/ms
	DEBUG(LOG_DEBUG, "distance:%d \n",(stop - start)  * 34 / 2000);
	dis = (stop - start)  * 34 / 2000;//单位cm
	//求出距离
	return  (float)dis;

 }
int read_fire()
{
  if((digitalRead(25) == 0)||(digitalRead(24) == 0))
    return 0;
	else return 1;
}

char read_switch()
{
static  char btnflg =0;
  if((digitalRead(26) == 0))
  	{
  	    usleep(2);
				printf("press btn\n");
		while(!digitalRead(26) );
		printf("leave btn\n");
		btnflg=!btnflg;
   
	}
	 return  btnflg;
	
}
void turn_left()
{
 	digitalWrite(22, 0) ;
	digitalWrite(23,1);
	
	digitalWrite(24, 0) ; //
	digitalWrite(25,1);
	

}
void turn_right()
{

	digitalWrite (22, 1) ;
	digitalWrite(23,0);

	digitalWrite(24, 1) ; //
	digitalWrite(25, 0);

}
void car_forward()
{
	digitalWrite (22, 1) ;
	digitalWrite(23,0);

	digitalWrite(24, 0) ; //
	digitalWrite(25, 1);


}

void car_stop()
{

	digitalWrite (22, 0) ;
	digitalWrite(23,0);

	digitalWrite(24, 0) ; //
	digitalWrite(25, 0);

}
void cmd_rasp_send(char cmd,char value)
{
	if(cmd ==0 )
	{
		car_stop();
                set_duty_speed(0);	

	}else if(cmd ==1)
{
   car_forward();
   set_duty_speed(value);

}else  if(cmd ==3)
{
   turn_left();
   set_duty_speed(value);
}else  if(cmd ==4)
{
   turn_right();
   set_duty_speed(value);
}



}
void *getSingleUltrasonicThread(void *arg)
{

  gpio_init();
 
  while(1)
  	{
		global_dis = disMeasure();
		usleep(500000);
  }

}
