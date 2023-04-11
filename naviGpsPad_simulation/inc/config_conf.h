#ifndef _CONFIG_CONF_H
#define _CONFIG_CONF_H

#ifdef __cplusplus
extern "C" {
#endif


#define	VIDEO_PICTUTE_DEFAULT_STORAGE_PATH		"/etc/tracklogpath.conf"
#define TRACKLOG_NAME_NUMER						"/etc/tracklogname.conf"
#define	TRACKLOG_STORAGE_DEFAULT_KEY			"track_path"
#define	TRACKLOG_NAME_DEFAULT_KEY				"track_name"

#define	TRACKLOG_STORAGE_DEFAULT_PATH			"/home/linaro/media/user/tracklog"
#define TRACKLOG_NAME_DEFAULT					"1000001"

#define VERSION_FILE_PATH						"/etc/system.prop"
#define SOFTWARE_KEY							"ro.software.version"
#define HARDWARE_KEY							"ro.hardware.version"
#define SYSTEM_UTC_KEY							"ro.build.date.utc"

#define IMAGE_PATH								"/etc/http_server.conf"
#define IMAGE_PASSWD							"/etc/ota_passwd.conf"
#define IMAGE_PATH_KEY							"UPGRADE_PATH"
#define IMAGE_PASSWD_KEY						"ota_passwd"
#define IMAGE_NAME								"upgrade_package.des3"
#define DIR_NAME								"upgrade_package"
#define IMAGE_PACK_NAME							"eagle8074-ota.zip"
#define SHA256_NAME								"sha256value"
#define PACKAGE_VERSION							"system/build.prop"

#define MAC_ADDR_FILE							"/sys/class/net/wlan0/address"
#define HOSTAPD_CONF							"/etc/hostapd.conf"
#define HOSTAPD_TMP								"/etc/.hostapd.tmp"

#define TIMEOUT_PATH							"/etc/conection.conf"
#define TIMEOUT_KEY								"DisConnectionTime"

#define SERVER_IP    "47.94.222.103" 
extern void get_tracklog_storage_path(const char *path,char *tracklog_path);
extern void get_tracklog_name(const char *config_path,int *name) ;
extern int get_value_from_config_file(const char* file, char *key, char *value);
extern int is_directory_exist(const char *path);
extern int create_directory(const   char   *path);
extern int is_tracklog_exist(const char *path) ; 
extern int create_tracklog(const char	*path) ; 
extern int get_version(int *hw_len,char *hw_version,int *sw_len,char *sw_version) ;
extern int get_mac(char *addr) ;
extern int modify_wifi_conf(char *ssid,int ssid_len,char *passwd,int passwd_lenn) ;
extern void check_wifi_conf() ;
extern int check_hostapd_conf() ;
extern int get_strchr_len(char *str, char c);



#ifdef __cplusplus
}
#endif
#endif

