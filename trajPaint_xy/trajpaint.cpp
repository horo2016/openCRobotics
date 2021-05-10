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
static double CalculateRadians(int degree)
{
    //弧度
    double current_angle = degree *M_PI/ 180  ;  //角度
    return current_angle;
}
#define Map_Size  300
char Sonars_logs[Map_Size][Map_Size]={0};
int main()
{

	
	int Xr = float(Map_Size/2);// #Set intial robot position
	int Yr = float(Map_Size/2);
// 读文件
    ifstream inFile("Data.txt", ios::in);
	string lineStr;
	vector<vector<string>> strArray;

	Mat src(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0));
	int row = Map_Size;//src.rows;//300
    int col = Map_Size;//src.cols;//300
	Mat Map_log_sonar = Mat(row, col, CV_8UC1,cv::Scalar(0));
	 
	 
	
	 memset(&Sonars_logs[0][0], 127, sizeof(Sonars_logs));
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
		int inv_angle = angle +30 ;//angle for rear sonar sensor
		int left_angle = angle -30 ;
		string a0 = Trim(lineArray[0]);
		string a1 = Trim(lineArray[1]);
		
		
		int avencoder = (atoi(a0.c_str()) + atoi(a1.c_str()))/2 ;//Distance travelled by robot is approximated to average of both encoder readings
		string Fsonar_str =  Trim(lineArray[3]) ;//Sonar values are in cm, convert to mm by muliplying by 10
		int xx = atoi(Fsonar_str.c_str())/10 ;
		string Rsonar_str =  Trim(lineArray[4]);
		int yy = atoi(Rsonar_str.c_str()) /10;
		//string LIR =  Trim(lineArray[5])*10 ;//Same for IR sensor values
		//string RIR =  Trim(lineArray[6])*10 ; 
		cout << "  angle : " << angle << endl;// 
		cout << "  inv_angle : " << inv_angle << endl;//
		cout << "  x : " << xx << endl;//
		cout << "  y : " << yy << endl;//
		printf("convert begin \n");
		cv::Mat Aimg(Map_Size, Map_Size, CV_8UC1,cv::Scalar(0));
		
		Sonars_logs[Yr][Xr]  = 255;
		
		
		//	Calculate new robot position from encoder readings
		//#cv2.line(map_image, (int(Xr),int(Yr)), (int(Xrnext),int(Yrnext)), 150, 1)
		Xr = xx;
		Yr = yy;
		
		std::memcpy(Aimg.data, Sonars_logs, Map_Size*Map_Size*sizeof(unsigned char));
		 
		 Map_log_sonar = Aimg;
		 Mat img3  ;
		 resize(Map_log_sonar,img3,Size(1000,1000));
		 imshow("Aimg",img3);
	
		waitKey(0);
		
	}
}

