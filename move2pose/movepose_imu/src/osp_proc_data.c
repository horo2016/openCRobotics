/******************************************************************************
* Copyright (c) 2015 - 2015, Thunder Software Technology Co.,Ltd.
* All rights reserved.
*******************************************************************************
* File name     :
* Function      :
* Version       : v1.0
* Create Time   : 2015/6/18
* Discription   :
* Modify history:
*******************************************************************************
* Modify Time   Modify person  BugID/CRID     content
* ------------------------------------------------------------------------------
*
*******************************************************************************/

/******************************* include *********************************/
#include "osp_common.h"

#include "osp_syslog.h"



app_task_attr task_table[MAX_APP_TASK_NUM] = {
	{
		MOD_ID_CEMSGMNG,
		LOOP_MODE ,
		0,
		NULL,
		NULL,
		0,
		APP_TSK_DEFAULT_PRIO//APP_TSK_DEFAULT_PRIO
	},
	
	{
		MOD_ID_GCS,
		 CAPTURE_MODE,
		0,
		NULL,
		NULL,
		0,
		APP_TSK_DEFAULT_PRIO
	},
	{
		MOD_ID_OM,
		HEARTBEAT_MODE,
		0,
		NULL,
		NULL,
		10,//5s
		APP_TSK_DEFAULT_PRIO
	},

	{
		MOD_ID_TRACKLOG,
		SOCKET_UDP,
		0,
		NULL,
		NULL,
		48,//ms
		APP_TSK_DEFAULT_PRIO
	},
#ifdef FC_CONTROL_ENABLED
	{
		MOD_ID_FC_CMD,
		LOOP_MODE,
		0,
		NULL,
		NULL,
		200,//ms
		APP_TSK_DEFAULT_PRIO
	},
#endif

#if 0
	{
		MOD_ID_WIFI,
		LOOP_MODE,
		0,
		check_wifi_init,
		check_wifi_main,
		2000,//ms
		APP_TSK_DEFAULT_PRIO}
	,
#endif


	{
		-1,
		-1,
		-1,
		0,
		0,
		-1
	},
};


