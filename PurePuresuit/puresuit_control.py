"""
移动到具体位置
使用权重比例法
"""

import matplotlib.pyplot as plt
import numpy as np
from random import random
import math


#仿真参数
#比例系数
Kp_rho = 0.9
Kp_alpha = 0.35 
Kp_beta = 0.15 
#时间周期
dt = 0.3
#最小接近距离
limit_dis =0.3
limit_ang =10
show_animation = True


def pp_control(x_start, y_start, theta_start, x_goal, y_goal):
    """
    d 为机器人和目标之间的距离
    alpha  为目标相对于本身的方向方位角
    beta 为最终角度和方位角的差
    """
    x = x_start
    y = y_start
    theta = theta_start

    x_diff = x_goal - x
    y_diff = y_goal - y

    x_traj, y_traj = [], []

    d = np.hypot(x_diff, y_diff)
    dis = d
    
    #满足到达目标的最小距离和角度最小值
    while dis > limit_dis:
        x_traj.append(x)
        y_traj.append(y)

        x_diff = x_goal - x
        y_diff = y_goal - y
        # 角度差
        # 范围为[-pi, pi] 
        # 从 0 到 2*pi 
        #求两点距离
        dis = np.hypot(x_diff, y_diff)
        #求两点角度
        gama =np.arctan2(y_diff, x_diff)
        alpha = gama - theta   
        #角度归一化
        if  (alpha < -np.pi):                    
            alpha = alpha + 2 * np.pi 
        elif(alpha > np.pi):
            alpha = alpha - 2 * np.pi 
        
        #使用匀速行驶
        v = 0.50
        if(dis < limit_dis):
            v = 0
        #根据匀速计算角速度w
        w = 2*v*math.sin(alpha)/dis

        print("dis,v,w,theta,oritation,goal")
        print(dis,v,w,theta*180/3.14,gama*180/3.14)
        theta = theta + w * dt
        if  (theta < -np.pi):                     
            theta = theta + 2 * np.pi 
        elif(theta > np.pi):
            theta = theta - 2 * np.pi 
        x = x + v * np.cos(theta) * dt
        y = y + v * np.sin(theta) * dt
        
        if show_animation:  # pragma: no cover
            plt.cla()
            plt.arrow(x_start, y_start, np.cos(theta_start),
                      np.sin(theta_start), color='r', width=0.1)
            plt.arrow(x_goal, y_goal, np.cos(20*3.1415/180),
                      np.sin(20*3.1415/180), color='g', width=0.1)
            plot_vehicle(x, y, theta, x_traj, y_traj)
    


def plot_vehicle(x, y, theta, x_traj, y_traj):  # pragma: no cover
    # Corners of triangular vehicle when pointing to the right (0 radians)
    p1_i = np.array([0.5, 0, 1]).T
    p2_i = np.array([-0.5, 0.25, 1]).T
    p3_i = np.array([-0.5, -0.25, 1]).T

    T = transformation_matrix(x, y, theta)
    p1 = np.matmul(T, p1_i)
    p2 = np.matmul(T, p2_i)
    p3 = np.matmul(T, p3_i)

    plt.plot([p1[0], p2[0]], [p1[1], p2[1]], 'k-')
    plt.plot([p2[0], p3[0]], [p2[1], p3[1]], 'k-')
    plt.plot([p3[0], p1[0]], [p3[1], p1[1]], 'k-')

    plt.plot(x_traj, y_traj, 'b--')

    # for stopping simulation with the esc key.
    plt.gcf().canvas.mpl_connect('key_release_event',
            lambda event: [exit(0) if event.key == 'escape' else None])

    plt.xlim(0, 20)
    plt.ylim(0, 20)

    plt.pause(dt)


def transformation_matrix(x, y, theta):
    return np.array([
        [np.cos(theta), -np.sin(theta), x],
        [np.sin(theta), np.cos(theta), y],
        [0, 0, 1]
    ])


def main():
    x_goal = 2
    for i in range(1):
        x_start = 1 
        y_start = 10 
        theta_start = 10*3.1415/180
        x_goal  =5 
        y_goal = 12 
        
        print("Initial x: %.2f m\nInitial y: %.2f m\nInitial theta: %.2f rad\n" %
              (x_start, y_start, theta_start))
        print("Goal x: %.2f m\nGoal y: %.2f m\n" %
              (x_goal, y_goal))
        pp_control(x_start, y_start, theta_start, x_goal, y_goal)


if __name__ == '__main__':
    main()
