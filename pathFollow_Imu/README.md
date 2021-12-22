# base_comm
base_comm to stm32
send command  to stm32 control 
##imu.cpp
1 RtIMU read imu heading,roll,pitch
##odometry.c
1 get odom in y axix；
#stm32_control.c  the interface to chasis
to stm32 chasis by (v,w)and get (v,w)
##move2poe
read a serias points from txt.and control the chasis
follow the point trajactory; 

路径跟随使用IMU和里程计的测试
1将几个航点坐标存入square。txt
2程序读取航点坐标并调用算法跟随轨迹
3 测试里程计和IMU分辨率效果
## odom path follow
![odom_map_traj](https://github.com/horo2016/openCRobotics/edit/master/pathFollow_Imu/Map_traj.png)
