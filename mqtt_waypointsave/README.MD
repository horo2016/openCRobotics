
##  开源路径规划与定位显示的方法和步骤  
本文档包含以下内容：  
1手动规划路径  
2 路径信息接收  
首先在树莓派或者其他的Linux系统安装 mosquito   
sudo apt-get install mosquitto-dev  
sudo apt-get install mosquitto-clients  
从github 网站git clone 代码mqttwaypoints至树莓派，已经在树莓派3B/+、ubuntu测试，然后 make 生成waypoint 程序，使用sudo   ./waypoint  ，执行一次，按ctrl +c 结束程序，获取sn.txt .
cat sn.txt查看 唯一序列号，将该序列号添加到网站上。   
重新执行sudo  ./waypoint 并在后台运行，网页地图订阅端显示在线后，即可在地图上路径规划，规划完路径后，单击【保存】后可在程序所在路径下获取   
waypoints.csv 内含经纬度路径信息.   
Mqtt/lib 下有不同架构不同的库，注意区分   。
#### 注：该程序和web 下的 map_car.html 配合使用获取路径规划的航点和定位。
联系qq:839602830   
v:13699147102   
