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
using namespace std;
using namespace cv;
 




static Point get_centre(vector<Point> v) {
	double x = 0, y = 0;
	int s = v.size();
	for (int i = 0; i < v.size(); i++) {
		x += v[i].x * 100/s;
		y += v[i].y * 100/s;
	}
	return Point((int) x/100, (int) y/100);
}


//计算两个点的角度
static double CalculateAngle(Point Mar1Point, Point Mar2Point)
{
  //  double k = (double)(Mar2Point.y - Mar1Point.y) / (Mar2Point.x - Mar1Point.x);  //计算斜率
    double arcLength1 = atan2(Mar2Point.y - Mar1Point.y,Mar2Point.x - Mar1Point.x);    //弧度
    double current_angle = arcLength1 * 180 / M_PI;  //角度
    return current_angle;
}



double **SonarModel(cv::Mat occmap,int Xr,int Yr,int Rangle,int SonarDist,int cellsize,float scale)
{

	int Map_size = occmap.cols;
	int thick  = int(cellsize * scale);
     SonarDist = int(SonarDist * scale);
	//创建一张图片  100*100的unsigned char 矩阵 都为0
	//Mat img1(Map_size, Map_size, CV_8UC1, cv::Scalar(0));
	//Returns: Sonar_log - Array of log odds probabilities. 
	//This can be added to existing log odds occ map to update map with latest sonar data.
	//double **Sonar_log[Map_size][Map_size];
	double **Sonar_log= (double**)malloc(Map_size*sizeof(double));
	for (int i = 0; i < Map_size; i++)
	{
		Sonar_log[i] = (double *)malloc(Map_size * sizeof(double));
	}
	
	 // 进行图像灰度化操作
   // cvtColor(src, src, CV_BGR2GRAY);
//	threshold(src, grayMaskSmallThresh, 230, 1, CV_THRESH_BINARY_INV);//反相选择 >230 =0,else= 1
    //获取 mat 的行和列
    int row = occmap.rows;//300
    int col = occmap.cols;//300
    
    int thicknesss=-1;
    
	//printf("SonarDist:%d \n",SonarDist);
	ellipse(occmap,Point(int(Xr),int(Yr)),Size(SonarDist, SonarDist), Rangle, -15, 15, Scalar(255), thicknesss);
	//Mat dst;
	//resize(src, dst, cv::Size(1000, 1000));
	
	
	//创建一个mat用于存储新的坐标
	//std::vector<cv::Point2i> locations;   // output, locations of non-zero pixels 
	Mat wLocMat = Mat::zeros(occmap.size(),CV_8UC1); 
	//findNonZero(src, locations);
	findNonZero(occmap, wLocMat);
    for (int i = 0; i < wLocMat.total(); i++ ) {
       // cout << "Zero#" << i << ": " << wLocMat.at<Point>(i).x << ", " << wLocMat.at<Point>(i).y << endl;
		
		//使用范数求距离
		//使用范数求距离
		float dealtx = pow(wLocMat.at<Point>(i).x - Xr,2);
		float dealty = pow(wLocMat.at<Point>(i).y - Yr,2);
		float dist = sqrt(dealtx + dealty);
		//printf("dist:%d \n",dist);//dist zhengque
		int  Ximg = wLocMat.at<Point>(i).x ; //# 是一个值 对应x
		int  Yimg = wLocMat.at<Point>(i).y ;//# 是一个值 对应Y
		Point startp,robotp;
		startp.x = Ximg;
		startp.y = Yimg;
		robotp.x = Xr;
		robotp.y = Yr;
		
	    float theta=	CalculateAngle(robotp,startp) - Rangle;
		/////数量一致  小数点有偏差
		if(theta < -180)                   //#Note:numpy and OpenCV X and Y reversed
            theta = theta + 360 ;
        else if( theta > 180)
            theta = theta - 360 ;
		
		int    sigma_t = 5;
        double A = 1 / (sqrt(2*M_PI*sigma_t));
        double C = pow((theta/sigma_t),2);
        double B = exp(-0.5*C);
        double Ptheta = A*B ;
		//小数点有偏差
		
        double Pdist = (SonarDist - dist/2)/SonarDist;
        double P = (Pdist*2)*Ptheta;
		double Px=0,logPx=0 ;
		//printf("dist:%f %d %d\n",dist,SonarDist,thick);
		if (dist > (SonarDist - thick) && dist < (SonarDist + thick))// #occupied region
		{
			Px = 0.5 + Ptheta;
			printf("Px:%f \n",Px);
            logPx = log(Px/(1-Px));
            Sonar_log[Yimg][Ximg] = logPx;
			printf("logPx:%f \n",logPx);
           // #occ = np.append(occ,[x],0)
		}
        else{// #free region
            Px = 0.5 - P;
            logPx = log(Px/(1-Px));
            Sonar_log[Yimg][Ximg] = logPx;
			//printf("p:%d,%d \n",Yimg,Ximg);
		}
    }
	

	//unsigned char *ptmp = NULL;//这是关键的指针！！   ptmp指针指向的是img2这个图像
	
   
	return Sonar_log;
    
    
}
int SonarModelArray(cv::Mat occmap,int Xr,int Yr,int Rangle,int SonarDist,int cellsize,float scale)
{

	int Map_size = occmap.cols;
	//这里必须重新构建新地图，如果直接使用occmap 绘制，会导致重复使用出现重影
	Mat mapgrid(Map_size, Map_size, CV_8UC1, cv::Scalar(0));
	int thick  = int(cellsize * scale);
     SonarDist = int(SonarDist * scale);
	//创建一张图片  100*100的unsigned char 矩阵 都为0
	//Mat img1(Map_size, Map_size, CV_8UC1, cv::Scalar(0));
	//Returns: Sonar_log - Array of log odds probabilities. 
	//This can be added to existing log odds occ map to update map with latest sonar data.
	//double **Sonar_log[Map_size][Map_size];
	
	

    int row = occmap.rows;//300
    int col = occmap.cols;//300
    
    int thicknesss=-1;
    
	//printf("SonarDist:%d \n",SonarDist);
	//在新地图中使用画椭圆
	ellipse(mapgrid,Point(int(Xr),int(Yr)),Size(SonarDist, SonarDist), Rangle, -15, 15, Scalar(255), thicknesss);
	//Mat dst;
	//resize(src, dst, cv::Size(1000, 1000));
	
	
	//创建一个mat用于存储新的坐标
	//std::vector<cv::Point2i> locations;   // output, locations of non-zero pixels 
	Mat wLocMat = Mat::zeros(mapgrid.size(),CV_8UC1); 
	//findNonZero(src, locations);
	//在新地图中找到非0,不能使用原地图
	findNonZero(mapgrid, wLocMat);
	//for(int m=0;m<row;m++)
	//	memset(Sonars_logs[m],0,col);
    for (int i = 0; i < wLocMat.total(); i++ ) {
       // cout << "Zero#" << i << ": " << wLocMat.at<Point>(i).x << ", " << wLocMat.at<Point>(i).y << endl;
		
		//使用范数求距离
		//使用范数求距离
		float dealtx = pow(wLocMat.at<Point>(i).x - Xr,2);
		float dealty = pow(wLocMat.at<Point>(i).y - Yr,2);
		float dist = sqrt(dealtx + dealty);
		//printf("dist:%d \n",dist);//dist zhengque
		int  Ximg = wLocMat.at<Point>(i).x ; //# 是一个值 对应x
		int  Yimg = wLocMat.at<Point>(i).y ;//# 是一个值 对应Y
		Point startp,robotp;
		startp.x = Ximg;
		startp.y = Yimg;
		robotp.x = Xr;
		robotp.y = Yr;
		
	    float theta=	CalculateAngle(robotp,startp) - Rangle;
		/////数量一致  小数点有偏差
		if(theta < -180)                   //#Note:numpy and OpenCV X and Y reversed
            theta = theta + 360 ;
        else if( theta > 180)
            theta = theta - 360 ;
		
		double    sigma_t = 5;
        double A = 1 / (sqrt(2*M_PI*sigma_t));
        double C = pow((theta/sigma_t),2);
        double B = exp(-0.5*C);
        double Ptheta = A*B ;
		//小数点有偏差
		
        double Pdist = (SonarDist - dist/2)/SonarDist;
        double P = (Pdist*2)*Ptheta;
		double Px=0,logPx=0 ;
		//printf("dist:%f %d %d\n",dist,SonarDist,thick);
		if (dist > (SonarDist - thick) && dist < (SonarDist + thick))// #occupied region
		{
			Px = 0.5 + Ptheta;
			//printf("Px:%f \n",Px);
            logPx = log(Px/(1-Px));
            Sonars_logs[Yimg][Ximg] = logPx;
			//printf("logPx:%f \n",logPx);
           // #occ = np.append(occ,[x],0)
		}
        else{// #free region
            Px = 0.5 - P;
			//printf("Px free:%f \n",Px);
            logPx = log(Px/(1-Px));
            Sonars_logs[Yimg][Ximg] = logPx;
			//printf("logPx free:%f \n",logPx);
			//printf("p:%d,%d \n",Yimg,Ximg);
		}
    }
	

	//unsigned char *ptmp = NULL;//这是关键的指针！！   ptmp指针指向的是img2这个图像
	
   
	return 0;
    
    
}