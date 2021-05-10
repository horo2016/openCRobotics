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
 //https://blog.csdn.net/tanjia6999/article/details/79966694
struct bgr_color {
    int b_low;
    int b_high;
    int g_low;
    int g_high;
    int r_low;
    int r_high;
};
class CPoint
{
public:
    CPoint(int x,int y):X(x),Y(y),G(0),H(0),F(0),m_parentPoint(NULL){ };
    ~CPoint();
    int X,Y,G,H,F;
    CPoint * m_parentPoint;
    void CalF(){
        F=G+H;
    };
};
class  CAStar
{
    private:
	int m_array[WIDTH][HEIGHT];
	static const int STEP = 10;
    static const int OBLIQUE = 14;
    
	typedef std::vector<CPoint*> POINTVEC;
	POINTVEC m_openVec;
	POINTVEC m_closeVec;
    public:
        CAStar(int array[WIDTH][HEIGHT])
        {
            for (int i=0;i<WIDTH;i++)
                for(int j=0;j<HEIGHT;j++)
                    m_array[i][j]=array[i][j];
        }
        CPoint* GetMinFPoint()
        {
            int idx=0,valueF=9999;
			printf("1 m_openVec size : %d \n",m_openVec.size());
            for(int i=0; i < m_openVec.size(); i++)//再开环中查找有几个坐标
                {
					printf("2 m_openVec[i]->F (%d,%d) %d \n",m_openVec[i]->X,m_openVec[i]->Y,m_openVec[i]->F);
                    if(m_openVec[i]->F < valueF)
                    {
                        valueF = m_openVec[i]->F;
                        idx = i;
                    }
                }
		printf("3 m_openVec selected idx: %d \n",idx);
		return m_openVec[idx];
        }
 
	bool RemoveFromOpenVec(CPoint* point)
	{
		for(POINTVEC::iterator it = m_openVec.begin(); it != m_openVec.end(); ++it)
		{
			if((*it)->X == point->X && (*it)->Y == point->Y)
			{
				m_openVec.erase(it);
				return true;
			}
		}
		return false;
	}
	
	bool canReach(int x, int y)
	{
		if(0 == m_array[x][y])
			return true;
		return false;
	}
 
	bool IsAccessiblePoint(CPoint* point, int x, int y, bool isIgnoreCorner)
	{
		if(!canReach(x, y) || isInCloseVec(x, y))
			return false;
		else
		{
			//可到达的点
			if(abs(x - point->X) + abs(y - point->Y) == 1)    // 左右上下点
				return true;
			else
			{
				if(canReach(abs(x - 1), y) && canReach(x, abs(y - 1)))   // 对角点
					return true;
				else
					return isIgnoreCorner;   //墙的边角
			}
		}
	}
 
	std::vector<CPoint*> GetAdjacentPoints(CPoint* point, bool isIgnoreCorner)
	{
		POINTVEC adjacentPoints;
		printf("4 current center point : (%d,%d) \n",point->X,point->Y);
		for(int x = point->X-1; x <= point->X+1; x++)
			for(int y = point->Y-1; y <= point->Y+1;  y++)
			{
				if(IsAccessiblePoint(point, x, y, isIgnoreCorner))
				{
					CPoint* tmpPoint = new CPoint(x, y);
					adjacentPoints.push_back(tmpPoint);
					
				}
			}
		
		return adjacentPoints;
	}
 
	bool isInOpenVec(int x, int y)
	{
		for(POINTVEC::iterator it = m_openVec.begin(); it != m_openVec.end(); it++)
		{
			if((*it)->X == x && (*it)->Y == y)
				return true;
		}
		return false;
	}
 
	bool isInCloseVec(int x, int y)
	{
		for(POINTVEC::iterator it = m_closeVec.begin(); it != m_closeVec.end(); ++it)
		{
			if((*it)->X == x && (*it)->Y == y)
				return true;
		}
		return false;
	}
	
	void RefreshPoint(CPoint* tmpStart, CPoint* point,CPoint* end)
	{
		int valueG = CalcG(tmpStart, point);
		int valueH = CalcH(end, point);
		point->G = valueG;
	    point->H = valueH;
		point->CalF();
		printf("7 point (%d,%d) finanlH:%d \n",point->X,point->Y,valueG +valueH);
		if((valueG +valueH)  <  (point->G + point->H ))
		{
			point->m_parentPoint = tmpStart;
			point->G = valueG;
			point->H = valueH;
			point->CalF();
		}
		usleep(500);
		
	}
	
	void NotFoundPoint(CPoint* tmpStart, CPoint* end, CPoint* point)
	{
		point->m_parentPoint = tmpStart;
		point->G = CalcG(tmpStart, point);
		point->G = CalcH(end, point);
		point->CalF();
		m_openVec.push_back(point);
	}
	
	int CalcG(CPoint* start, CPoint* point)//要么10 要么14
	{
		int G = (abs(point->X - start->X) + abs(point->Y - start->Y)) == 2 ? OBLIQUE : STEP ;
	//	printf("pointx (%d,%d) G:%d \n",point->X,point->Y,G);
		int parentG = point->m_parentPoint != NULL ? point->m_parentPoint->G : 0;
		return G + parentG;
	}
	
	int CalcH(CPoint* end, CPoint* point)//distance 
	{
		int step = abs(point->X - end->X)+ abs(point->Y - end->Y);
		//printf("pointx (%d,%d) step:%d \n",point->X,point->Y,step);
		return (STEP * step);
	}
 
	// 搜索路径
	CPoint* FindPath(CPoint* start, CPoint* end, bool isIgnoreCorner)
	{
		m_openVec.push_back(start);//加入开环中
		//printf("m_openVec.size() %d \n",m_openVec.size());
		while(0 != m_openVec.size())
		{
			CPoint* tmpStart = GetMinFPoint();   // 获取F值最小的点
			RemoveFromOpenVec(tmpStart);//从开环中移除
			m_closeVec.push_back(tmpStart);//加入闭环
			
			
			POINTVEC adjacentPoints = GetAdjacentPoints(tmpStart, isIgnoreCorner);
			printf("5 new adjacentPoints point push openvec : %d \n",adjacentPoints.size());
			for(POINTVEC::iterator it=adjacentPoints.begin(); it != adjacentPoints.end(); it++)
			{
				CPoint* point = *it;
			//	printf("point-> %d %d \n",point->X,point->Y);
				if(isInOpenVec(point->X, point->Y))   // 在开启列表
				   {
					printf("6 point isInOpenVec (%d,%d) \n",point->X, point->Y);
					RefreshPoint(tmpStart, point,end);
				}
				//else if(inCloseVec(point))
				//{
				// 检查节点的g值，如果新计算得到的路径开销比该g值低，那么要重新打开该节点（即重新放入OPEN集）		
				//}
				else
					NotFoundPoint(tmpStart, end, point);
			}
			
			if(isInOpenVec(end->X, end->Y)) // 目标点已经在开启列表中
			{
				for(int i=0; i < m_openVec.size(); ++i)
				{
					if(end->X == m_openVec[i]->X && end->Y == m_openVec[i]->Y)
						return m_openVec[i];
				}
			}
		}
		return end;
	}

 
};

void init_bgrcolor(struct bgr_color &green, struct bgr_color &red) {
green = {0, 10, 200, 255, 0, 10};
red = {0, 10, 0, 10, 100, 255};
}

void extract_color(Mat &img, Mat &start_img, Mat &end_img,
				   vector<vector<Point> > &start_points,
				   vector<vector<Point> > &end_points,
				   struct bgr_color &green, struct bgr_color &red) {

	vector<Vec4i> start_hierarchy, end_hierarchy;
	inRange(img, Scalar(green.b_low, green.g_low, green.r_low),
			Scalar(green.b_high, green.g_high, green.r_high), start_img);

	inRange(img, Scalar(red.b_low, red.g_low, red.r_low),
			Scalar(red.b_high, red.g_high, red.r_high), end_img);

	findContours(start_img, start_points, start_hierarchy,
				 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	findContours(end_img, end_points, end_hierarchy,
				 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
}

void display_images(Mat &img, Mat &start_img, Mat &end_img) {
	imwrite("Area.jpg", img);
	imwrite("Start.jpg", start_img);
	imwrite("End.jpg", end_img);
}
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


int main1(int argc ,char *argv[])
{
	
	int cellsize = 100 ;// #Set cell width and height in mm
	float scale = float(1)/cellsize;// #Set map scale
	
    int start_point_x= WIDTH/2;
    int start_point_y= HEIGHT/2;
    int goal_point_x= 22;
    int goal_point_y= 35;
	int array_img[WIDTH][HEIGHT];
    //float Map_log[300][300] = {0};
	
	 int thickness = int(cellsize * scale);
    int  SonarDist = int(4500 * scale);
	//创建一张图片  100*100的unsigned char 矩阵 都为0
	Mat img1(Map_Size, Map_Size, CV_8UC1, cv::Scalar(0));
	//Returns: Sonar_log - Array of log odds probabilities. 
	//This can be added to existing log odds occ map to update map with latest sonar data.
	double Sonar_log[Map_Size][Map_Size];
	Mat src = img1;//imread(argv[1]);
	Mat grayMaskSmallThresh;
	 if(src.empty())  
    {   
        if (!src.data) { 
            printf("Oh，no，读取图片文件错误~！ \n"); 
        } 
        cout << "error" << endl;
    } 
	 // 进行图像灰度化操作
   // cvtColor(src, src, CV_BGR2GRAY);
//	threshold(src, grayMaskSmallThresh, 230, 1, CV_THRESH_BINARY_INV);//反相选择 >230 =0,else= 1
    //获取 mat 的行和列
    int row = src.rows;//300
    int col = src.cols;//300
    cout << "  src.cols : " << src.cols << endl;//50 
    cout << "  src.rows : " << src.rows << endl;//50
   int thicknesss=-1;
    int LineType=8;
	printf("SonarDist:%d \n",SonarDist);
	ellipse(src,Point(int(Xr),int(Yr)),Size(SonarDist, SonarDist), -90, -15, 15, Scalar(255), thicknesss);
	//Mat dst;
	//resize(src, dst, cv::Size(1000, 1000));
	
	
	//创建一个mat用于存储新的坐标
	//std::vector<cv::Point2i> locations;   // output, locations of non-zero pixels 
	Mat wLocMat = Mat::zeros(src.size(),CV_8UC1); 
	//findNonZero(src, locations);
	findNonZero(src, wLocMat);
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
		printf("dist:%f %d %d\n",dist,SonarDist,thickness);
		if (dist > (SonarDist - thickness) && dist < (SonarDist + thickness))// #occupied region
		{
			Px = 0.5 + Ptheta;
            logPx = log(Px/(1-Px));
            Sonar_log[Yimg][Ximg] = logPx;
			//printf("logPx:%f \n",logPx);
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
	Mat img2 ;//= Mat(row, col);//图像img2：row*col大小  这里只是定义了img2图像的大小还没有传递图像的信息
	Mat Map_log = Mat(row, col, CV_64F,cv::Scalar(0.0));
	Mat Map_log2 = Mat(row, col, CV_64F,cv::Scalar(0.0));
	//for (int i = 0; i < row; i++)
	{
	//	ptmp = img2.ptr<uchar>(i);//指针指向img2的第i行
	//	for (int j = 0; j < col; j++)
		{
			//ptr[i][j] = src.at<unsigned char>(i, j);//src[][]  像素值传递
		//	ptmp[j] = Sonar_log[i][j];//二维数组数据传给img2的第i行第j列
		}
	}
	
	cv::Mat Aimg(Map_Size, Map_Size, CV_64F);
	std::memcpy(Aimg.data, Sonar_log, Map_Size*Map_Size*sizeof(double));
	cv::add(Aimg,Map_log,Map_log2);
	imshow("Map_log",Map_log2);
	waitKey(0);
	
	exp(Map_log2, Map_log2);
	 
	img2= (1 / ( 1 + Map_log2));
	imshow("Map_log",img2);
	waitKey(0);
    // 循环二维数组和mat，并将mat对应值赋给二维数组对应值，
  /*  for (int i = 0; i < row; i ++){
        for (int j = 0; j < col; j ++){
            array_img[i][j] = grayMaskSmallThresh.at<uchar>(i, j);
		//	printf("%d ",array_img[i][j]);
        }
    }
 
	*/
	/*
	Mat img_src, start_img, end_img;
    vector<vector<Point> > start_points, end_points;
	
    Point img_start, img_end;
     stack<Point> path;
    struct bgr_color green, red;
    printf("path:%s \n",argv[1]);
    img_src = imread(argv[1], CV_LOAD_IMAGE_COLOR);

    init_bgrcolor(green, red);
    extract_color(img_src, start_img, end_img, start_points, end_points,green, red);

    img_start = get_centre(start_points[0]);
	printf("img_start %d %d \n",img_start.x,img_start.y);
    img_end = get_centre(end_points[0]);
    printf("end %d %d \n",img_end.x,img_end.y);
    */
	//int (*La)[320] = array_img;
	#if 0
	CAStar* pAStar = new CAStar(array_img);
    if(array_img[start_point_x][start_point_y]||array_img[goal_point_x][goal_point_y])
    {
        cout<<"start point or goal point set error!!!"<<endl;
        return 0;
    }
    CPoint* start = new CPoint(start_point_x,start_point_y);
    CPoint* end = new CPoint(goal_point_x,goal_point_y);
    CPoint* point = pAStar->FindPath(start, end, false);
 
    Rect rect;
    Point left_up,right_bottom;
    
    Mat img(500,500,CV_8UC3,Scalar(255,255,255));
    //namedWindow("Test"); 
    
    while(point != NULL)
    {
        left_up.x = point->X*10;  //存储数组的列(point->Y)对应矩形的x轴，一个格大小50像素
        left_up.y = point->Y*10;  
        right_bottom.x = left_up.x+10;  
        right_bottom.y = left_up.y+10;
        rectangle(img,left_up,right_bottom,Scalar(0,255,255),CV_FILLED,8,0);//path yellow(full)
        std::cout << "(" << point->X << "," << point->Y << ");" << std::endl;
        point = point->m_parentPoint;
        
    }
 
    for(int i=0;i<WIDTH;i++)
    {
        for(int j=0;j<HEIGHT;j++)
        {   
            left_up.x = i*10; //存储数组的列(j)对应矩形的x轴
            left_up.y = j*10;  
            right_bottom.x = left_up.x+10;  
            right_bottom.y = left_up.y+10;
            if(array_img[i][j])
            {
                rectangle(img,left_up,right_bottom,Scalar(0,0,0),CV_FILLED,8,0);//obstacles BLACK
            }
            else
            {
                if(i==start_point_x&&j==start_point_y)
                    rectangle(img,left_up,right_bottom,Scalar(255,0,0),CV_FILLED,8,0);//start point blue(full)
                else if(i==goal_point_x&&j==goal_point_y)
                    rectangle(img,left_up,right_bottom,Scalar(0,0,255),CV_FILLED,8,0);//goal point RED(full)
                else
                    rectangle(img,left_up,right_bottom,Scalar(180,180,180),2,8,0);//free gray content,  edge
            }    
        }
    }
 
    
    imwrite("astar.jpg",img);   //窗口中显示图像 
	#endif
	return 0;
    
    
}
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