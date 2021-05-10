//
// Created by pang on 2020/6/13.
//

#ifndef DWA_DEMO_UTILITY_H
#define DWA_DEMO_UTILITY_H

#include <vector>


#define PI 3.141592653



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
    float min_speed = 0.1; //v/w =r 可知道旋转半径
    float max_yawrate =   0.5;//40.0 * PI / 180.0;//0.5rad =30du 0.1rad=6du 0.4rad=23du
    float min_yawrate = -0.5;
    float max_accel = 0.2;
    float robot_radius = 0.6;//1 bylide
    float max_dyawrate = 0.2;//   80 * PI / 180.0; //可以求出最小旋转半径  

    float v_reso = 0.1;//
    float yawrate_reso =   0.1;//0.1 * PI / 180.0;//分辨率太大 导致轨迹间距太大  

    float dt = 0.5;
    float predict_time = 10.0;//预测5/0.5 个轨迹点  最后一个轨迹点的方向如果背离目标可以抛弃作为后期优化
    float to_goal_cost_gain = 1.0;// 权重调节 认为航向角的比重为0.5
    float speed_cost_gain = 1.0;
};



#endif //DWA_DEMO_UTILITY_H
