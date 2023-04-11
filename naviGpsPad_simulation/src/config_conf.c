/******************************************************************************
 * Copyright (c) 2007-2016, ZeroTech Co., Ltd.
 * All rights reserved.
 *******************************************************************************
 * File name     : config_conf.cpp
 * Description   : config file parsing
 * Version       : v1.0
 * Create Time   : 2016/2/29
 * Author   		: ZhangDanfeng
 * Modify history:
 *******************************************************************************
 * Modify Time   Modify person  Modification
 * ------------------------------------------------------------------------------
 *2016/04/19	  wangbo
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>


#include "config_conf.h"
#include "osp_syslog.h"


#define CONFIGFILE_NAME_TMP			"/home/pi/app/control_engine/sys_config.tmp"
unsigned char head[] = {
"#this config file is to configure tracklog storage path\n" \
"#the config item is key=value format\n"
"#note,there is not space arround =\n"
"#each line is end by '\\n' \n"
"#\n\n"
};

/*******************************************************************************
 * function name	: is_file_exist
 * description	: set video and picture path of storage
 * param[in] 	: path-file path
 * param[out] 	: none
 * return 		: 0-exist, -1-not exist
 *******************************************************************************/
int is_file_exist(const char *path)
{
	if(path == NULL)
		return -1;

	if(0 == access(path, F_OK))
		return 0;

	return -1;
}

/*******************************************************************************
 * function name	: is_directory_exist
 * description	: To judge whether a directory exists
 * param[in] 	: path-directory path
 * param[out] 	: none
 * return 		: 0-exist, -1-not exist
 *******************************************************************************/
int is_directory_exist(const char *path)
{
	if(path == NULL)
		return -1;

	if(opendir(path) == NULL)
		return -1;

	return 0;
}

/*******************************************************************************
 * function name	: create_directory
 * description	: create directory
 * param[in] 	: path-directory path
 * param[out] 	: none
 * return 		: 0-exist, -1-not exist
 *******************************************************************************/
int create_directory(const   char   *path)
{
	char   dir_name[256];

	strcpy(dir_name, path);
	int   i,len   =   strlen(dir_name);
	if(dir_name[len-1] != '/')
		strcat(dir_name, "/");

	len = strlen(dir_name);

	for(i=1; i<len; i++) {
		if(dir_name[i]=='/'){
			dir_name[i]   =   0;
			if(access(dir_name, 0) != 0 ){
				if(mkdir(dir_name, 0777) == -1){
					perror("mkdir   error");
					return -1;
				}
			}
			dir_name[i]   =   '/';
		}
	}

	return   0;
}



/*******************************************************************************
 * function name	: get_strchr_len
 * description	: Get the migration length of the characters in a string
 * param[in] 	: str-string, c-char
 * param[out] 	: none
 * return 		: >0-len, -1-fail
 *******************************************************************************/
int get_strchr_len(char *str, char c)
{
	int i = 0;
	int len = 0;

	if(!str)
		return -1;

	while(*(str+i++) != c)
		len ++;

	return len;
}


/*******************************************************************************
 * function name	: get_value_from_config_file
 * description	: get value from config
 * param[in] 	: file-config file, key-dest string, value-key value
 * param[out] 	: none
 * return 		: 0-exist, -1-not exist
 *******************************************************************************/
int get_value_from_config_file(const char* file, char *key, char *value)
{
	int value_len = 0 ;
	FILE *fp = NULL ;
	char *buf = NULL ;
	char *sub_str = NULL ;
	size_t line_len = 0 ;
	size_t len = 0;


	fp = fopen(file, "r");
	if(fp == NULL){
		perror("open config file fail");
		return -1;
	}

	while((line_len = getline(&buf, &len, fp)) != (size_t)-1){
		if(buf == NULL)
			continue ;

		if(*buf == '#'){
			free(buf) ;
			buf = NULL ;
			continue;
		}
		//printf("zdf config file : %s\n", buf);
		sub_str = strstr(buf, key);

		if(sub_str != NULL){//video="/home/linaro/video"
			sub_str += (strlen(key) + 1); //=" length is 2 bytes
			//printf("zdf sub_str : %s\n", sub_str);
			value_len = get_strchr_len(sub_str, '\n');
			//printf("zdf value len = %d\n", value_len);
			memcpy(value, sub_str, value_len);
			fclose(fp);
			free(buf) ;
			buf = NULL ;
			return 0;
		}
		if(buf != NULL){
			free(buf) ;
			buf = NULL ;
		}
	}
	fclose(fp);

	return -1;
}

/*******************************************************************************
 * function name	: set_value_from_config_file
 * description	: get value from config
 * param[in] 	: file-config file, key-dest string, value-key value
 * param[out] 	: none
 * return 		: 0-exist, -1-not exist
 *******************************************************************************/
int set_value_from_config_file(const char* file, char *key, char *value)
{
    FILE *fp = NULL ;
	FILE *ftmp = NULL ;
	char *buf = NULL ;
	char *sub_str = NULL ;
	size_t line_len = 0 ;
	size_t len = 0;
	int ret = 0 ;
	int flag = 0 ;



	fp = fopen(file, "r");
	if(fp == NULL){
		DEBUG(LOG_ERR,"open %s fail\n",file);
		return -1;
	}
	ftmp = fopen(CONFIGFILE_NAME_TMP,"w");
	if(ftmp == NULL){
		DEBUG(LOG_ERR,"open %s failed\n",CONFIGFILE_NAME_TMP) ;
		return -1 ;
	}

	while((line_len = getline(&buf, &len, fp)) != (size_t)-1){
		if(buf == NULL)
			continue ;

		if(*buf == '#'){
			fprintf(ftmp,"%s",buf) ;
			free(buf) ;
			buf = NULL ;
			continue;
		}
		
		sub_str = strstr(buf, key);
		if((sub_str != NULL)&&(sub_str - buf == 0 )){
			fprintf(ftmp,"%s=%s\n",key,value) ;
			flag =1 ;
		}
		
		if(flag == 0){
			fprintf(ftmp,"%s",buf) ;
		}
		flag = 0 ;

		if(buf != NULL){
			free(buf) ;
			buf = NULL ;
		}
	}

	fclose(fp);
	fclose(ftmp) ;

	//renmae will remove HOSTAPD_CONF first ,and rename old file name
	ret = rename(CONFIGFILE_NAME_TMP,file) ;
	if(ret < 0 ){
		DEBUG(LOG_ERR,"rename configure file failed\n") ;
		return ret ;
	}else {
     DEBUG(LOG_DEBUG,"rename configure file success \n") ;
	}
	
/*	ret = remove(CONFIGFILE_NAME_TMP) ;
	if(ret < 0 ){
		DEBUG(LOG_ERR,"remove %s failed ,ignore this warning\n",CONFIGFILE_NAME_TMP) ;
	}
*/
	return 0 ;
}
/*******************************************************************************
 * function name	: set_default_config_file
 * description	: set config file default value
 * param[in] 	: path-storage path
 * param[out] 	: none
 * return 		: 0-success, -1-fail
 *******************************************************************************/
int set_default_config_file(const char *path)
{
	int fd = 0 ;
	int path_len = 0 ;
	int w_len = 0 ;
	char tmp[512] = {0};

	if(path == NULL){
		DEBUG(LOG_ERR,"path is null ,return \n") ;
		return -1 ;
	}

	fd = open(path, O_WRONLY | O_CREAT,0777);
	if(fd < 0){
		DEBUG(LOG_ERR,"video picture storage path config file open fail");
		return -1;
	}

	path_len = strlen((char*)head);
	//printf("zd tmp len %d : %s\n", video_path_len, tmp);
	w_len = write(fd, head, path_len);
	if(w_len != path_len){
		perror("video storage default path write head fial");
		//close(fd);
		//return -1;
	}

	sprintf(tmp, "%s%c%s%c", TRACKLOG_STORAGE_DEFAULT_KEY, '=', TRACKLOG_STORAGE_DEFAULT_PATH, '\n');
	path_len = strlen(tmp);
	//printf("zd tmp len %d : %s\n", video_path_len, tmp);
	w_len = write(fd, tmp, path_len);
	if(w_len != path_len){
		DEBUG(LOG_ERR,"video storage default path write fial");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}
/*******************************************************************************
 * function name	: set_default_tracklog_name_file
 * description	: set config file default tracklog name number
 * param[in] 	: path-storage path
 * param[out] 	: none
 * return 		: 0-success, -1-fail
 *******************************************************************************/
int set_default_tracklog_name_file(const char *path)
{
	int fd = 0 ;
	int path_len = 0 ;
	int w_len = 0 ;
	char tmp[512] = {0};

	if(path == NULL){
		DEBUG(LOG_ERR,"[set_default_config_file]:path is null ,return \n") ;
		return -1 ;
	}

	fd = open(path, O_WRONLY | O_CREAT,0777);
	if(fd < 0){
		DEBUG(LOG_ERR,"video picture storage path config file open fail");
		return -1;
	}

	path_len = strlen((char*)head);
	//printf("zd tmp len %d : %s\n", video_path_len, tmp);
	w_len = write(fd, head, path_len);
	if(w_len != path_len){
		DEBUG(LOG_ERR,"video storage default path write head fial");
		//close(fd);
		//return -1;
	}

	sprintf(tmp, "%s%c%s%c", TRACKLOG_NAME_DEFAULT_KEY, '=', TRACKLOG_NAME_DEFAULT, '\n');
	path_len = strlen(tmp);
	//printf("zd tmp len %d : %s\n", video_path_len, tmp);
	w_len = write(fd, tmp, path_len);
	if(w_len != path_len){
		DEBUG(LOG_ERR,"video storage default path write fial");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}
/*******************************************************************************
 * function name	: modify_tracklog_name_file
 * description	: modify tracklog name number
 * param[in] 	: path-storage path
 * param[out] 	: none
 * return 		: 0-success, -1-fail
 *******************************************************************************/
int modify_tracklog_name_file(const char *path,int num)
{
	int fd = 0 ;
	int path_len = 0 ;
	int w_len = 0 ;
	char tmp[512] = {0};

	if(path == NULL){
		DEBUG(LOG_ERR,"[set_default_config_file]:path is null ,return \n") ;
		return -1 ;
	}

	fd = open(path, O_WRONLY | O_CREAT,0777);
	if(fd < 0){
		DEBUG(LOG_ERR,"video picture storage path config file open fail");
		return -1;
	}

	path_len = strlen((char*)head);
	//printf("zd tmp len %d : %s\n", video_path_len, tmp);
	w_len = write(fd, head, path_len);
	if(w_len != path_len){
		perror("video storage default path write head fial");
		//close(fd);
		//return -1;
	}

	sprintf(tmp, "%s%c%d%c", TRACKLOG_NAME_DEFAULT_KEY, '=', num, '\n');
	path_len = strlen(tmp);
	//printf("zd tmp len %d : %s\n", video_path_len, tmp);
	w_len = write(fd, tmp, path_len);
	if(w_len != path_len){
		DEBUG(LOG_ERR,"video storage default path write fial");
		close(fd);
		return -1;
	}

	close(fd);

	return 0;
}


/*******************************************************************************
 * function name	: get_tracklog_storage_path
 * description	: set traclog path of storage
 * param[in] 	: config file path
 * param[out] 	:
 * return 		: none
 *******************************************************************************/
void get_tracklog_storage_path(const char *config_path,char *tracklog_path)
{
	char value[128] = {0};
	int ret = 0 ;


	if(tracklog_path == NULL){
		DEBUG(LOG_ERR,"tracklog_path is null ,cannot create tracklog path ,exit \n") ;
		return ;
	}

	if(config_path == NULL){
		DEBUG(LOG_ERR,"config path is null ,cannot get tracklog path ,default path :'./' \n") ;
		strcpy(tracklog_path , ".") ;
		return ;
	}

	if(0 == is_file_exist(config_path)){
		//printf("zdf key : %s\n", VIDEO_KEY);
		ret = get_value_from_config_file(config_path, TRACKLOG_STORAGE_DEFAULT_KEY, value);
		if(0 == ret){
			//printf("zdf key %s=%s\n", VIDEO_KEY, value);
			if(is_directory_exist(value) < 0){
				create_directory(value);
			}

			strcpy(tracklog_path,value) ;

		}
	}
	else{
		set_default_config_file(config_path);

		if(is_directory_exist(TRACKLOG_STORAGE_DEFAULT_PATH) < 0){
			create_directory(TRACKLOG_STORAGE_DEFAULT_PATH);
		}
		strcpy(tracklog_path, TRACKLOG_STORAGE_DEFAULT_PATH) ;

	}
}
/*******************************************************************************
 * function name	: get_tracklog_name
 * description	: set traclog name
 * param[in] 	: config file path
 * param[out] 	:
 * return 		: none
 *******************************************************************************/
void get_tracklog_name(const char *config_path,int *name)
{
	char value[128] = {0};
	int ret = 0 ;

	if(name == NULL){
		DEBUG(LOG_ERR,"name is null ,cannot create tracklog path ,exit \n") ;
		return ;
	}

	if(config_path == NULL){
		DEBUG(LOG_ERR,"config path is null ,cannot get tracklog path ,default path :'./' \n") ;
		*name = 1000000 ;
		return ;
	}

	if(0 == is_file_exist(config_path)){
		ret = get_value_from_config_file(config_path, TRACKLOG_NAME_DEFAULT_KEY, value);
		if(0 == ret){
			*name = atoi(value) ;
		}
		if(*name >= 9999999){
			modify_tracklog_name_file(config_path,1000000 ) ;
		}
		else{
			modify_tracklog_name_file(config_path,*name + 1 ) ;
		}
	}
	else{
		set_default_tracklog_name_file(config_path) ;
		*name = 1000000;
	}
}

/*******************************************************************************
* function name	: get_version
* description	: get hardware and software version
* param[in] 	: none
* param[out] 	: hardware length and hardware version and software length and software version
* return 		: 0:success ;-1 failed
*******************************************************************************/
int get_version(int *hw_len,char *hw_version,int *sw_len,char *sw_version)
{
	char sw_ver[0xff] = "" ;
	char hw_ver[0xff] = "" ;
	char software_len = 0 ;
	char hardware_len = 0 ;
	int ret = 0 ;

	if((hw_len == NULL)||(hw_version == NULL)||(sw_len == NULL)||(sw_version == NULL)){
		DEBUG(LOG_ERR,"buf is null ,cannot get version \n") ;
		return -1 ;
	}

	ret = get_value_from_config_file(VERSION_FILE_PATH,SOFTWARE_KEY,sw_ver) ;
	if(ret == -1){
		DEBUG(LOG_ERR,"get software version from config failed \n") ;
		return -1 ;
	}

	ret = get_value_from_config_file(VERSION_FILE_PATH,HARDWARE_KEY,hw_ver) ;
	if(ret == -1){
		DEBUG(LOG_ERR,"get hardware version from config failed \n") ;
		return -1 ;
	}
	software_len = strlen(sw_ver) ;
	if(software_len == 0){
		DEBUG(LOG_ERR,"the length of software version is 0,failed \n") ;
		return -1 ;
	}

	hardware_len = strlen(hw_ver) ;
	if(hardware_len == 0){
		DEBUG(LOG_ERR,"the length of hardware version is 0,failed \n") ;
		return -1 ;
	}

	DEBUG(LOG_DEBUG,"software version:%s \n",sw_ver) ;
	DEBUG(LOG_DEBUG,"hardware version:%s \n",hw_ver) ;
	memcpy(sw_version,sw_ver,software_len) ;
	memcpy(hw_version,hw_ver,hardware_len) ;

	*sw_len = software_len ;
	*hw_len = hardware_len ;

	return 0 ;
}

/*******************************************************************************
* function name	: get_mac
* description	: get wifi mac address
* param[in] 	: none
* param[out] 	: mac
* return 		: 0:success ;-1 failed
*******************************************************************************/
int get_mac(char *addr)
{
	FILE  *addr_file = NULL ;
	char str_mac[50] = { 0 } ;
	char *p = NULL ;

	if(addr == NULL){
		DEBUG(LOG_ERR,"addr is null ,return \n") ;
		return -1 ;
	}

	addr_file = fopen(MAC_ADDR_FILE,"r") ;
	if(addr_file == NULL){
		DEBUG(LOG_ERR,"open file :%s failed ,return \n",MAC_ADDR_FILE) ;
		return -1 ;
	}

	fgets(str_mac,49,addr_file) ;

	if(strlen(str_mac) == 0 ){
		DEBUG(LOG_ERR,"read buff len == 0 ,read failed ,reutrn \n") ;
		fclose(addr_file) ;
		return -1;
	}
	fclose(addr_file) ;
	p = str_mac ;
	DEBUG(LOG_DEBUG,"mac address :%s \n",str_mac) ;
	while(*p != 0){
		if(*p == ':')
			*p = '-' ;
		p++ ;
	}
	memcpy(addr,str_mac,strlen(str_mac)) ;
	return 0 ;
}
/*******************************************************************************
* function name	: modify_wifi_conf
* description	: modify wifi config file ,write new ssid and new password to config file
* param[in] 	: none
* param[out] 	: none
* return 		: 0-success,-1-fail
*******************************************************************************/
int modify_wifi_conf(char *ssid,int ssid_len,char *passwd,int passwd_len)
{
	FILE *fp = NULL ;
	FILE *ftmp = NULL ;
	char *buf = NULL ;
	char *sub_str = NULL ;
	size_t line_len = 0 ;
	size_t len = 0;
	int ret = 0 ;
	int flag = 0 ;

	if((ssid == NULL)||(passwd == NULL)){
		DEBUG(LOG_ERR,"ssid or password is NULL,modify failed,will return \n") ;
		return -1 ;
	}

	fp = fopen(HOSTAPD_CONF, "r");
	if(fp == NULL){
		DEBUG(LOG_ERR,"open %s fail\n",HOSTAPD_CONF);
		return -1;
	}
	ftmp = fopen(HOSTAPD_TMP,"w");
	if(ftmp == NULL){
		DEBUG(LOG_ERR,"open %s failed\n",HOSTAPD_TMP) ;
		return -1 ;
	}

	while((line_len = getline(&buf, &len, fp)) != (size_t)-1){
		if(buf == NULL)
			continue ;

		if(*buf == '#'){
			fprintf(ftmp,"%s",buf) ;
			free(buf) ;
			buf = NULL ;
			continue;
		}
		if(ssid_len!=0){
		sub_str = strstr(buf, "ssid=");
		if((sub_str != NULL)&&(sub_str - buf == 0 )){
			fprintf(ftmp,"ssid=%s\n",ssid) ;
			flag =1 ;
		}
		}
		if(passwd_len!=0){
		sub_str = strstr(buf, "wpa_passphrase=");
		if(sub_str != NULL){
			fprintf(ftmp,"wpa_passphrase=%s\n",passwd) ;
			flag = 1 ;
		}
		}

		if(flag == 0){
			fprintf(ftmp,"%s",buf) ;
		}
		flag = 0 ;

		if(buf != NULL){
			free(buf) ;
			buf = NULL ;
		}
	}

	fclose(fp);
	fclose(ftmp) ;

	//renmae will remove HOSTAPD_CONF first ,and rename old file name
	ret = rename(HOSTAPD_TMP,HOSTAPD_CONF) ;
	if(ret < 0 ){
		DEBUG(LOG_ERR,"rename configure file failed\n") ;
		return ret ;
	}
	ret = remove(HOSTAPD_TMP) ;
	if(ret < 0 ){
		DEBUG(LOG_ERR,"remove %s failed ,ignore this warning\n",HOSTAPD_TMP) ;
	}

	return 0 ;
}

void check_wifi_conf()
{
	FILE *fconf = NULL;
	int ret = 0 ;

	fconf = fopen(HOSTAPD_CONF,"r") ;
	if(fconf == NULL){
		DEBUG(LOG_ERR,"%s is not exit\n",HOSTAPD_CONF) ;
		ret = rename(HOSTAPD_TMP,HOSTAPD_CONF) ;
		if(ret < 0 ){
			DEBUG(LOG_ERR,"rename configure file failed\n") ;
		}
		else{
			system("ifdown wlan0") ;
			usleep(500000) ;
			system("ifup wlan0") ;
		}
	}
	else{
		fclose(fconf) ;
	}
}

int check_hostapd_conf()
{
	struct stat buf;
	int ret = 0 ;

	DEBUG(LOG_TRACE,"check ota is first boot or not \n") ;
	ret = stat("/home/linaro/otafirstboot.txt", &buf);
	if((ret < 0)&&(errno == ENOENT)){
		DEBUG(LOG_DEBUG,"/home/linaro/otafirstboot.txt is not exist \n") ;
		return  1 ;
	}
	remove("/home/linaro/otafirstboot.txt") ;
	DEBUG(LOG_INFO,"found otafirstboot file,check hostapd.conf\n") ;

	DEBUG(LOG_TRACE,"mount /dev/mmcblk0p17 /cache\n") ;
	system("mount /dev/mmcblk0p17 /cache") ;

	ret = stat("/cache/hostapd.conf", &buf);
	if((ret < 0)&&(errno == ENOENT)){
		DEBUG(LOG_DEBUG,"/cache/hostapd.conf is not exist \n") ;
		return  1 ;
	}

	DEBUG(LOG_DEBUG,"ota first boot,and found /cache/hostapd.conf,reset hostapd.conf and restart wifi\n") ;
	system("cp /cache/hostapd.conf /etc/ -rf") ;
	remove("/cache/hostapd.conf") ;

	system("ifdown wlan0 ") ;
	usleep(500000) ;
	system("ifup wlan0") ;

	return 0 ;
}
