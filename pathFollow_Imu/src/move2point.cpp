/*
创建一张300*300像素的上个地图，每个栅格10cm ，所以共30m*30m
square。csv为路径航点信息，每50cm一个航点，

*/


#include "move2point.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <vector> 
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stack>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
//app
#include "imu.h"
#include "odometry.h"
#include "stm32_control.h"


#define Map_Size 500

float Kp_rho = 0.5;;
float Kp_alpha = 1.5;
float Kp_beta = -0.3;
float deta = 0.2;// UNIT:S
float v=0.5,w=0;
float minwa = 0.5;
float magxwa = 0.4;


float speedMult[8] = {0.75f, 0.5f, 0.25f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
unsigned char Sonars_logs[Map_Size][Map_Size]={0};


//pose 
vector<Pose> traj_pose;
unsigned long get_micros()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    unsigned long count = tv.tv_sec * 1000000 + tv.tv_usec;
    return count ;
}

//增量式PD
float Incremental_PD (float angle_rad,float target)
{ 	
	 
   float Kp=0.0923,Kd=0.003;	
	 static float Bias=0,Pwm=0,Last_bias=0,pwm2=0;
	
     
	  Bias =  target -angle_rad ;                 
	 if  (Bias < -M_PI)										 
					 Bias = Bias + 2 * M_PI ;
	 else if(Bias > M_PI)
					 Bias = Bias - 2 * M_PI ;                 
	 printf("bias:%f \n",(Bias*180/3.14));
	 //+- 10度角的范围内
	 if(fabs(Bias*180/3.14) < 10)
	 {
	 	 	Bias=0;Pwm=0;Last_bias=0;pwm2=0;
			return 0;
	 }
	 Pwm =Kd*(Bias-Last_bias)+Kp*Bias;      
	 Pwm = Pwm + pwm2;
	 pwm2 = Pwm;
	 Last_bias=Bias;	                    
	 return Pwm;                         
}
//pi调节
float Incremental_PI (float angle_rad,float target)
{ 	
	 
   float Kp=0.023,Ki=0.03;	
	 static float Bias=0,Pwm=0,Total_bias=0,pwm2=0;
	
     
	  Bias =  target -angle_rad ;                 
	 if  (Bias < -M_PI)										 
					 Bias = Bias + 2 * M_PI ;
	 else if(Bias > M_PI)
					 Bias = Bias - 2 * M_PI ;                 
	 printf("bias:%f \n",(Bias*180/3.14));
	 //+- 10度角的范围内
	 if(fabs(Bias*180/3.14) < 10)
	 {
	 	 	Bias=0;Pwm=0;Total_bias=0;pwm2=0;
			return 0;
	 }
	 Pwm =Ki*Total_bias +Kp*Bias;   //   
	 Pwm = Pwm + pwm2;
	 pwm2 = Pwm;
	 Total_bias +=Bias;	                   // 
	 return Pwm;                         // 
}
float Incremental_P (float angle_rad,float target)
{ 	
	 
   float Kp=0.083,Ki=0.03;	
	 static float Bias=0,Pwm=0,Total_bias=0,pwm2=0;
	
     
	 Bias =  target -angle_rad ;                // 
	 if  (Bias < -M_PI)										 
					 Bias = Bias + 2 * M_PI ;
	 else if(Bias > M_PI)
					 Bias = Bias - 2 * M_PI ;

	 //+- 10度角的范围内
	 if(fabs(Bias*180/3.14) < 10)
	 {
	 	 	Bias=0;Pwm=0;Total_bias=0;pwm2=0;
			return 0;
	 }
	 Pwm = Kp*Bias;   //   
	// Pwm = Pwm + pwm2;
	// pwm2 = Pwm;
	 	               
	 return Pwm;                         // 
}
int  	paint_path(Mat m,int x ,int y,int n)
{
 static Point p1;
 if(n == 0){
 	  p1.x =x;
		p1.y =y;
   	return 0;
 	}
 Point  p2(x, y); // (x, y) = (w, h)
 Scalar colorLine(0, 255, 0); // Green - (B, G, R)
 int thicknessLine = 2;
 
 line(m, p1, p2, colorLine, thicknessLine);
 p1=p2;
		
}
int  	paint_odom_path(Mat m,int x ,int y,int n)
{
 static Point p1;
 if(n == 0){
 	  p1.x =x;
		p1.y =y;
   	return 0;
 	}
 Point  p2(x, y); // (x, y) = (w, h)
 Scalar colorLine(255, 0, 0); // Green - (B, G, R)
 int thicknessLine = 2;
 
 line(m, p1, p2, colorLine, thicknessLine);
 p1=p2;
		
}

/*
    rho is the distance between the robot and the goal position
    alpha is the angle to the goal relative to the heading of the robot
    beta is the angle between the robot's position and the goal position plus the goal angle
    Kp_rho*rho and Kp_alpha*alpha drive the robot along a line towards the goal
    Kp_beta*beta rotates the line so that it is parallel to the goal angle
	alpha_error = tan(y,x)-theta 
	beta_error = theta_goal-tan(y,x)
	α = tan(dy,dx)- start_theta;
	β = theta_goal - theta - (tan(dy,dx)-theta);
	  = theta_goal - tan(dy,dx);
	  
	dθ = α + β = theta_goal - start_theta ;
	上式中 dθ就是为要旋转的角度。dθ > 0 向右转。
	dθ <0 向左转。
	实际中需要加入 影响比例，该参数需要调试。
	dθ = kα + pβ;k的值决定了向右转的幅度，p决定了向左转的幅度。
	k越大转向角度越大，p同理.太小的话会导致转弯半径太大无法以最短的距离
	行驶 到下一个航点。
*/
Pose move2pose(Mat m,float x_start,float y_start,float theta_start,float x_goal,float y_goal,float theta_goal)
{
	float x = x_start;
  float y = y_start;
  float theta = theta_start;
	Pose tmp_pose;
	tmp_pose.x  = x_start;
	tmp_pose.y  = y_start;
	tmp_pose.yaw= theta_start*180/3.14;

  float x_diff = x_goal - x;
  float y_diff = y_goal - y;

     
	vector<int> x_traj;	
	vector<int> y_traj;
	vector<Pose> xy_traj;
	
	float dealtx = pow(x_diff,2);
	float dealty = pow(y_diff,2);
	float rho = sqrt(dealtx + dealty);
     
   float dis = rho;
	 static int cnt =0;
	//如果位置一样 但航向角存在差别 返回-1
	if(dis  == 0){
		printf("Goal POint has arrived \n");
		return tmp_pose;
		}
	unsigned long lastMillis = get_micros();
	//条件为0.1 单位m
	while(rho > 10)
	{
		if(get_micros() - tinestamp_dead >600000UL)
			continue;
		x_traj.push_back(x);
             y_traj.push_back(y);
		xy_traj.push_back(tmp_pose);
		x_diff = x_goal - x ;
             y_diff = y_goal - y;
		// Restrict alpha and beta (angle differences) to the range
        // [-pi, pi] to prevent unstable behavior e.g. difference going
        // from 0 rad to 2*pi rad with slight turn
		rho = sqrt(pow(x_diff,2) + pow(y_diff,2));
		 //得到航向角和方位角之差
		float orintation = atan2((double)y_diff, (double)x_diff);
	
		printf(">>>>>>>> robot status<<<<<<<<<<<<<<< \n");
		printf("  Now Pose: \n");
		printf("     x:%1f \n",x);
		printf("     y:%1f \n",y);
		printf("    theta:%1f \n",theta*180/3.14);
		printf("  Goal Pose: \n");
		printf("     x:%1f \n",x_goal);
		printf("     y:%1f \n",y_goal);
		//printf("     goal orintation:%1f \n",orintation*180/3.14);
		
		
	
		
    float alpha = (orintation - theta )  ;
		
		if  (alpha < -M_PI)                     
            alpha = alpha + 2 * M_PI ;
        else if(alpha > M_PI)
            alpha = alpha - 2 * M_PI ;
		
        float beta =  theta_goal - theta - alpha   ;
		if  (beta < -M_PI)                     
            beta = beta + 2 * M_PI ;
        else if(beta > M_PI)
            beta = beta - 2 * M_PI ;
		
        if (rho > dis/2)
            v = Kp_rho * rho;
        else  
            v = 0.5 ;
		
		v = 0.2;//Kp_rho * rho;
        w = Kp_alpha * alpha + Kp_beta * beta ;
		
		if(w < -0.0)
			w = -minwa;
		if(w > 0)
			w = minwa;
	
		printf("Kinectic status: \n");
		printf("    Distance:%1f\n",rho);
		printf("    Goal orintation:%.1f \n",orintation*180/M_PI);
    printf("    alpha:%1f \n",alpha*180/3.14);
		printf("    IMUDevice Heading:%f\n",-heading); 
		printf("    courseHeading:%f\n",-courseHeading); 

		
		int Xrnext =	position_x; 
		int Yrnext = position_y; 
		
		float ww =Incremental_PI(theta,orintation);
		
		if(ww < -1.0)
			ww = -minwa;
		if(ww> 1)
			ww = minwa;

		v = v*speedMult[(int)(abs(alpha*180/M_PI)/22.5f)];
		printf("Control: \n");
		printf("    v:%1f \n",v);
		printf("    w:%1f \n",ww);
		
		cmd_send2(v, ww);
		
	
		//if alpha > np.pi / 2 or alpha < -np.pi / 2:
        //v = -v
	//下面的参数用车体的代替
	   deta =  (float)(get_micros() - lastMillis )/1000000  ;
		lastMillis = get_micros();
		
     theta += angspeed   * deta;// (-courseHeading)*M_PI/180;//theta + w * deta;

     x +=    velspeed *  cos(theta) * deta *100; //position_x;// //
     y +=    velspeed *  sin(theta) * deta *100; //position_y;   //
     printf("odom moveing: \n");
		 printf("    deta:%1f \n",deta);
		 printf("    velspeed:%1f \n",velspeed);
		 printf("    angspeed:%1f \n",angspeed);
     paint_odom_path(m,x,500-y,cnt);
		 cnt ++;
		 if(cnt >= 30){
		   	imwrite("Map_traj.png",m);
				cnt = 1;
		 }
		 tmp_pose.x 	 = x;
	   tmp_pose.y 	 = y;
	   tmp_pose.yaw = theta*180/3.14;
		 usleep(100000);
	}
	tmp_pose.x 	 = x;
	tmp_pose.y 	 = y;
	tmp_pose.yaw = theta*180/3.14;
	return tmp_pose;
}
/*
利用pure puresuit方法跟踪航点，起始点的位姿，目标点的位姿，需要求出距离和方位角
*/
Pose pure_puresuit(Mat m,float x_start,float y_start,float theta_start,float x_goal,float y_goal,float theta_goal)
{
	float x = x_start;
  float y = y_start;
  float theta = theta_start;
	Pose tmp_pose;
	tmp_pose.x  = x_start;
	tmp_pose.y  = y_start;
	tmp_pose.yaw= theta_start*180/3.14;

  float x_diff = x_goal - x;
  float y_diff = y_goal - y;

     
	vector<int> x_traj;	
	vector<int> y_traj;
	
	float dealtx = pow(x_diff,2);
	float dealty = pow(y_diff,2);
	float rho = sqrt(dealtx + dealty);
     
   float dis = rho;
	 static int cnt =0;
	//如果位置一样 但航向角存在差别 返回-1
	if(dis  == 0){
		printf("Goal POint has arrived \n");
		return tmp_pose;
		}
	unsigned long lastMillis = get_micros();
	//条件为0.1 单位m
	while(rho > 10)
	{
		x_traj.push_back(x);
    y_traj.push_back(y);
		x_diff = x_goal - x ;
    y_diff = y_goal - y;
		// Restrict alpha and beta (angle differences) to the range
        // [-pi, pi] to prevent unstable behavior e.g. difference going
        // from 0 rad to 2*pi rad with slight turn
		rho = sqrt(pow(x_diff,2) + pow(y_diff,2));
		 //得到航向角和方位角之差
		float orintation = atan2((double)y_diff, (double)x_diff);
		printf(">>>>>>>> robot status<<<<<<<<<<<<<<< \n");
		printf("  Now Pose: \n");
		printf("     x:%1f \n",x);
		printf("     y:%1f \n",y);
		printf("    theta:%1f \n",theta*180/3.14);
		printf("  Goal Pose: \n");
		printf("     x:%1f \n",x_goal);
		printf("     y:%1f \n",y_goal);
		//printf("     goal orintation:%1f \n",orintation*180/3.14);
		
		
	
		
    float alpha = (orintation - theta )  ;
		if  (alpha < -M_PI)                     
            alpha = alpha + 2 * M_PI ;
        else if(alpha > M_PI)
            alpha = alpha - 2 * M_PI ;
		
    float beta =  theta_goal - theta - alpha   ;
		if  (beta < -M_PI)                     
            beta = beta + 2 * M_PI ;
        else if(beta > M_PI)
            beta = beta - 2 * M_PI ;
		
    /*w = Kp_alpha * alpha + Kp_beta * beta ;
		if(w < -0.0)
			w = -minwa;
		if(w > 0)
			w = minwa;
		*/
	
		printf("Kinectic status: \n");
		printf("    Distance:%1f\n",rho);
		printf("    Goal orintation:%.1f \n",orintation*180/M_PI);
    printf("    alpha:%1f \n",alpha*180/3.14);
		printf("    IMUDevice Heading:%f\n",-heading); 
		printf("    courseHeading:%f\n",-courseHeading); 

		
		int Xrnext =	position_x; 
		int Yrnext = position_y; 
		v = v*speedMult[(int)(abs(alpha*180/M_PI)/22.5f)];	
		// 方法一：使用比例PId算法，纠正偏离角度。
		if(abs(alpha*180/M_PI) > 90)
		   w =Incremental_P(theta,orintation);
		//方法2:使用pure_puresuit 算法，根据匀速计算角速度w 。
		else w = 2*v*sin(alpha)*100/ rho ;
		if(w < -1.0)
			w = -minwa;
		if(w> 1)
			w = minwa;
		
		
   
		printf("Control: \n");
		printf("    v:%1f \n",v);
		printf("    w:%1f \n",w);		
		cmd_send2(v, w);
	//下面的参数用车体的代替
	   deta =  (float)(get_micros() - lastMillis )/1000000  ;
		lastMillis = get_micros();
		
     theta += angspeed   * deta;// (-courseHeading)*M_PI/180;//theta + w * deta;
     if  (theta < -M_PI)                     
            theta = theta + 2 * M_PI ;
     else if(theta > M_PI)
            theta = theta - 2 * M_PI ;
     x +=    velspeed *  cos(theta) * deta *100; //position_x;// //
     y +=    velspeed *  sin(theta) * deta *100; //position_y;   //
     printf("odom moveing: \n");
		 printf("    deta:%1f \n",deta);
		 printf("    velspeed:%1f \n",velspeed);
		 printf("    angspeed:%1f \n",angspeed);
     paint_odom_path(m,x,500-y,cnt);
		 cnt ++;
		 usleep(200000);
	}
	tmp_pose.x 	 = x;
	tmp_pose.y 	 = y;
	tmp_pose.yaw = theta*180/3.14;
	return tmp_pose;
}

string Trim(string& str)
{
 str.erase(0,str.find_first_not_of(" \t\r\n"));

 str.erase(str.find_last_not_of(" \t\r\n") + 1);

 return str;
}
int readTraj()
{// 读文件
	//char *file = "./square.csv";
	printf("sss \n");
  ifstream inFile("waypoint.csv", ios::in);
	if (!inFile)
	{
	   cout << "文件不能打开" <<endl;
		 return -1;
	}
	cout << "File can open " <<endl;
	string lineStr;
	
	int linenumber =0;
	Pose tmpPose;
	while (getline(inFile, lineStr))
	{
		// 打印整行字符串
		//	cout << lineStr << endl;
		// 存成二维表结构
		linenumber ++;
		if(linenumber == 1)
			continue;
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		
		// 按照逗号分隔 分别提取 x ,y ,heading
		while (getline(ss, str, ',')){
			lineArray.push_back(str);
		}
		
		string angle_str =  Trim(lineArray[2]) ;//Robot heading angle
		int angle = atoi(angle_str.c_str());
		string x0 = Trim(lineArray[0]);
		string y0 = Trim(lineArray[1]);
		int lng = atoi(x0.c_str());
		cout << x0 << y0 << endl;
		int lat = atoi(y0.c_str());
		tmpPose.x = lng ;
		tmpPose.y = lat ;
		tmpPose.yaw = angle ;
		traj_pose.push_back(tmpPose);
	}
}



int moveFollow()
{

	//wait imu device online 
	sleep(2);
	float x_start = 250 ;
	float y_start = 250 ;
	//imu read inverse different tan imu读取的航向角和直角坐标系相反
	float theta_start = (-courseHeading)*3.1415/180;
	int x_goal  =3 ;
	int y_goal = 10 ;
	float theta_goal = 10*3.1415/180;
	printf("Initial x: %d m\nInitial y: %d m\nInitial theta: %.2f rad\n" ,
		  x_start, y_start, theta_start);
	printf("Goal x: %d m\nGoal y: %d m\nGoal theta: %.2f rad\n" ,
		  x_goal, y_goal, theta_goal);
	Pose ret ;
	readTraj();
	if(traj_pose.empty())
		{
		  printf("Error:csv is empty \n");
		  return 0;
		}else {
		  printf("csv size is :%d\n",traj_pose.size());

		}
	
  
	Mat path_map(Map_Size, Map_Size, CV_8UC3, cv::Scalar(255,255,255));
	
	printf("path_map\n");
	theta_start =90*3.1415/180;//-courseHeading*3.1415/180;
	vector<Pose>::iterator iter = traj_pose.begin();
	int n=0;
	for(; iter != traj_pose.end(); ++iter)
	{
		
		cout<<(*iter).x<<","<<(*iter).y<<","<<(*iter).yaw<<" ";
		paint_path(path_map,(*iter).x,500-(*iter).y,n);
		n++;
		
  }
	imwrite("Map_log.png",path_map);
  
	vector<Pose>::iterator it = traj_pose.begin();
	for(; it != traj_pose.end(); ++it)
	{
		cout<<" waypoint trajactory start" <<endl;
		cout<<(*it).x<<","<<(*it).y<<","<<(*it).yaw<<" ";
		
		x_goal = (*it).x;
		y_goal = (*it).y;
		theta_goal = (*it).yaw *3.1415/180;
		ret = move2pose(path_map,x_start, y_start, theta_start, x_goal, y_goal, theta_goal);
		//返回问题状态
		
		imwrite("Map_path.png",path_map);
		x_start 		= ret.x;
		y_start 	  = ret.y;
		theta_start = ret.yaw*3.14/180;
	}
	
	exit(0);
	/*while (1)
		{
		 printf("from IMU Device: %f \n",heading);
		 usleep(5000);
		}*/
}
