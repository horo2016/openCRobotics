//
// Created by pang on 2020/6/13.
//

#ifndef DWA_DEMO_UTILITY_H
#define DWA_DEMO_UTILITY_H

#include <vector>


#define PI 3.141592653

#define CENTER_FACE_SONAR  1
#define LEFT_FACE_SONAR    2
#define RIGHT_FACE_SONAR   0


struct State {
    float x_;
    float y_;
    float theta_;
    float v_;
    float w_;
};


using Traj = std::vector<State>;

struct Window {
    float min_v_;
    float max_v_;
    float min_w_;
    float max_w_;

};

struct Control {
    float v_;
    float w_;
};

struct Point {
    float x_;
    float y_;
};

using Obstacle = std::vector<Point>;;

struct Config{
    float max_speed = 0.5;
    float min_speed = 0.1;
    float max_yawrate =0.3;//  30.0 * PI / 180.0;
    float min_yawrate = -0.5;
    float max_accel = 0.2;
    float max_dyawrate = 0.2; //40.0 * PI / 180.0;

    float v_reso = 0.1;
    float yawrate_reso = 0.1;//0.5 * PI / 180.0;
//预测时间和单步执行时间
    float dt = 0.5;
    float predict_time = 5.0;
    //障碍物最小距离
    float obstacle_min_dis = 0.05;
    //距离目标最小距离
    float goal_min_dis = 1.0;
    float robot_radius = 0.6;//1 bylide 实际中注意这个机器人的半径 太大导致原地打转
    float to_goal_cost_gain = 1.0;// 权重调节 认为航向角的比重为
    float speed_cost_gain = 1.0;
};



#endif //DWA_DEMO_UTILITY_H
