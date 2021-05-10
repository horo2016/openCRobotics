//
// Created by pang on 2020/6/13.
//

#include "dwa.h"
#include<cmath>
#include<iostream>
#include<vector>
#include<array>
#include<cmath>
#include <limits>

Dwa::Dwa(const State& start, const Point& goal,  const Obstacle& obs, const Config& config):
cur_x_(start), goal_(goal), obs_(obs), config_(config)
{

}

int Dwa::state_error_check()
{
   static float start_theta = cur_x_.theta_ ;
   printf("start theta:%.1f \n",start_theta);
   if((cur_x_.theta_ - start_theta) >= 2*PI)
      return -1;
}

bool Dwa::stepOnceToGoal(std::vector<State>* best_trajectry, State* cur_state,Obstacle *cur_obs) 
{
     
	static int ccn=0;
//    Window dw = calc_dynamic_window(cur_x_, config_);
//    printf("window:%.1f,%.1f,%.1f,%.1f \n",dw.min_v_,dw.max_v_,dw.min_w_,dw.max_w_); 
	if(ccn ++ >= 2)
	{
	 ccn = -9999;
   	 Point ob{cur_x_.x_+ 0.5 * std::cos(cur_x_.theta_ + 0*3.1415/180),cur_x_.y_+ 0.5* std::sin(cur_x_.theta_+0*3.1415/180)};
	     
	 
	 obs_.push_back(ob);
         Point ob1{cur_x_.x_+ 0.5 * std::cos(cur_x_.theta_ + 45*3.1415/180),cur_x_.y_+ 0.5 * std::sin(cur_x_.theta_+45*3.1415/180)};
	              
         obs_.push_back(ob1); 
	printf("add objec in map !!!!!!!\n");
	cur_x_.v_ = 0.05;
	}
     Window dw = calc_dynamic_window(cur_x_, config_);

     Traj ltraj = calc_final_input(cur_x_, calculated_u, dw, config_, goal_, obs_);
	
	 

    printf("control (v,w) (%.1f,%.1f)\n ",calculated_u.v_,calculated_u.w_);
	//w >0 左转 <0 右转
 //   cur_x_ = motion(cur_x_, feed_u, config_.dt);
     cur_x_ = motion(cur_x_, calculated_u, config_.dt);
    //
    *best_trajectry = ltraj;
    *cur_state = cur_x_;
	*cur_obs = obs_;
    if (std::sqrt(std::pow((cur_x_.x_ - goal_.x_), 2) + std::pow((cur_x_.y_ - goal_.y_), 2)) <= config_.robot_radius){
       return true;
    }
    return false;
}


State Dwa::motion(State x, Control u, float dt){
    x.theta_ += u.w_ * dt;
    x.x_ += u.v_ * std::cos(x.theta_) * dt;
    x.y_ += u.v_ * std::sin(x.theta_) * dt;
    x.v_ = u.v_;
    x.w_ = u.w_;
    return x;
};

Window Dwa::calc_dynamic_window(State x, Config config){

   /* return {
            std::max((x.v_ - config.max_accel * config.dt), config.min_speed ) ,
            std::min((x.v_ + config.max_accel * config.dt ), config.max_speed ),
            std::max((x.w_ - config.max_dyawrate * config.dt),config.min_yawrate) -0.1,
            std::min((x.w_ + config.max_dyawrate * config.dt), config.max_yawrate)+0.1
    };
   */
     return {
            std::max((x.v_ - config.max_accel * config.dt), config.min_speed ) ,
            std::min((x.v_ + config.max_accel * config.dt ), config.max_speed ),
             -0.3,
             0.2
    };

//    return {
//            (x.v_ - config.max_accel * config.dt),
//            (x.v_ + config.max_accel * config.dt),
//            (x.w_ - config.max_dyawrate * config.dt),
//            (x.w_ + config.max_dyawrate * config.dt)
//    };
};


Traj Dwa::calc_trajectory(State x, float v, float w, Config config){

    Traj traj;
    traj.push_back(x);
    float time = 0.0;

    while (time <= config.predict_time){
        x = motion(x, Control{v, w}, config.dt);
        traj.push_back(x);
        time += config.dt;
    }
    return traj;
};


float Dwa::calc_obstacle_cost(Traj traj, Obstacle ob, Config config){
    // calc obstacle cost inf: collistion, 0:free
    int skip_n = 2;
    float minr = std::numeric_limits<float>::max();

    for (unsigned int ii=0; ii<traj.size(); ii+=skip_n){
        for (unsigned int i=0; i< ob.size(); i++){
            float ox = ob[i].x_;
            float oy = ob[i].y_;
            float dx = traj[ii].x_ - ox;
            float dy = traj[ii].y_ - oy;

            float r = std::sqrt(dx*dx + dy*dy);
            if (r <= 0.1){//机器人半径大于道路返回
                return std::numeric_limits<float>::max();
            }

            if (minr >= r){
                minr = r;
            }
        }
    }

    return 1.0 / minr;//距障碍物越近  1/x越大
};

float Dwa::calc_to_goal_cost(Traj traj, Point goal, Config config){

    float goal_magnitude = std::sqrt(goal.x_*goal.x_ + goal.y_*goal.y_);
    float traj_magnitude = std::sqrt(std::pow(traj.back().x_, 2) + std::pow(traj.back().y_, 2));
    float dot_product = (goal.x_ * traj.back().x_) + (goal.y_ * traj.back().y_);
    float error = dot_product / (goal_magnitude * traj_magnitude);
    float error_angle = std::acos(error);
    float cost = config.to_goal_cost_gain * error_angle;

    return cost;
};
float Dwa::calc_to_goalDist_cost(Traj traj, Point goal, Config config){

	 
	 int skip_n = 2;
	 float minr = 1;
	 float cost = 100;
         char cnt =0;
	 float base_dis = std::sqrt(std::pow(traj[0].x_ - goal.x_,2)  + std::pow(traj[0].y_ - goal.y_,2));
	 
	 for (unsigned int ii=0; ii<traj.size(); ii+=skip_n){
		
			 float ox = goal.x_;
			 float oy = goal.y_;
			 float dx = traj[ii].x_ - ox;
			 float dy = traj[ii].y_ - oy;
	
			 float r = std::sqrt(dx*dx + dy*dy);
			 if(r > base_dis)
			 {
				base_dis = r;
				cnt ++;
				if(cnt >2)
					return 10;//此路不通
			 } 
			 cost = r/(r+1);
			 if (cost <= minr){ 
                 minr = cost;
             }
	
	}
   return    cost;
};
Traj Dwa::calc_final_input(
        State x, Control& u,
        Window dw, Config config, Point goal,
        std::vector<Point>ob){

    float min_cost = 10000.0;
    Control min_u = u;
    min_u.v_ = 0.0;
    Traj best_traj;

    // capture the start time
    clock_t         start, stop;
    start = clock();

    // evalucate all trajectory with sampled input in dynamic window
    int traj_cnt = 0;
    for (float v=dw.min_v_; v<=dw.max_v_; v+=config.v_reso){
        printf("v=%.1f,w scale:%.1f,%.1f \n",v,dw.min_w_,dw.max_w_);
        for (float w=dw.min_w_; w<=dw.max_w_; w+=config.yawrate_reso){

            Traj traj = calc_trajectory(x, v, w, config);

            float to_goal_cost = calc_to_goal_cost(traj, goal, config);
            float speed_cost = config.speed_cost_gain * (config.max_speed - traj.back().v_);
            float ob_cost = calc_obstacle_cost(traj, ob, config);
            float dis_cost = calc_to_goalDist_cost(traj, goal, config);	    
            // by lide 增加比重
		    // 航向得分的比重、速度得分的比重、障碍物距离得分的比重 
            //evalParam = [];
            float final_cost = to_goal_cost + speed_cost + ob_cost +dis_cost;
            printf("ob cost:%.3f,goalcost:%.1f,dis_cost:%.1f,speed_cost:%.1f\n",ob_cost,to_goal_cost,dis_cost,speed_cost);
             printf("finanl cost:%.3f \n",final_cost);
            if (min_cost > final_cost){
                min_cost = final_cost;
                min_u = Control{v, w};
                best_traj = traj;
            }else  if (min_cost  ==  final_cost)
	    {  
	    	
	    }
	
            traj_cnt ++;
	}
        
    }

    stop = clock();
    float   elapsedTime = (float)(stop - start) /
                          (float)CLOCKS_PER_SEC * 1000.0f;
    printf( "Time to generate:  %3.1f ms\n", elapsedTime );

    u = min_u;
    return best_traj;
};



