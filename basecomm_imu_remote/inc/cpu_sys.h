#ifndef CPU_SYS_H
#define CPU_SYS_H


#ifdef __cplusplus
extern "C" {
#endif

extern double cpuPercentage ;
extern int cpuTemperature ;
extern int wifiSignalStrength ;

extern char chargename[17];

extern void *getCPUPercentageThread(void *);


#ifdef __cplusplus
}
#endif


#endif
