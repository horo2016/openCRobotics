#include "navi_manage.h"


#include "osp_syslog.h"

#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
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
//#include "include.h"
#include "geocoords.h"
#include "gps.h"

#include <stdexcept>
#include <errno.h>
 
#include"stm32_control.h"



#include<sched.h>
#include "dwa_demo.h"





char  GLOBAL_STATUS =0;
char  GLOBAL_SWITCH =0;
float rollOffset = 0.0; //-0.4;
float pitchOffset = 0.0; // 3.3;


Kalman headingFilter(0.125, 4, 1, 0);

// PID controller variables for heading
double targetHeading, headingPIDInput, headingPIDOutput;
PID headingPID(&headingPIDInput, &headingPIDOutput, &targetHeading,1,0,0, DIRECT);




double waypointlongitude;
double waypointlatitude;
GeoCoordinate waypoints[256];
int waypointCount = 0;
int currentWaypoint = 0;
double waypointRange = 0.0;


float voltage1;
unsigned long voltage1_t;
float voltage2;
unsigned long voltage2_t;

bool voltageHysteresis = 0;


int rotatethreadid =0;
int movethreaddwaid =0;

//单位是度
int car_heading_simulation =0;
//单位是m
float car_run_simulation =0;
float car_speed_from_web=0.1;

//500ms的预测时间,a的单位是rad
//直角坐标系  正右是0 正前是90，后方是-90。0-180
//百度坐标系 顺时针为正传为大于0  逆时针小于0
void cmd_send_vstep_simulation(float v,float a)
{
  
    car_heading_simulation += a*0.5*180/M_PI;



 if (car_heading_simulation > 180)
     car_heading_simulation -= 360;
     else if (car_heading_simulation < -180)
     car_heading_simulation += 360;
 
 if(v > 0)
   car_run_simulation =   v*0.5;

   if(v == 0)
   {
      car_run_simulation =0;

   }
    
}

/*******************************************************************************
 * function name        : is_file_exist
 * description  : set video and picture path of storage
 * param[in]    : path-file path
 * param[out]   : none
 * return              : 0-exist, -1-not exist
 *******************************************************************************/
int is_file_exist(const char *path)
{
        if(path == NULL)
                return -1;

        if(0 == access(path, F_OK))
                return 0;

        return -1;
}

/*******************************************************************************
* function name	: ReadWaypointsFile
* description	: from file waypoint.dat read datas from internet ,format :lat|long  
*				  call heatbeat main func
* param[in] 	: 将经纬度从文件中读到waypoints 中
* param[out] 	: none
* return 		: none
 *******************************************************************************/

void ReadWaypointsFile()
{
    FILE *waypointFile = fopen("waypoints.csv", "r");
    if (waypointFile == NULL)
     {
	 DEBUG(LOG_ERR,"waypoints read err \n");	
	 return;
	}
    waypointCount = 0;
    while (!feof(waypointFile))
    {
	char line[256];
	fgets(line, 256, waypointFile);
	const char *wpLat  = strtok(line, ",");
	const char *wpLong = strtok(0, ",");
	if (wpLat && wpLong)
	{
	    GeoCoordinate waypoint(wpLat,wpLong);
	    waypoints[waypointCount] = waypoint;
	    waypointCount++;
	}
    }
    printf("the first latitude:%f longititude %f \n",waypoints[0].latitude,waypoints[0].longitude);
    printf("waypoints all count is %d read from waypoint file \n",waypointCount);
    fclose(waypointFile);
}
// Quick and dirty function to get elapsed time in milliseconds.  This will wrap at 32 bits (unsigned long), so
// it's not an absolute time-since-boot indication.  It is useful for measuring short time intervals in constructs such
// as 'if (lastMillis - millis() > 1000)'.  Just watch out for the wrapping issue, which will happen every 4,294,967,295
// milliseconds - unless you account for this, I don't recommend using this function for anything that will cause death or
// disembowelment when it suddenly wraps around to zero (e.g. avionics control on an aircraft)...
unsigned long getmillis()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned long count = tv.tv_sec * 1000000 + tv.tv_usec;
    return count / 1000;
}

/*****
 *    判断是否在范围内
 * @param raduis    圆的半径
 * @param lat       点的纬度
 * @param lng       点的经度
 * @param lat1      圆的纬度
 * @param lng1      圆的经度
 * @return  
 */
 char isInRange(int raduis,double present_lat,double present_lng,double lat_circle,double lng_circle){
    
    double R = 6378137.0;
    float const PI_F = 3.14159265F;
    double dLat = (lat_circle- present_lat ) * M_PI / 180;
    double dLng = (lng_circle - present_lng )* M_PI / 180;
    double a = sin(dLat / 2) * sin(dLat / 2) + cos(present_lat * PI / 180) * cos(lat_circle* PI / 180) * sin(dLng / 2) * sin(dLng / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = R * c;
    double dis = round(d);//近似值
    DEBUG(LOG_DEBUG,"caculate  distance :%.2f\n",dis);
    if (dis <= raduis){  //点在圆内
        return 1;
    }else {
        return 0;
    }
}
 
 // Initialization stuff - open and configure the serial device, etc.
 void Setup()
 {

 
     // Set up the PID controllers for heading and wall following
     headingPIDInput = 0;
   //  headingPID.SetOutputLimits(-NORMAL_SPEED, NORMAL_SPEED);
   //  headingPID.SetMode(AUTOMATIC);
 
  
 }
 //heading ：车的航向角相对于真北方向
// bearing： 目的地相对于真北方向的方位角
//返回：最优的转弯方案：转弯方向，转弯角度，行驶距离
int HeadingAnalysis(int Heading,int Bearing)
{
 int headingtmp;
 char t_dirction =0;
 if((Heading - Bearing) < 0)
 {
	 headingtmp = Heading + 360;
	 if(headingtmp - Bearing <= 180)
	 {
	 	//左转
	 	printf("turn left %d\n",headingtmp - Bearing);
		t_dirction = 1 ;
	 }else if(headingtmp - Bearing > 180){
	 	printf("turn right %d\n",360 - (headingtmp-Bearing));
		t_dirction = 2 ;
	 }
 }else if((Heading - Bearing) > 0)
 {
 	if(Heading - Bearing < 180)
	 {
		printf("turn left %d\n",Heading - Bearing);
		t_dirction = 1 ;
		
	 }
 	else if(Heading - Bearing > 180)
	 {
	 	printf("turn right %d \n",360 - (Heading-Bearing));
		t_dirction = 2 ;
	 }
 }
 return t_dirction;
 
}
 float prevHeading = 0;
 
/*******************************************************************************
* function name	: SteerToHeading
* description	: heartbeat function ,if receive new data ,clear counter,or,
*				  call heatbeat main func
* param[in] 	: task_table[4]
* param[out] 	: none
* return 		: none
*
* Steer to heading subsumption task.  If active and not subsumed by a higher priority task, 
* this will set the motor speeds
* to steer to the given heading (targetHeading)
 *******************************************************************************/
 void SteerToHeading()
 {
 
 }
 
/*******************************************************************************
* function name	: SteerToHeadingOfGPS
* description	: heartbeat function ,if receive new data ,clear counter,or,
*				  call heatbeat main func
* param[in] 	: task_table[4]
* param[out] 	: none
* return 		: none
*
* Steer to heading subsumption task.  If active and not subsumed by a higher priority task, 
* this will set the motor speeds
* to steer to the given heading (targetHeading)
 *******************************************************************************/
 void SteerToHeadingOfGPS()
 {
     // Filter the mag data to eliminate noise
      int  filteredHeading = 0;//heading;
      int tmpgpsheading = (int)gpsheading;
	  int tmpgpsvelocity = (int)gpsvelocity;
	  int tmp_currentheading =0;
	  int direction =0;
	  if((gpsvelocity > 1.0)&&(tmpgpsheading != 0))//m/s
	  
 			tmp_currentheading  = tmpgpsheading;
      else {
			for(int i =0;i<3;i++)
				//filteredHeading += (int)heading;
			tmp_currentheading = filteredHeading /3;
	  }
     direction =  (tmp_currentheading- (int)targetHeading);
	  if(abs(direction) >=20)
	  	{
	  	  if(HeadingAnalysis(tmp_currentheading, (int)targetHeading) == 1)
	  	  cmd_send(5,0.20);//left 
		 else  
	  	  cmd_send(5,-0.20);
	  	}
	  else if(abs(direction) >=10)
	  	{
	  	  if(HeadingAnalysis(tmp_currentheading, (int)targetHeading) == 1)
	  	  cmd_send(5,0.10);
		 else  
	  	  cmd_send(5,-0.10);
	  	}   
	else if(abs(direction) < 10)
	  	{
	  	  
	  	  cmd_send(5,0);
	  	} 
 
  //   steerToHeadingControl->leftMotorPower = NORMAL_SPEED - headingPIDOutput;
   //  steerToHeadingControl->rightMotorPower = NORMAL_SPEED + headingPIDOutput;
 
  //   steerToHeadingControl->active = steerToHeadingMode;
 }
 /*******************************************************************************
 * function name : DetectObstacles
 * description   : heartbeat function ,if receive new data ,clear counter,or,
 *                 call heatbeat main func
 * param[in]     : task_table[4]
 * param[out]    : none
 * return        : none
 *
 * Detect obstacles subsumption task.  If an obstacle is detected, set active flag to subsume all other tasks.  This
 * will generally be the highest priority task (except for manual control), since we always want to avoid obstacles
 * regardless of what other tasks are active.
 * return degree 
 *******************************************************************************/

 int  DetectObstacles()
 {
     char ret = -1;
     // Need to set the servo to LEFT, CENTER, or RIGHT, then wait a few hundres ms for it to get there, then grab the
     // distance reading.  Reading the distance while the servo is moving will generate too much noise.
     //
     // ...
     //
     // This doesn't do anything currently
     // TODO:  Do something useful here
     //
      int distanceAhead = 0;//global_dis;
     if (distanceAhead > 4 && distanceAhead < 40 ) // cm
     {
       ret = true ;   //  detectObstaclesControl->active = true;
     }
      else
          ret = false;  //detectObstaclesControl->active = false;

      //如果存在障碍物，返回避障动作,及角度值
 }
  /*******************************************************************************
 * function name : CalculateHeadingToWaypoint
 * description   : 计算当前距离到航点的角度
 *                 call heatbeat main func
 * param[in]     : task_table[4]
 * param[out]    : none
 * return        : none
 *
 *******************************************************************************/
 void CalculateHeadingToWaypoint()
 {
     GeoCoordinate current(latitude, longitude);
     GeoCoordinate waypoint = waypoints[currentWaypoint];
 
     // getBearing() expects its waypoint coordinates in radians
     waypoint.latitude = waypoint.latitude * PI / 180.0;
     waypoint.longitude = waypoint.longitude * PI / 180.0;
 
     // targetHeading is the value used by the heading PID controller.  By changing this, we change the heading
     // to which the SteerToHeading subsumption task will try to steer us.
     targetHeading = getBearing(current, waypoint);
     if (targetHeading < -180)
          targetHeading += 360;
     if (targetHeading > 180)
        targetHeading -=360;
     
     DEBUG(LOG_DEBUG,"---->calculat two points degress is %d \n",( int)targetHeading);
   
     return;
 }
   /*******************************************************************************
 * function name : CalculateDistanceToWaypoint
 * description   : caclulate  到航向点的距离distance
 *                 call heatbeat main func
 * param[in]     : task_table[4]
 * param[out]    : none
 * return        : none
 *
 *******************************************************************************/
 void CalculateDistanceToWaypoint()
 {
     GeoCoordinate current(latitude, longitude);
     GeoCoordinate waypoint = waypoints[currentWaypoint];
 
     // getDistance() expects its waypoint coordinates in radians
     waypoint.latitude = waypoint.latitude * PI / 180.0;
     waypoint.longitude = waypoint.longitude * PI / 180.0;

 
     // targetHeading is the value used by the heading PID controller.  By changing this, we change the heading
     // to which the SteerToHeading subsumption task will try to steer us.
     double waypointRangetmp = getDistance(current, waypoint);
     waypointRange = waypointRangetmp*1000;
       DEBUG(LOG_DEBUG,"--->caculate two points distance is %.1f m\n",waypointRange); 
     if (waypointRange < 3.0) // 3.0 meters
     {
           /// currentWaypoint ++;
            DEBUG(LOG_DEBUG,"current points in range <3 m,please read next point\n"); 
     }
      if ((currentWaypoint+1) > waypointCount)
         {
             DEBUG(LOG_DEBUG,"point read over ,it is  the last+1 \n"); 
         }  
     if (currentWaypoint >= waypointCount)
     currentWaypoint = 0;
   
     return;
 }
 

 static void *rotateDegreesThread1(void *threadParam)
 {
     // Make sure there's only one rotate thread running at a time.
     // TODO: proper thread synchronization would be better here
   
 
 }
 



 /*******************************************************************************
 * function name : RotateDegrees
 * description   : caclulate  rotate Degrees  N DEGREE 
 *                 call heatbeat main func
 * param[in]     : task_table[4]
 * param[out]    : none
 * return        : none
 *
 *******************************************************************************/
  void RotateDegrees(int degrees)
 {
    
   
     // degrees 两点之间的而角度
     DEBUG(LOG_DEBUG,"rotateDegreesThread  start ****************\n");
     //  百度地图的坐标系是正北为0 正东为90度，直角坐标系是右手为0，正北为正90，方向与百度地图坐标系相反
     //仿真根据 
     int startHeading =  (int)car_heading_simulation ;// /这里获得是真北方向角
     
     DEBUG(LOG_DEBUG,"startHeading:%d  \n",startHeading);
     int target_degrees  = degrees - startHeading   ;//得到最终需要旋转的角度，这里一定要注意是目标角度-车辆航向角
      if (target_degrees > 180)
     target_degrees -= 360;
     else if (target_degrees < -180)
     target_degrees += 360;
     //得到最终需要旋转的角度target_degrees必须在+-0-180之间，正值和负值代表旋转方向
     char  done = 0;
     int heading_sum =0;
	 int waitquit =0;
	 DEBUG(LOG_DEBUG,"need turn:%d ,start heading:%d \n",target_degrees,startHeading);
  do{
	     if (target_degrees < 0)//为了迎合百度地图坐标系。小于0时，左转
	     {
	  
	        cmd_send_vstep_simulation(0.0,-0.05);
	     }
	     else//为了迎合百度地图坐标系。大于0时，右转
	     {

	         cmd_send_vstep_simulation(0.0,0.051);
	     }
	     usleep(500000);//500ms 一轮
         if(waitquit ++> (2*60))//1分中
	       {
                 
                cmd_send_vstep_simulation(0.0,0.0);
                usleep(500000);
                DEBUG(LOG_ERR,"1min time  over \n");
                break;
		   }
   
     // Backup method - use the magnetometer to see what direction we're facing.  Stop turning when we reach the target heading.
	     int currentHeading  = int(car_heading_simulation);//headingFilter.GetValue();
	     int error_degree = currentHeading - startHeading;
		 heading_sum += error_degree;
		 int currentHeading_sec = heading_sum ;
         if(waitquit%2 ==0)
	        DEBUG(LOG_DEBUG," Rotating: sum =%d,target degree = %d\n", heading_sum, target_degrees);
	     if (abs(currentHeading_sec - target_degrees) <= 10)
	     {
             DEBUG(LOG_DEBUG," Rotating sum degree:%d,target degree = %d\n", heading_sum, target_degrees);
	         done = 1;
	     }
	  //   if (currentHeading < startHeading && degrees > 0)
	    //     startHeading = currentHeading;
	    // if (currentHeading > startHeading && degrees < 0)
	      //   startHeading = currentHeading;
 
     //sleep(1);//不要太长否则容易转过头 
     }
     while (!done);
    cmd_send_vstep_simulation(0,0);
         if(waitquit > 120)
         GLOBAL_STATUS = ROTATE_STATUS ;
         else
                GLOBAL_STATUS = MOVE_STATUS ;
	DEBUG(LOG_DEBUG,"rotate complete exit \n");
      
   



 }
  void RotateDegreesByManual(int degrees)
 {
     pthread_t rotThreadId;
     static bool threadActive = false;
 

     int *rotationDegrees = (int *)malloc(sizeof(int));
     *rotationDegrees = degrees;

     pthread_create(&rotThreadId, NULL, rotateDegreesThread1, rotationDegrees);
 }


 /*******************************************************************************
 * function name : MoveDistance
 * description   : caclulate    distance  N meters 
 *                 call heatbeat main func
 * param[in]     : task_table[4]
 * param[out]    : none
 * return        : none
 *
 *******************************************************************************/
  void MoveDistance(int meters)
 {
    
       

    
     DEBUG(LOG_DEBUG,"moveDistanceThread  start ****************\n");
    
     int target_dis =    meters;
    char ret =0;
     char  done = 0;
     float  sum_run_dis =0;
      int waitquit =0;
	 DEBUG(LOG_DEBUG,"meters:%d \n",meters);
  do{
  	 
	   
    // obstacleAvoidance();
     cmd_send_vstep_simulation(car_speed_from_web,0.0);

     // Backup method - use the magnetometer to see what direction we're facing.  Stop turning when we reach the target heading.
     sum_run_dis +=  car_run_simulation;
    // DEBUG(LOG_DEBUG,"MOve: currentPosition = %d   targetPosition = %d\n", currentPosition, targetPosition);
     if (abs(sum_run_dis - target_dis) < 3)
     {
         done = 1;
     }
      
    ret =  isInRange(3, latitude , longitude, waypoints[currentWaypoint].latitude, waypoints[currentWaypoint].longitude);
    if (ret == 1) //点在圆圈内
    {   
        DEBUG(LOG_DEBUG,"arrive into circle scale ,will read next point  \n");
            GLOBAL_STATUS = WAYPOINTARRIVE_STATUS ;
            done = 1;
    }
     usleep(500000);
      if(waitquit ++> (30))//持续行驶太久导致角度太大
        {
                
            cmd_send_vstep_simulation(0.0,0.0);
            usleep(500000);
            DEBUG(LOG_ERR,"1min time  over \n");
            break;
        }
        if(waitquit%2 ==0)
        DEBUG(LOG_DEBUG," distance sum:%f\n", sum_run_dis);
     }
     while ((!done)&&(GLOBAL_STATUS == MOVE_STATUS));
      cmd_send_vstep_simulation(0.0,0.0);
	
      
	DEBUG(LOG_DEBUG,"move distance  thread exit \n");
 
  
	
 }
  /*******************************************************************************
   * function name : MoveDistanceDwa
   * description   : caclulate	  distance	N meters 
   *				 call heatbeat main func
   * param[in]	   : task_table[4]
   * param[out]    : none
   * return 	   : none
   *
   *******************************************************************************/
	void MoveDistanceDwa(int meters)
   {
	  
	   pthread_t rotThreadId;
	   static bool threadActive = false;
	   
	   
	   
		 threadActive = true;
		 
	   int *MoviMeters = (int *)malloc(sizeof(int));
	   *MoviMeters = meters;
	//   if(pthread_create(&rotThreadId, NULL, moveDistanceDwaThread,MoviMeters))
	  {
			  perror( "pthread_create error "); 
			  DEBUG(LOG_ERR,"MOVE THREAD CREATE ERROR \n");
	  }
	  
   }

/*******************************************************************************
* function name	: loop_handle
* description	: heartbeat function ,if receive new data ,clear counter,or,
*				  call heatbeat main func
* param[in] 	: task_table[4]
* param[out] 	: none
* return 		: none
*******************************************************************************/
void *navimanage_handle (void *arg)
{

	char ret = -1;

   
    unsigned long lastDISMillis = 0;
    unsigned long lastSubMillis = 0;
    unsigned long lastGPSMillis = 0;
    unsigned long motorsOffMillis = 0;
    bool motorsRunning = false;
 //   ReadWaypointsFile();
    DEBUG(LOG_DEBUG,"navimanage_handle   \n");
    
    char onceread =0;
    while (1)
    {
    	unsigned long loopTime = getmillis();
        
		while(GLOBAL_SWITCH)
		{   
	  	    if(onceread ==0)
	       	{
		        onceread =1;
	            if(is_file_exist("waypoints.csv")!=0)//判断航点的文件是否存在
                {
                    GLOBAL_STATUS=STOP_STATUS;
                    GLOBAL_SWITCH =0 ;
                    onceread =0;
                    DEBUG(LOG_ERR,"waypoint file is not exist \n");
                    break;	
                }
		    }
		    if((latitude ==0.0)&&(longitude == 0.0))//是否定位成功
		    {
			        DEBUG(LOG_ERR,"GPS CANNOT LOCATION PLEASE CHECK \n");
	                GLOBAL_STATUS = STOP_STATUS;
	                GLOBAL_SWITCH = 0;
	                break;

		    }
	        if ((getmillis() - lastSubMillis > SUBSUMPTION_INTERVAL))//定时处理
            {
                switch(GLOBAL_STATUS)
                {
                case STANDBY_STATUS://启动后的初始状态
                    DEBUG(LOG_DEBUG,"STANDBY STATUS \n");
                    lastGPSMillis =0 ;
                    ReadWaypointsFile();//读取航点到队列
                    CalculateHeadingToWaypoint();//计算当前角度和航点的方向
                    CalculateDistanceToWaypoint();//计算当前和航点的距离
                    GLOBAL_STATUS = ROTATE_STATUS ;
                break;
                case CACULATE_STATUS://计算状态
                    DEBUG(LOG_DEBUG,"CACULATE STATUS \n");
                    CalculateHeadingToWaypoint();
                    CalculateDistanceToWaypoint();
                    GLOBAL_STATUS = ROTATE_STATUS ;
                break;
                case ROTATE_STATUS ://旋转状态
                    DEBUG(LOG_DEBUG,"ROTATE_STATUS   \n");
                    if (abs((int)car_heading_simulation - (int)targetHeading) > 10)
                    {
                        
                        RotateDegrees(targetHeading);//这里需要根据求出的角度进行转动，需要旋转的角度位 当前航向角-两点之间的角度，需要做-180~180之间的归一化
                    
                    }else {
                            GLOBAL_STATUS = MOVE_STATUS ;
                    }
                
                break;
                case MOVE_STATUS ://前进状态  
                    DEBUG(LOG_DEBUG,"MOVE_STATUS   \n");
                    if((waypointRange > 200))//大于100m 认为不合法 所以规划路径时需要注意
                    {	
                        DEBUG(LOG_ERR,"distance > 200m \n");
                         break;
                    }	
                    MoveDistance(waypointRange);
                
                    break;    
                
                case WAYPOINTARRIVE_STATUS://读取下一个航点
                    DEBUG(LOG_DEBUG,"WAYPOINTARRIVE_STATUS,currentWaypoint:%d,waypointCount:%d\n",currentWaypoint,waypointCount);
                    if(currentWaypoint < waypointCount-1 )
                    {
                        currentWaypoint ++;
                        DEBUG(LOG_DEBUG,"currentWaypoint ++\n");
                        GLOBAL_STATUS = CACULATE_STATUS ;
                    }
                    else  if(currentWaypoint >= waypointCount-1 ){
                        GLOBAL_STATUS = STOP_STATUS ;
                        DEBUG(LOG_ERR,"currentWaypoint >= waypointCount stop status\n");
                    }
                break;
                case STOP_STATUS :
                
                break;
                case MANUAL_STATUS :
                break;
                default :
                break;

                }//end switch
                // 必须先运行一次 standby 状态 只有当在目的地附近3米内才会转换状态
                ret =  isInRange(3, latitude , longitude, waypoints[currentWaypoint].latitude, waypoints[currentWaypoint].longitude);
                if (ret == 1) //点在圆圈内
                {   
                    DEBUG(LOG_DEBUG,"arrive into circle scale ,will read next point  \n");
                    GLOBAL_STATUS = WAYPOINTARRIVE_STATUS ;
                }
                waypointlongitude =  waypoints[currentWaypoint].longitude;
                waypointlatitude = waypoints[currentWaypoint].latitude;
                CalculateHeadingToWaypoint();//计算两点之间的角度
                int b_heading = abs((int)car_heading_simulation - (int)targetHeading);//获取当前航向和两点之间的差值
                DEBUG(LOG_DEBUG,"caculate b-heading:%d ,ret=%d \n",b_heading,ret);
                if ((b_heading > 10 )&&(ret != 1)  )
                {
                    GLOBAL_STATUS = ROTATE_STATUS ;
                }
                lastSubMillis = getmillis();
            }//end  sub loop
	          
	    }//end while switch on
     	
     

        unsigned long now = getmillis();
    	if (now - loopTime < 1)
            usleep((1 - (now - loopTime)) * 1000);
    }//end while1

}

