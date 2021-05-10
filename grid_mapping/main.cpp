#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <stack>
#include <opencv2/opencv.hpp>
#include <math.h>
#include "sensor_model.h"
#include "occ_grid.h"
using namespace std;
using namespace cv;
 
 
 #define WIDTH  50
 #define HEIGHT 50



Point get_centre(vector<Point> v) {
	double x = 0, y = 0;
	int s = v.size();
	for (int i = 0; i < v.size(); i++) {
		x += v[i].x * 100/s;
		y += v[i].y * 100/s;
	}
	return Point((int) x/100, (int) y/100);
}

#define PI  3.141592627
//计算两个点的角度
double CalculateAngle(Point Mar1Point, Point Mar2Point)
{
  //  double k = (double)(Mar2Point.y - Mar1Point.y) / (Mar2Point.x - Mar1Point.x);  //计算斜率
    double arcLength1 = atan2(Mar2Point.y - Mar1Point.y,Mar2Point.x - Mar1Point.x);    //弧度
    double current_angle = arcLength1 * 180 / M_PI;  //角度
    return current_angle;
}
int Map_Size = 100;

int Xr = float(Map_Size/2);// #Set intial robot position
int Yr = float(Map_Size/2);
int Rangle = -90;


int single_point()
{
	int cellsize = 100 ;// #Set cell width and height in mm
	float scale = float(1)/cellsize;// #Set map scale
	
	Mat src(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0));
	int row = src.rows;//300
    int col = src.cols;//300
	double **Sonar_logs =SonarModel(src,Xr,Yr,-90,4500,cellsize,scale);
	Mat img2 ;//= Mat(row, col);//图像img2：row*col大小  这里只是定义了img2图像的大小还没有传递图像的信息
	Mat Map_log = Mat(row, col, CV_64F,cv::Scalar(0.0));
	//Mat Map_log2 = Mat(row, col, CV_64F,cv::Scalar(0.0));

	
	cv::Mat Aimg(Map_Size, Map_Size, CV_64F);
	double* data =NULL;
	for(int i=0;i<Map_Size;i++)  //行数--高度
    {
         data = Aimg.ptr<double>(i); //指向第i行的数据
        for(int j=0;j<Map_Size;j++)      //列数 -- 宽度
        {
            
                data[j] = Sonar_logs[i][j];
        }
    }
	//std::memcpy(Aimg.data, Sonar_log, Map_Size*Map_Size*sizeof(double));
	cv::add(Aimg,Map_log,Map_log);

	exp(Map_log, Map_log);
	 
	img2= (1 / ( 1 + Map_log));
	//resize(src, dst, cv::Size(1000, 1000));
	resize(img2,img2,Size(1000,1000));
	imshow("Map_log",img2);
	waitKey(0);
	for(int i=0;i< Map_Size;i++)
		free(Sonar_logs[i]);
	free(Sonar_logs);
}

int main(int argc ,char *argv[])
{
	offline_test_Array();

}