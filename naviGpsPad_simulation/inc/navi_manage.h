#ifndef NAVI_MANAGE_h
#define NAVI_MANAGE_h

#ifdef __cplusplus
extern "C" {
#endif


extern char  GLOBAL_STATUS;
extern char  GLOBAL_SWITCH;
#define STANDBY_STATUS 1
#define ROTATE_STATUS 2
#define MOVE_STATUS  3
#define AVOIDOBJ_STATUS 4
#define WAYPOINTARRIVE_STATUS 5
#define STOP_STATUS 6
#define MANUAL_STATUS 7
#define CACULATE_STATUS 8

#define SUBSUMPTION_INTERVAL 200
#define DIS_BLINK_INTERVAL 500
#define CALCULATE_GPS_HEADING_INTERVAL 1000



#define MAX_SPEED     0.5
#define SPEED_RESO    0.05

extern char  GLOBAL_STATUS ;
extern int currentWaypoint ;

extern double waypointlongitude;
extern double waypointlatitude;


extern double targetHeading;


extern double waypointRange ;
extern //单位是度
int car_heading_simulation ;
//单位是m
extern
float car_run_simulation ;

extern float car_speed_from_web;

extern  unsigned long getmillis();
extern   void RotateDegrees(int degrees);
extern    void RotateDegreesByManual(int degrees);
extern   void MoveDistance(int meters);
extern void MoveDistanceDwa(int meters);

extern  void *navimanage_handle (void *arg);

#ifdef __cplusplus
}
#endif

#endif
