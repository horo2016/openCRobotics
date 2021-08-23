#ifndef MQTT_MAIN_H
#define  MQTT_MAIN_H


#ifdef __cplusplus
extern "C" {
#endif

extern void *Mqtt_ClentTask(void *argv);
extern void *Mqtt_PublishTask(void *argv);




#ifdef __cplusplus
}
#endif

#endif
