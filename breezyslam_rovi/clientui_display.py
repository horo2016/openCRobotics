#-*- coding:utf-8 -*-  
from socket import *
import json
MAP_SIZE_PIXELS         = 500
MAP_SIZE_METERS         = 10
LIDAR_DEVICE            = '/dev/rplidar'
# Ideally we could use all 250 or so samples that the RPLidar delivers in one 
# scan, but on slower computers you'll get an empty map and unchanging position
# at that rate.
MIN_SAMPLES   = 180
import sys
import os
import time
import keyboard
from breezyslam.algorithms import RMHC_SLAM
from breezyslam.sensors import RPLidarA1 as LaserModel
from roboviz import MapVisualizer
def keyboard_callback(x):
    #键盘箭头值:103 ↑ ,105 ← ,106 →  108↓
    print(x.scan_code)
    
    tcp_client_socket.send(bytes(x.scan_code))
    flg =x.scan_code
    '''a = keyboard.KeyboardEvent('down', 28, 'enter')
    #按键事件a为按下enter键，第二个参数如果不知道每个按键的值就随便写，
    #如果想知道按键的值可以用hook绑定所有事件后，输出x.scan_code即可
    if x.event_type == 'down' and x.name == a.name:
        print("你按下了enter键")'''
    #当监听的事件为enter键，且是按下的时候
def mm2pix(mm):
    return int(mm / (MAP_SIZE_METERS * 1000. / MAP_SIZE_PIXELS)) 
def main():
    global tcp_client_socket
    global flg 
    flg ="0"
    # Create an RMHC SLAM object with a laser model and optional robot model
    slam = RMHC_SLAM(LaserModel(), MAP_SIZE_PIXELS, MAP_SIZE_METERS)
    # Set up a SLAM display
    viz = MapVisualizer(MAP_SIZE_PIXELS, MAP_SIZE_METERS, 'VosSLAM')
    # Initialize an empty trajectory
    trajectory = []
    # Initialize empty map
    mapbytes = bytearray(MAP_SIZE_PIXELS * MAP_SIZE_PIXELS)
    # Create an iterator to collect scan data from the RPLidar
    #   iterator = lidar.iter_scans()
    # We will use these to store previous scan in case current scan is inadequate
    previous_distances = None
    previous_angles    = None
    # 创建tcp_client_socket 套接字对象
    tcp_client_socket = socket(AF_INET,SOCK_STREAM)
    #连接服务器
    tcp_client_socket.connect(("192.168.31.135",55555))
    while True:
        """无限循环可以实现无限聊天"""

        recv_data = tcp_client_socket.recv(1024*1024)
        time.sleep(0.05)
        if not recv_data:#如果为空
            #print("datas is empty")
            continue
        tcp_client_socket.send(flg)
        #找到两个list的中间间隔符的位置
        startindex =recv_data.find('<')
        if startindex==-1:
            print("cannot  find <")
            continue
        splitindex =recv_data.find('+')
        if splitindex==-1:
            print("cannot  find +")
            continue
        endindex =recv_data.find('>')
        if endindex==-1:
            print("cannot  find >")
            continue
        if startindex>splitindex or splitindex>endindex:
            continue
        
        #print("position:")
        #print(startindex,splitindex,endindex)
        #print("length:")
        #print(len(recv_data))
        #分别取两段赋值给距离和角度
        distances = json.loads(recv_data[startindex+1:splitindex])
        angles = json.loads(recv_data[splitindex+1:endindex])
        '''if recv_data:
            print("receive msg")
            print(recv_data)
        else:
            print("对方已离线。。")
            break'''
        # Update SLAM with current Lidar scan and scan angles if adequate
        slam.update(distances, scan_angles_degrees=angles)
        # Get current robot position
        x, y, theta = slam.getpos()
        # Add new position to trajectory
        trajectory.append((x, y))
        # Get current map bytes as grayscale
        slam.getmap(mapbytes)
        # Put trajectory into map as black pixels
        for coords in trajectory:
            x, y = coords
            x_pix = mm2pix(x)
            y_pix = mm2pix(y)
            mapbytes[y_pix * MAP_SIZE_PIXELS + x_pix] = 0;
        # Display map and robot pose, exiting gracefully if user closes it
        if not viz.display(x/1000., y/1000., theta, mapbytes):
         exit(0)
    tcp_client_socket.close()
if __name__ == '__main__':
    keyboard.hook(keyboard_callback)
    main()
    keyboard.wait()
