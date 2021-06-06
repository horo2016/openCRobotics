#include "move2pose.h"
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
//#include <opencv2/opencv.hpp>

using namespace std;
//using namespace cv;
//app
#include "imu.h"
#include "odometry.h"
#include "stm32_control.h"
float Kp_rho = 0.5;;
float Kp_alpha = 1.5;
float Kp_beta = -1.1;
float deta = 0.01;
float v,w=0;
float minwa = 0.4;
float magxwa = 0.4;




//pose 
vector<Pose> traj_pose;

//增量式PID
float Incremental_PI (float Encoder,float Target)
{ 	
	 
   float Kp=0.023,Ki=0.03;	
	 static float Bias=0,Pwm=0,Last_bias=0,pwm2=0;
	
     
	 Bias =  Encoder - Target;                //¼ÆËãÆ«²î
	 printf("bias:%f \n",(Bias*180/3.14));
	 if(fabs(Bias*180/3.14) < 10)
	 {
	 	 	Bias=0;Pwm=0;Last_bias=0;pwm2=0;
			return 0;
	 }
	 Pwm =Kp*(Bias-Last_bias)+Ki*Bias;   //ÔöÁ¿Ê½PI¿ØÖÆÆ÷
	 Pwm = Pwm + pwm2;
	 pwm2 = Pwm;
	 Last_bias=Bias;	                   //±£´æÉÏÒ»´ÎÆ«²î 
	 return Pwm;                         //ÔöÁ¿Êä³ö
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
int move2pose(float x_start,float y_start,float theta_start,float x_goal,float y_goal,float theta_goal)
{
	float x = x_start;
    float y = y_start;
    float theta = theta_start;

    float x_diff = x_goal - x;
    float y_diff = y_goal - y;

     
	vector<int> x_traj;	
	vector<int> y_traj;
	float dealtx = pow(x_diff,2);
	float dealty = pow(y_diff,2);
	float rho = sqrt(dealtx + dealty);
     
    float dis = rho;
	//如果位置一样 但航向角存在差别 返回-1
	if(dis  == 0)
		return -1;
	//条件为0.1 单位m
	while(rho > 0.1)
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
        float alpha = (atan2((double)y_diff, (double)x_diff) - theta )  ;
		float v_error =  alpha / M_PI;
		double Px = v_error,logPx=0 ;
		
		
		
		printf("logPx :%.5f \n",1/1+(Px));
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
		
		v = 0.01;//Kp_rho * rho;
      //  w = Kp_alpha * alpha + Kp_beta * beta ;
	//	printf("raw w:%.5f \n",w);
		if(w < -0.0)
			w = -minwa;
		if(w > 0)
			w = minwa;
	
		float ww =Incremental_PI(theta,theta_goal);
		printf(" pid :%f\n",ww);
		if(ww < -1.0)
			ww = -minwa;
		if(ww> 1)
			ww = minwa;
		cmd_send2(v, ww);
		printf("$$$$$$$$$$$$current robot status$$$$$$$$$$$$$$ \n");
		printf("alpha:%1f theta:%1f\n",alpha*180/3.14,theta*180/3.14);
		printf("goal:%f dis:%1f\n",theta_goal*180/3.14,rho);
        printf("%.2f,%.2f,v:%.5f,w:%.5f \n",alpha*180/3.14,beta*180/3.14,v,w);
		printf("from IMUDevice Heading:%f\n",heading);
		//if alpha > np.pi / 2 or alpha < -np.pi / 2:
        //v = -v
	//下面的参数用车体的代替
         theta =    heading*M_PI/180;//theta + w * deta;
         x = x +  position_x;//��ʼλ��x������// v *  cos(theta) * deta;
         y = y + position_y;   //v *  sin(theta) * deta;
		 usleep(100000);
	}
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
    ifstream inFile("./square.csv", ios::in);
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
		//int inv_angle = angle +30 ;//angle for rear sonar sensor
		///int left_angle = angle -30 ;
		string x0 = Trim(lineArray[0]);
		string y0 = Trim(lineArray[1]);
		int lng = atoi(x0.c_str());
		 
		int lat = atoi(y0.c_str());
		tmpPose.x = lng ;
		tmpPose.y = lat ;
		tmpPose.yaw = angle ;
		traj_pose.push_back(tmpPose);
	}
}



int moveFollow()
{
	
	float x_start = 0 ;
	float y_start = 0 ;
	float theta_start = heading*3.1415/180;
	float x_goal  =3 ;
	float y_goal = 10 ;
	float theta_goal = 10*3.1415/180;
	printf("Initial x: %d m\nInitial y: %d m\nInitial theta: %.2f rad\n" ,
		  x_start, y_start, theta_start);
	printf("Goal x: %d m\nGoal y: %d m\nGoal theta: %.2f rad\n" ,
		  x_goal, y_goal, theta_goal);
	int ret =0 ;
	readTraj();
	if(traj_pose.empty())
		return 0;
	//wait imu device online 
	sleep(2);
	vector<Pose>::iterator it = traj_pose.begin();
	
	for(; it != traj_pose.end(); ++it)
	{
		cout<<" waypoint trajactory start" <<endl;
		cout<<(*it).x<<","<<(*it).y<<","<<(*it).yaw<<" ";
		
		x_goal = (*it).x;
		y_goal = (*it).y;
		theta_goal = (*it).yaw *3.1415/180;
		ret = move2pose(x_start, y_start, theta_start, x_goal, y_goal, theta_goal);
		//返回问题状态
		if(ret == -1)
		{
			theta_start = theta_start;
		}else {
		
			theta_start = theta_goal;
		}
		x_start = x_goal;
		y_start = y_goal;
	}
	
	/*while (1)
		{
		 printf("from IMU Device: %f \n",heading);
		 usleep(5000);
		}*/
}
