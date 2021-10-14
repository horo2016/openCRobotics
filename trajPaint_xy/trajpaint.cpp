#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stack>
#include <opencv2/opencv.hpp>
#include "trajpaint.h"
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

#define Map_Size  300
char Sonars_logs[Map_Size][Map_Size]={0};
int main()
{

	//设置机器人初始位置
	int Xr = float(Map_Size/2);
	int Yr = float(Map_Size/2);
    // 读文件
    ifstream inFile("xy.txt", ios::in);
	string lineStr;
	//定义两个Mat图片
	Mat src(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0));
	Mat Map_log_sonar = Mat(Map_Size, Map_Size, CV_8UC1,cv::Scalar(0));
	//初始化灰色背景
	 memset(&Sonars_logs[0][0], 127, sizeof(Sonars_logs));
	while (getline(inFile, lineStr))
	{
	
		stringstream ss(lineStr);
		string str;
		vector<string> lineArray;
		// 按照逗号分隔
		while (getline(ss, str, ',')){
			lineArray.push_back(str);
		}
		//分别读取x,y坐标
		string a0 = Trim(lineArray[0]);
		string a1 = Trim(lineArray[1]);
		int xx = atoi(a0.c_str()) ;
		int yy = atoi(a1.c_str()) ; 
		cv::Mat Aimg(Map_Size, Map_Size, CV_8UC1,cv::Scalar(0));
		Sonars_logs[Yr][Xr]  = 255;
		Xr = xx;
		Yr = yy;
		//数组中的数据拷贝到Mat 中
		std::memcpy(Aimg.data, Sonars_logs, Map_Size*Map_Size*sizeof(unsigned char));		 
		Map_log_sonar = Aimg;

	}
	imshow("traj",Map_log_sonar);
	waitKey(0);
}

