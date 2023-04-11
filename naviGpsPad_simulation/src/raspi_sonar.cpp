#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <iostream> 
#include <string>
#include <vector>
#include "osp_syslog.h"
#include"wiringPi.h"
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
#include "raspi_sonar.h"
#include "stm32_control.h"
#include "check_dis_module.h"
#include "raspi_sonar.h"
#include "navi_manage.h"
#include "imu.h"
#include "dwa_demo.h"

#include "SimpleKalmanFilter.h"

 //gcc raspi_sonar.cpp -lpthread -lstdc++

//#include <boost/algorithm/string.hpp>

double min_freq = 0.5;
double max_freq = 60;



uint8_t leftSensor;             //Store the sensor's value.
uint8_t centerSensor;
uint8_t rightSensor;

bool isObstacleLeft;           //If obstacle detected or not.
bool isObstacleCenter;
bool isObstacleRight;


uint8_t MIN_RANGE_OBSTACLE = 5; //Between 0 and 5 cm is the blind zone of the sensor.
uint8_t MAX_RANGE_OBSTACLE = 65; //The maximum range to check if obstacle exists.
 

SimpleKalmanFilter KF_Left(2, 2, 0.01);
SimpleKalmanFilter KF_Center(2, 2, 0.01);
SimpleKalmanFilter KF_Right(2, 2, 0.01);

enum NavigationStates {
  CHECK_ALL,
  FULL_SPEED,
  SPEED_DECREASE,
  CHECK_OBSTACLE_POSITION,
  LEFT,
  CENTER,
  RIGHT,
  BACK
};
NavigationStates _navState = CHECK_ALL;

int gpio = -1;
pthread_t* sonarthread;

class Sonar {
public:
    // wiringPi GPIO pins
    int trigger_pin;
    int echo_pin;
    int id;

    uint32_t start_tick;
    uint32_t elapsed_ticks;

    std::string frame;
     

    bool range_error = false;

     
    Sonar(int trigger_pin, int echo_pin, int id)
    {
        this->trigger_pin = trigger_pin;
        this->echo_pin = echo_pin;
        this->id = id;

        start_tick = 0;
        elapsed_ticks = 0;

       
    }

  /*  void range_check(diagnostic_updater::DiagnosticStatusWrapper &stat)
    {
        if (range_error) {
            stat.summary(diagnostic_msgs::DiagnosticStatus::WARN, "Range out of bounds!");
        }
        else {
            stat.summary(diagnostic_msgs::DiagnosticStatus::OK, "Range within bounds!");
        }
    }*/
};


static std::vector<Sonar> sonars;
#define HC_SR04_READ_TIMEOUT 100000 //us -> 100ms

static unsigned int  time_spent = 0;
sonar_dis  raspi_sonars[3];
static void hc_sr04_clear_timeout(void)
{
    time_spent = 0;
}

static unsigned char  hc_sr04_is_timeout(unsigned short sleep_us)
{
    time_spent += sleep_us;

    unsigned char is_timeout = 1;
    if (time_spent >= HC_SR04_READ_TIMEOUT) {
        is_timeout = 0;
    }

    return is_timeout;
}
/* Trigger the next sonar */
void sonar_trigger(int sonar)
{
    int pin = sonars[sonar].trigger_pin;

	digitalWrite(pin, LOW);
	delayMicroseconds(2);
	digitalWrite(pin, HIGH);//trig
	delayMicroseconds(10);
	//发出超声波脉冲
	digitalWrite(pin, LOW);

}




/* Handle pin change */
float echo_callback( uint32_t id)
{
	struct timeval tv1;
	struct timeval tv2;
	long start, stop;
	int dis;
	int pin = sonars[id].echo_pin;
    hc_sr04_clear_timeout();
	while((digitalRead(pin) != 1) && (hc_sr04_is_timeout(5))){
	      usleep(5);
	}
	gettimeofday(&tv1, NULL);
	hc_sr04_clear_timeout();
	while((digitalRead(pin) != 0) && (hc_sr04_is_timeout(100))){
	  usleep(100);
	}
	gettimeofday(&tv2, NULL);
	 
	start = tv1.tv_sec * 1000000 + tv1.tv_usec;
//	DEBUG(LOG_DEBUG, "start:%d \n",start);
	//微秒级的时间
	stop = tv2.tv_sec * 1000000 + tv2.tv_usec;
//	DEBUG(LOG_DEBUG, "stop:%d \n",stop);
//	DEBUG(LOG_DEBUG, "stop-start:%d \n",(stop-start));//34cm/ms
	if((stop - start) >= 38000)
	      return 5.0;
//	DEBUG(LOG_DEBUG, "distance:%d cm\n",(stop - start)  * 34 / 2000);
	dis = (stop - start)  * 34 / 2000;// 
	 
	return  (float)dis;
     
     
     
}

int setup_gpio()
{
    wiringPiSetup();
	
    for (const auto& sonar : sonars) {
	
        
		pinMode (sonar.echo_pin,INPUT ) ;//echo
		pinMode (sonar.trigger_pin,OUTPUT ) ; 
		digitalWrite(sonar.trigger_pin, LOW);
	    
      
    }

    return true;
}


bool my_caculate_direction()
{

	if (isObstacleLeft == 0 && isObstacleRight == 0) {
          
          if(heading > targetHeading )
		  	return true ;//turn left 
		  	else return false ;
        }

}
//Obstacle avoidance algorithm.
void obstacleAvoidance()
{

#if 1
  switch (_navState) {
    case CHECK_ALL: { //if no obstacle, go forward at maximum speed
        if (isObstacleLeft == 0 && isObstacleCenter == 0 && isObstacleRight == 0) {
          _navState = FULL_SPEED;
        } else {
          
          _navState = SPEED_DECREASE;
        }
      } break;
 
    case FULL_SPEED: {
		 if(velspeed < MAX_SPEED)
         cmd_send2(velspeed + SPEED_RESO,0.0);
		 else  cmd_send2(MAX_SPEED,0.0);
        _navState = CHECK_ALL;
      } break;
 
    case SPEED_DECREASE: {
        cmd_send2(0.0,0.0);
		//dwa_loop(float meters);
        //Wait for few more readings at low speed and then go to check the obstacle position
         _navState = CHECK_OBSTACLE_POSITION;
      } break;
 
    case CHECK_OBSTACLE_POSITION: 

	    dwa_loop(3.0);
         _navState = FULL_SPEED;
        break;
        
  	}
    
   

	#endif
	
}



 //Define the minimum and maximum range of the sensors, and return true if an obstacle is in range.
 bool obstacleDetection(char id,int sensorRange) {
 if(id ==0){
	     if ((MIN_RANGE_OBSTACLE <= sensorRange) && (sensorRange <= MAX_RANGE_OBSTACLE/3))
		 	return true; 
	   else return false;
 	}
else if(id == 1){
	     if ((MIN_RANGE_OBSTACLE <= sensorRange) && (sensorRange <= MAX_RANGE_OBSTACLE))
		 	return true; 
	   else return false;
 	}
else  if(id ==2){
	     if ((MIN_RANGE_OBSTACLE <= sensorRange) && (sensorRange <= MAX_RANGE_OBSTACLE/3))
		 	return true; 
	   else return false;
 	}
 }

 //Apply Kalman Filter to sensor reading.
 void applyKF() {
   isObstacleLeft = obstacleDetection(2,(raspi_sonars[2].distance));
   isObstacleCenter = obstacleDetection(1,(raspi_sonars[1].distance));
   isObstacleRight = obstacleDetection(0,(raspi_sonars[0].distance));
 }
 int main_sonar()
 {
	 
 
	 double field_of_view;
	 double min_range;
	 double max_range;
 
 
	 // pin numbers are specific to the hardware
	 sonars.push_back(Sonar(28, 29, 0));
	 sonars.push_back(Sonar(26, 27, 1));
	 sonars.push_back(Sonar(24, 25, 2));
	// sonars.push_back(Sonar(27, 22, 3));
	// sonars.push_back(Sonar(19, 26, 4));
 
	 if (!setup_gpio()) {
		 printf("Cannot initalize gpio");
		 return 1;
	 }
 
  
 
	  
	  
	 int id =0;
	 while (1) {
		 for (auto& sonar: sonars) {
			 sonar_trigger(sonar.id);
				 if(sonar.id == 0)
				   raspi_sonars[sonar.id].distance =  KF_Right.updateEstimate(echo_callback(sonar.id))  ;
				 else if(sonar.id == 1)
					 raspi_sonars[sonar.id].distance =	KF_Center.updateEstimate(echo_callback(sonar.id))  ;
				 else if(sonar.id == 2)
					 raspi_sonars[sonar.id].distance =	KF_Left.updateEstimate(echo_callback(sonar.id))  ;
									  
 			//	printf("%ddis:%f \n",sonar.id,raspi_sonars[sonar.id].distance );
				 applyKF();
			 usleep(1000);
		 }
		
	 }
  
 
	 return 0;
 }

 void *getUltrasonicThread(void *arg)
 {
	 main_sonar();
	 
 }
