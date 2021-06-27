#include "odometry.h"
#include <math.h>

#include "stm32_control.h"

wheelInf wheelParam;
/***********************************************  输出  *****************************************************************/

float position_x=0;//初始位置x的坐标
float position_y=0;
float oriention=90*M_PI/180;//初始位置的角度
float velocity_linear=0;//y轴上的线速度
float velocity_angular=0;//角速度
float velocity_linear_x=0,velocity_linear_y=0;//x，y轴上的线速度
float velocity_linear_l=0,velocity_linear_r=0;
/***********************************************  输入  *****************************************************************/
 float odometry_right,odometry_left;//串口得到的左右轮速度

/***********************************************  变量  *****************************************************************/

float wheel_interval= 0.268f;//    272.0f;      m  //  1.0146
//float wheel_interval=276.089f;    //轴距校正值=原轴距/0.987

float multiplier=2.0f;           //倍频数
float line_number=2.0f;       //码盘线数  AB 2

float deceleration_ratio=35.0f;  //减速比 170rpm 35
float wheel_diameter= 0.100f;     //轮子直径，单位m
float pi_1_2=1.570796f;          //π/2
float pi=3.141593f;              //π
float pi_3_2=4.712389f;          //π*3/2
float pi_2_1=6.283186f;          //π*2
float dt=0.50f;                 //采样时间间隔500ms

float pusle_cnt =11.0f;
float oriention_interval=0;  //dt时间内方向变化值

float sin_=0;        //角度计算值
float cos_=0;

float delta_distance=0,delta_oriention=0;   //采样时间间隔内运动的距离

float const_frame=0,const_angle=0,distance_sum=0,distance_diff=0;

float oriention_1=0;

char once=1;

/****************************************************************************************************************/

//里程计计算函数 这是可横向平移 竖向平移的计算放法
void odometry(float right,float left)
{   
	odometry_right = right;
	odometry_left = left;
    if(once)  //常数仅计算一次
    {
			//这里跟电机的参数有关系
	//一个计数常量走的距离是多少？
	//电机AB 相是2线，上升沿下降沿都检测所以是2倍频 一圈11个脉冲 一圈的计数为
	//11 *2 *2 个计数。减速比为35 也就是轮子主轴输出 1/35圈此时 44个计数。
	// πd 一圈距离，一个计数的距离是πd*1/35 /44 
        const_frame=wheel_diameter*pi/(line_number*multiplier*deceleration_ratio*pusle_cnt);
        const_angle=const_frame/wheel_interval;
        once=0;
    }

    distance_sum = 0.5f*(odometry_right+odometry_left);//在很短的时间内，小车行驶的路程为两轮速度和
    distance_diff = right-left;//在很短的时间内，小车行驶的角度为两轮速度差

    //根据左右轮的方向，纠正短时间内，小车行驶的路程和角度量的正负
    if((odometry_right>0)&&(odometry_left>0))            //左右均正
    {
        delta_distance = distance_sum;
        delta_oriention = distance_diff;
    }
    else if((odometry_right<0)&&(odometry_left<0))       //左右均负
    {
        delta_distance = -distance_sum;
        delta_oriention = -distance_diff;
    }
    else if((odometry_right<0)&&(odometry_left>0))       //左正右负
    {
        delta_distance = -distance_diff;
        delta_oriention = -2.0f*distance_sum;       
    }
    else if((odometry_right>0)&&(odometry_left<0))       //左负右正
    {
        delta_distance = distance_diff;
        delta_oriention = 2.0f*distance_sum;
    }
    else
    {
        delta_distance=0;
        delta_oriention=0;
    }

    oriention_interval = delta_oriention * const_angle;//采样时间内走的角度  
    oriention = oriention + oriention_interval;//计算出里程计方向角
    oriention_1 = oriention + 0.5f * oriention_interval;//里程计方向角数据位数变化，用于三角函数计算

    sin_ = sin(oriention_1);//计算出采样时间内y坐标
    cos_ = cos(oriention_1);//计算出采样时间内x坐标

    position_x = position_x + delta_distance * cos_ * const_frame;//计算出里程计x坐标
    position_y = position_y + delta_distance * sin_ * const_frame;//计算出里程计y坐标

	velocity_linear_x = delta_distance * cos_ * const_frame / dt;
	velocity_linear_y = delta_distance * sin_ * const_frame / dt;
    velocity_linear = 0.5f*(odometry_right+odometry_left)*const_frame / dt;//计算出里程计线速度
    velocity_angular = distance_diff *const_angle / dt;//计算出里程计角速度
	if(velocity_angular !=0 )
	{
	   int tmp = left;
	}
	velocity_linear_l = left;
	velocity_linear_r = right;
    //方向角角度纠正
    if(oriention > pi)
    {
        oriention -= pi_2_1;
    }
    else
    {
        if(oriention < -pi)
        {
            oriention += pi_2_1;
        }
    }
	printf("*************odom output********************\n");
	printf("linear vel:%3f,ang  vel:%3f,\n",velocity_linear,velocity_angular);
	printf("position_x:%3f,position_x:%3f,\n",position_x,position_y);
}

void odometry_simple(float right,float left)
{   
	  odometry_right = right;
	  odometry_left = left;
    if(once)  //常数仅计算一次
    {
			//这里跟电机的参数有关系
	//一个计数常量走的距离是多少？
	//电机AB 相是2线，上升沿下降沿都检测所以是2倍频 一圈11个脉冲 一圈的计数为
	//11 *2 *2 个计数。减速比为35 也就是轮子主轴输出 1/35圈此时 44个计数。
	// πd 一圈距离，一个计数的距离是πd*1/35 /44 
        const_frame=wheel_diameter*pi/(line_number*multiplier*deceleration_ratio*pusle_cnt);
        const_angle=const_frame/wheel_interval;
        once=0;
    }
    
	//计算出里程计线速度 这里就是vy  不能横向走所以vx =0;
    velocity_linear = 0.5f*(odometry_right+odometry_left)*const_frame / dt;
	//计算出里程计角速度
    velocity_angular = (odometry_right -odometry_left) *const_angle / dt;
	velspeed = velocity_linear;
	angspeed = velocity_angular;
	double delta_x = velocity_linear * cos(oriention)  * dt;
	double delta_y = velocity_linear * sin(oriention)  * dt;
	double delta_th = velocity_angular * dt;
	//计算出里程计x坐标
	position_x = position_x + delta_x;
	//计算出里程计y坐标
    position_y = position_y + delta_y;
	 
	oriention += delta_th;
	if(oriention > pi)
    {
        oriention -= pi_2_1;
    }
    else if(oriention < -pi)
	{
		oriention += pi_2_1;
	}
	printf("odom:\n");
	printf("    vel:%3f\n",velocity_linear);
	printf("    ang:%3f,\n",velocity_angular);
	printf("    pose:\n");
	printf("        position_x:%3f,\n",position_x);
	printf("        position_y:%3f,\n",position_y);
    printf("        heading:%3f\n",oriention*180/3.1415);

    //方向角角度纠正

}

//https://blog.csdn.net/forrest_z/article/details/55001231 
