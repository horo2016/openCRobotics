#ifndef IMU_H
#define IMU_H


#ifdef __cplusplus
extern "C" {
#endif

extern void *IMUThread(void *);
extern float heading ;
extern float rollAngle ;
extern float pitchAngle  ;

extern float courseHeading;

#ifdef __cplusplus
}
#endif

#endif

