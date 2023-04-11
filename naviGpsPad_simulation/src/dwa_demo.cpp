
#include<iostream>
#include<vector>
#include<array>
#include<cmath>
#include <unistd.h>
#if defined WITH_OPENCV
#include<opencv2/opencv.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#endif
#include "utility.h"
#include "dwa.h"
#include "dwa_demo.h"
#include "stm32_control.h"
#include "navi_manage.h"
#include "imu.h"
#if defined  WITH_OPENCV

//20m*20m 200dm*200dm
cv::Point2i cv_offset(    float x, float y, int image_width=200, int image_height=200)
{
    cv::Point2i output;
    output.x = int(x * 10) ;
    output.y = image_height - int(y * 10) ;
    return output;
};

//如果目标里障碍物太近 极容易导致原地画圈
int dwa_loop(float meters){
    State start{10.0, 1.0, 1.6, 0.0, 0.0};//起始坐标设定在x10m,y1m处
    Point goal{10.0,meters};//坐标设定在x10m,y9m处
    if(meters <= 10.0){
   		 goal.x_ = 10.0; 
		 goal.y_ = 1.0 + meters;
		}
    Obstacle ob{
                        
                        {6.0, 4.0},//1g =10cm
                        {6.0, 5.0},
                     //   {2.0, 5.0},
                       // {1.0, 5.0},
                        {14.0, 4.0},
                        {14.0, 6.0},
                        
                };

    Control u{0.0, 0.0};
    Config config;
    Traj traj;
    traj.push_back(start);

    bool terminal = false;

   // cv::namedWindow("dwa", cv::WINDOW_NORMAL);
    int count = 0;
    char once =0;
    clock_t         startclock, stopclock;
    Dwa dwa_demo(start, goal, ob, config);
    static int ccn=0;
    cv::Mat final_canvas;
    Traj ltraj;
    State x;
    Obstacle dyn_ob;
	cv::VideoWriter writer;
	writer.open("out_dwa.avi",CV_FOURCC('M', 'J', 'P', 'G'),
        20, //不进行跟踪，定位，只显示、录制时的帧率
        cv::Size(200,200),
        true);
    if (!writer.isOpened())
    {
        return 0;
    }
    while(!dwa_demo.stepOnceToGoal(&ltraj, &x,&dyn_ob)&&(GLOBAL_STATUS != STOP_STATUS)){
        traj.push_back(x);
         // action to wheel 
	 if(once == 0)
            once =1;
        else if(once == 1)
        {
            stopclock = clock();
            float   elapsedTime = (float)(stopclock - startclock) / (float)CLOCKS_PER_SEC * 1000.0f;
             DEBUG(LOG_DEBUG,"elapsed time:%3.1f ms \n", elapsedTime);
            if(elapsedTime <= (config.dt * 1000)){
                        DEBUG(LOG_DEBUG,"sleep total time:%3.1f ms \n",(config.dt*1000 - elapsedTime-10));
                        usleep((config.dt*1000 - elapsedTime - 1 )*1000);//延迟时间直到底盘执行完毕
                }
        }
         DEBUG(LOG_DEBUG,"1 control tocar v=%.1f,w=%.1f\n", dwa_demo.calculated_u.v_, dwa_demo.calculated_u.w_);
        cmd_send2(dwa_demo.calculated_u.v_, dwa_demo.calculated_u.w_);
        startclock = clock();//
        usleep(100000);//100ms 后采集速度
           //w >0 左转 <0 右转
        dwa_demo.feed_u.v_ =  velspeed;
        dwa_demo.feed_u.w_ =  angspeed;
        DEBUG(LOG_DEBUG,"2 car receive info v=%.1f,w=%.1f\n", dwa_demo.feed_u.v_, dwa_demo.feed_u.w_);
        dwa_demo.cur_x_ = dwa_demo.motion_calculate(dwa_demo.cur_x_, heading,dwa_demo.feed_u, config.dt);

		//avoidance check		
       // if(dwa_demo.update_obstacle(raspi_sonars) == 0)
			//break;
   //error check
        if(dwa_demo.state_error_check(heading) == -1){
			DEBUG(LOG_ERR,"error cannot navigation \n");
			break;
        }
   // visualization
        cv::Mat bg(200,200, CV_8UC3, cv::Scalar(255,255,255));
        cv::circle(bg, cv_offset(goal.x_, goal.y_, bg.cols, bg.rows),
                   3, cv::Scalar(255,0,0), 5);
				   
        for(unsigned int j=0; j<dyn_ob.size(); j++){
            cv::circle(bg, cv_offset(dyn_ob[j].x_, dyn_ob[j].y_, bg.cols, bg.rows),
                       5, cv::Scalar(0,0,0), -1);
        }
        for(unsigned int j=0; j<ltraj.size(); j++){
            cv::circle(bg, cv_offset(ltraj[j].x_, ltraj[j].y_, bg.cols, bg.rows),
                       2, cv::Scalar(0,255,0), -1);
        }
        cv::circle(bg, cv_offset(x.x_, x.y_, bg.cols, bg.rows),
                   3, cv::Scalar(0,0,255), 5);
// toDegree degree = radian/pi*180;
		DEBUG(LOG_DEBUG,"car state:(%.1f,%.1f,%.1f),%.1f \n",x.x_,x.y_,x.theta_,x.theta_/3.14*180);
		DEBUG(LOG_DEBUG, "remain to goal distance is:%.1f \n",std::sqrt(std::pow((x.x_ - goal.x_), 2) + std::pow((x.y_ - goal.y_), 2)));
        cv::arrowedLine(
                bg,
                cv_offset(x.x_, x.y_, bg.cols, bg.rows),
                cv_offset(x.x_ + std::cos(x.theta_), x.y_ + std::sin(x.theta_), bg.cols, bg.rows),
                cv::Scalar(255,0,255),
                2);

		
       // cv::imwrite("dwa.jpg", bg);
         

        // std::string int_count = std::to_string(count);
        // cv::imwrite("/home/pi/dwa/"+int_count+".jpg", bg);
        writer.write(bg);
        count++;
        final_canvas = bg;
    }


    if (!final_canvas.empty()) {
        for(unsigned int j=0; j<traj.size(); j++){
            cv::circle(final_canvas, cv_offset(traj[j].x_, traj[j].y_, final_canvas.cols, final_canvas.rows),
                       2, cv::Scalar(0,0,255), -1);
        }

        cv::imwrite("dwa2.jpg", final_canvas);
        
    }
    writer.release();
	 cmd_send2(0.0,0.0);
    return 1;



}


#endif

