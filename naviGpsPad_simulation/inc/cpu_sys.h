#ifndef CPU_SYS_H
#define CPU_SYS_H


#ifdef __cplusplus
extern "C" {
#endif

extern double cpuPercentage ;
extern int cpuTemperature ;
extern int wifiSignalStrength ;

 

extern void *getCPUPercentageThread(void *);
extern char  read_ID_fromSn(char *sn);


#ifdef __cplusplus
}
#endif


#endif
