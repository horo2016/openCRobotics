//
// Created by pang on 2020/6/13.
//

#ifndef DWA_DEMO_DWA_H
#define DWA_DEMO_DWA_H
#include "utility.h"


class Dwa {
public:
    Dwa(const State& start, const Point& goal,  const Obstacle& obs, const Config& config);

    bool stepOnceToGoal(std::vector<State>* best_trajectry, State* cur_state,Obstacle *cur_obs);
//	int update_obstacle(sonar_dis *sonars);
    State motion(State x, Control u, float dt);
	State motion_calculate(State x, float heading_d,Control u, float dt);
	int  state_error_check(float head_d);
    Point goal_;
    Obstacle obs_;
    Config config_;
    Control calculated_u;
    Control feed_u;
    State cur_x_;
private:
    
    Window calc_dynamic_window(State x, Config config);
    Traj calc_trajectory(State x, float v, float w, Config config);
    float calc_obstacle_cost(Traj traj, Obstacle ob, Config config);
    float calc_to_goal_cost(Traj traj, Point goal, Config config);
	
	float calc_to_goalDist_cost(Traj traj, Point goal, Config config);
	
    Traj calc_final_input(
            State x, Control& u,
            Window dw, Config config, Point goal,
            std::vector<Point>ob);



   

};


#endif //DWA_DEMO_DWA_H
