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
		imshow("Map_log",img3);
		waitKey(0);
		
	}
}
