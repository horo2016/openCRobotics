#ifndef MOVE2_POSE_H
#define MOVE2_POSE_H 

#ifdef __cplusplus
extern "C" {
#endif
typedef struct{
int x;
int y;
int yaw;
}Pose;



extern int moveFollow();




extern unsigned long get_micros();



#ifdef __cplusplus
}
#endif
#endif
