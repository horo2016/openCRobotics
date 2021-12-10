#ifndef SENSOR_MODEL_H
#define SENSOR_MODEL_H
//#include <opencv2/opencv.hpp>
//#ifdef __cplusplus
//extern "C" {
//#endif
extern double **SonarModel(cv::Mat occmap,int Xr,int Yr,int Rangle,int SonarDist,int cellsize,float scale);
extern int SonarModelArray(cv::Mat occmap,int Xr,int Yr,int Rangle,int SonarDist,int cellsize,float scale);
extern int LidarModelArray(cv::Mat occmap,int Xr,int Yr,int Rangle,int SonarDist,int cellsize,float scale);
//#ifdef

//#ifdef __cplusplus
//}
//#endif
#endif
