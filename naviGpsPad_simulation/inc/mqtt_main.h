#ifndef MQTT_MAIN_H
#define  MQTT_MAIN_H


#ifdef __cplusplus
extern "C" {
#endif

extern void *Mqtt_ClentTask(void *argv);
extern void *Mqtt_PublishTask(void *argv);

extern int get_value_from_cmdline( char* buf, char *key, char *value);

extern void User_MsgContl(char * msg);


extern char chargename[17];
#ifdef __cplusplus
}
#endif

#endif
