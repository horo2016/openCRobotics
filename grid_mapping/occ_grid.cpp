#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stack>
#include <opencv2/opencv.hpp>
#include "sensor_model.h"
#include "occ_grid.h"
#include <fstream>
#include <sstream>
using namespace std;
using namespace cv;

string Trim(string& str)
{
 str.erase(0,str.find_first_not_of(" \t\r\n"));

 str.erase(str.find_last_not_of(" \t\r\n") + 1);

 return str;
}
static double CalculateRadians(int degree)
{
    //弧度
    double current_angle = degree *M_PI/ 180  ;  //角度
    return current_angle;
}
#define Map_Size  300
double Sonars_logs[Map_Size][Map_Size]={0};
int offline_test_Array()
{

	
	
	
    int mm_per_tick = 5 ;//Encoder resolution
	int Xr = float(Map_Size/2);// #Set intial robot position
	int Yr = float(Map_Size/2);
// 读文件
    ifstream inFile("Data.txt", ios::in);
	string lineStr;
	vector<vector<string>> strArray;
	int cnt =0;
	
	int cellsize = 50 ;// #Set cell width and height in mm
	float scale = float(1)/cellsize;// #Set map scale
	
	Mat src(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0));
	int row = Map_Size;//src.rows;//300
    int col = Map_Size;//src.cols;//300
	Mat Map_log_sonar = Mat(row, col, CV_64F,cv::Scalar(0.0));
	Mat Map_log_sonar2= Mat(row, col, CV_64F,cv::Scalar(0.0));
	Mat Map_log_sonar_res= Mat(row, col, CV_64F,cv::Scalar(0.0));
	
	
	while (getline(inFile, lineStr))
	{
		// 打印整行字符串
	//	cout << lineStr << endl;
		// 存成二维表结构
	
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		// 按照逗号分隔
		while (getline(ss, str, ',')){
			lineArray.push_back(str);
		}
		 
		string angle_str =  Trim(lineArray[2]) ;//Robot heading angle
		int angle = atoi(angle_str.c_str());
		int inv_angle = angle+30 ;//angle for rear sonar sensor
		int left_angle = angle-30 ;
		string a0 = Trim(lineArray[0]);
		string a1 = Trim(lineArray[1]);
		
		
		int avencoder = (atoi(a0.c_str()) + atoi(a1.c_str()))/2 ;//Distance travelled by robot is approximated to average of both encoder readings
		string Fsonar_str =  Trim(lineArray[3]) ;//Sonar values are in cm, convert to mm by muliplying by 10
		int Fsonar = atoi(Fsonar_str.c_str())*10;
		string Rsonar_str =  Trim(lineArray[4]);
		int Rsonar = atoi(Rsonar_str.c_str())*10;
		//string LIR =  Trim(lineArray[5])*10 ;//Same for IR sensor values
		//string RIR =  Trim(lineArray[6])*10 ; 
		cout << "  angle : " << angle << endl;// 
		cout << "  inv_angle : " << inv_angle << endl;//
		cout << "  Fsonar : " << Fsonar << endl;//
		cout << "  Rsonar : " << Rsonar << endl;//
		printf("convert begin \n");
		cv::Mat Aimg(Map_Size, Map_Size, CV_64F,cv::Scalar(0.0));
		if (Fsonar < 5000){
			
			SonarModelArray(src,Xr,Yr,angle,Fsonar,cellsize,scale);
			std::memcpy(Aimg.data, Sonars_logs, Map_Size*Map_Size*sizeof(double));
			//cv::add(Aimg,Map_log_sonar,Map_log_sonar);
			Map_log_sonar = Map_log_sonar + Aimg;
			
		}
		
		if (Rsonar < 5000){
			
			SonarModelArray(src,Xr,Yr,inv_angle,Rsonar,cellsize,scale);
			
		
			std::memcpy(Aimg.data, Sonars_logs, Map_Size*Map_Size*sizeof(double));
			//cv::add(Aimg,Map_log_sonar,Map_log_sonar);
			Map_log_sonar = Map_log_sonar + Aimg;
             
		}
			if (Rsonar < 5000){
			
			SonarModelArray(src,Xr,Yr,left_angle,Rsonar,cellsize,scale);
			
		
			std::memcpy(Aimg.data, Sonars_logs, Map_Size*Map_Size*sizeof(double));
			//cv::add(Aimg,Map_log_sonar,Map_log_sonar);
			Map_log_sonar = Map_log_sonar + Aimg;
             
		}
		int Xrnext = Xr + float((cos(CalculateRadians(angle)) * (avencoder*mm_per_tick)*scale)) ;//
		//	Calculate new robot position from encoder readings
		int Yrnext = Yr + float((sin(CalculateRadians(angle)) * (avencoder*mm_per_tick)*scale));

		//#cv2.line(map_image, (int(Xr),int(Yr)), (int(Xrnext),int(Yrnext)), 150, 1)
		Xr = Xrnext;
		Yr = Yrnext;
		
		exp(Map_log_sonar, Map_log_sonar2);//e的0次方等于1
		Mat img2(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0)); 
		Mat img3(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0));
		img2 =1- (1 / ( 1 + Map_log_sonar2));
		img3 = 1 -img2;
		//resize(src, dst, cv::Size(1000, 1000));
		resize(img3,img3,Size(1000,1000));
		//imshow("Map_log",img3);
		imwrite("Map_log.png",img3*255);
		waitKey(0);
		
	}
}

float dis[]={493.5, 496.25, 500.0, 504.0, 508.25, 511.5, 517.0, 521.5, 525.75, 532.75, 538.75, 544.25, 551.25, 558.5, 565.25, 573.25, 581.25, 591.0, 599.75, 611.75, 622.5, 634.25, 647.5, 660.75, 674.75, 690.25, 709.0, 725.25, 746.5, 765.75, 790.0, 812.25, 838.25, 866.5, 896.5, 925.5, 963.75, 1001.25, 1048.0, 1094.75, 1145.25, 1205.25, 1268.0, 1341.25, 1420.5, 1522.5, 1624.0, 1744.25, 1880.25, 2046.0, 2126.0, 2807.75, 2841.75, 2987.75, 2987.5, 2981.0, 2976.0, 2973.0, 2971.75, 2970.0, 2969.75, 2969.75, 2971.75, 2978.5, 2980.75, 2981.0, 2993.5, 3010.5, 3018.5, 2733.25, 2493.5, 3054.75, 3076.0, 3096.25, 3109.0, 3141.75, 3161.0, 3195.75, 3220.5, 3267.25, 3036.25, 3172.5, 3164.25, 2457.5, 2495.75, 2537.0, 2574.25, 2626.0, 3734.5, 2692.5, 2725.25, 2780.25, 2860.75, 2894.0, 4177.25, 4284.25, 4409.25, 4680.75, 4728.75, 4619.5, 4269.5, 4206.5, 4151.0, 4110.75, 4061.75, 3988.25, 3995.0, 3918.75, 3898.5, 3888.5, 3846.75, 3803.75, 3771.75, 3931.75, 3656.75, 3639.0, 3622.75, 3575.5, 3556.25, 3533.5, 3537.75, 3773.25, 3767.0, 3500.25, 3485.0, 3468.0, 3464.5, 3460.5, 3463.25, 3459.25, 3468.5, 3726.5, 3737.75, 3767.75, 3767.0, 3788.75, 6020.0, 6059.0, 6011.75, 5841.25, 870.5, 870.0, 432.75, 421.25, 416.25, 413.5, 413.5, 414.25, 418.0, 428.5, 441.5, 540.75, 534.75, 530.0, 526.5, 523.25, 524.0, 527.0, 531.25, 536.25, 541.0, 552.0, 561.25, 586.0, 597.5, 606.75, 624.25, 672.25, 2380.25, 2350.5, 2334.75, 2313.25, 2294.25, 2281.0, 2256.5, 384.5, 380.5, 374.75, 375.25, 374.5, 374.25, 375.0, 376.0, 381.25, 2216.0, 2356.25, 2367.25, 2367.5, 2368.5, 2367.75, 1116.75, 1068.75, 1074.0, 1076.5, 1081.25, 1239.0, 1174.25, 1091.75, 873.75, 956.5, 964.5, 976.5, 992.75, 600.5, 591.0, 581.25, 571.75, 291.25, 556.0, 549.0, 178.75, 364.0, 527.0, 523.0, 426.5, 425.75, 415.5, 401.0, 346.25, 333.75, 319.75, 314.25, 305.25, 297.5, 289.5, 286.0, 293.75, 303.75, 314.5, 327.5, 338.5, 493.25, 483.75, 485.25, 487.25, 490.25, 493.25};
float ang[]={354.578125, 355.734375, 357.125, 358.328125, 359.5625, 0.890625, 1.875, 3.21875, 4.53125, 5.6875, 6.984375, 8.296875, 9.5625, 10.609375, 11.828125, 13.015625, 14.375, 15.53125, 16.75, 18.09375, 19.234375, 20.5, 21.75, 22.859375, 24.046875, 25.265625, 26.46875, 27.6875, 28.984375, 30.171875, 31.34375, 32.59375, 33.78125, 35.015625, 36.3125, 37.5625, 38.75, 39.984375, 41.125, 42.328125, 43.578125, 44.75, 45.9375, 47.15625, 48.40625, 49.5625, 50.8125, 52.015625, 53.21875, 54.40625, 55.65625, 56.765625, 58.015625, 59.375, 60.625, 61.921875, 63.1875, 64.46875, 65.734375, 67.015625, 68.265625, 69.546875, 70.828125, 72.109375, 73.34375, 74.65625, 75.90625, 77.203125, 78.4375, 79.796875, 81.109375, 82.28125, 83.484375, 84.78125, 86.0625, 87.296875, 88.578125, 89.828125, 91.125, 92.375, 93.6875, 96.203125, 100.03125, 101.421875, 102.703125, 103.9375, 105.203125, 106.453125, 107.609375, 109.0625, 110.3125, 111.5625, 112.828125, 114.09375, 115.234375, 116.484375, 117.765625, 119.015625, 120.296875, 121.5625, 122.859375, 124.140625, 125.421875, 126.703125, 127.984375, 129.265625, 130.453125, 131.734375, 133.0, 134.265625, 135.546875, 136.8125, 138.09375, 139.359375, 140.640625, 141.9375, 143.1875, 144.46875, 145.71875, 147.0, 148.265625, 149.5, 150.78125, 152.078125, 153.359375, 154.671875, 155.953125, 157.21875, 158.5, 159.78125, 161.03125, 162.28125, 163.546875, 164.8125, 166.109375, 167.359375, 174.859375, 176.125, 177.390625, 179.875, 182.5, 183.875, 186.96875, 188.078125, 189.640625, 190.828125, 192.0, 193.5625, 194.5, 195.671875, 196.734375, 197.59375, 198.90625, 200.203125, 201.5, 202.796875, 203.9375, 205.125, 206.5625, 207.734375, 208.953125, 210.109375, 211.484375, 212.609375, 213.734375, 215.109375, 216.28125, 217.375, 219.59375, 220.875, 222.171875, 223.4375, 224.671875, 225.96875, 227.34375, 231.78125, 233.125, 234.296875, 235.9375, 237.234375, 238.25, 239.609375, 240.8125, 241.96875, 245.21875, 246.46875, 247.734375, 249.0, 250.296875, 251.546875, 253.5, 254.875, 256.078125, 257.375, 258.6875, 259.765625, 261.09375, 262.5, 266.671875, 269.0625, 270.3125, 271.484375, 272.734375, 300.390625, 301.625, 302.9375, 304.3125, 307.90625, 306.84375, 308.125, 314.609375, 312.3125, 312.21875, 313.359375, 317.78125, 319.140625, 320.40625, 322.046875, 324.046875, 325.0, 326.890625, 328.484375, 329.890625, 330.71875, 332.484375, 334.171875, 335.09375, 336.46875, 336.921875, 337.859375, 339.15625, 345.390625, 349.171875, 350.484375, 351.875, 352.921875, 354.125};

int offline_lidar_Array()
{
	int Xr = float(Map_Size/2);// #Set intial robot position
	int Yr = float(Map_Size/2);
	int cellsize = 50 ;// #Set cell width and height in mm
	float scale = float(1)/cellsize;// #Set map scale
	Mat src(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0));
	int row = Map_Size;//src.rows;//300
    int col = Map_Size;//src.cols;//300
	Mat Map_log_sonar = Mat(row, col, CV_64F,cv::Scalar(0.0));
	Mat Map_log_sonar2= Mat(row, col, CV_64F,cv::Scalar(0.0));
	cv::Mat Aimg(Map_Size, Map_Size, CV_64F,cv::Scalar(0.0));
	for(int i=0;i<360;i++){
		float angle = ang[i];
		float Fdis = dis[i];
		if (Fdis < 5000){
			LidarModelArray(src,Xr,Yr,angle,Fdis,cellsize,scale);
			
		}
	}
	std::memcpy(Aimg.data, Sonars_logs, Map_Size*Map_Size*sizeof(double));
			Map_log_sonar = Map_log_sonar + Aimg;
	exp(Map_log_sonar, Map_log_sonar2);//e的0次方等于1
	Mat img2(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0)); 
	Mat img3(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0));
	img2 =1- (1 / ( 1 + Map_log_sonar2));
	img3 = 1 -img2;
	resize(img3,img3,Size(1000,1000));
	imwrite("Map_lidars_log.png",img3*255);
}
