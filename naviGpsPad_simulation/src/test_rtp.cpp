/**********************************************************************
Copyright (C) 2015 CASKY eTech Co., Ltd .
------------------------------------------------------------
Project    : UAV
************************************************************************
   File Name	: main.cpp
   Module Name	: test code to run rtp sender
   Description	: 
   History:
  No.		Ver.		Date		Designed by		Comment
-----+----------+-----------------+--------------+-----------------
   1		2015.07.24		CASKY			new created

************************************************************************/

#include "base_rtp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define DEBUG
#ifdef DEBUG
#define debug_info(fmt, args...) \
	fprintf(stderr, fmt, ##args)
#else
#define debug_info(fmt, args...) 
#endif

/*RTP Info*/
typedef struct {
	char strm[100];
	char cDest_ip[20];
	unsigned int ulDest_port;
	unsigned int eEnc_fmt;
	unsigned int ulFrmrate;
}sRTP_INFO;

/* return */
typedef enum {
	RTP_RET_SUCCESS=0,
	RTP_RET_FAIL=-1,
} eBASE_RET_STATE;

/*for test*/
#define DEST_IP_ADDRESS "192.168.0.100"
#define DEST_PORT 7078
#define ENCODE_FRAMERATE 30 //30fps,24fps
#define H264_FRAME_BUFFER_SIZE 524288 //512KB

static unsigned char BUFFER[H264_FRAME_BUFFER_SIZE]; //rtp send buffer
static int LastCmd = 0; //no
static sRTP_INFO srtpinfo;
pthread_t pid = 0;


/***************************************************************************
  Function		: vHelpinfo( )
  Description	: help info
  INput		: NULL
  OUTput		: NULL
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static void
vHelpinfo()
{
	printf("Usage: \n");
	printf("    -i: destination ip address. (default: 172.21.0.106)\n");
	printf("    -p: destination port.       (default: 7078)\n");
	printf("    -e: encoder format.         (default: 0 ->H264 ES)\n");
	printf("    -f: frame rate.             (default: 30)\n");
	printf("    -s: Input stream name.      (default: H264.es)\n");
	printf("    -h: help info.              \n");
	printf("\n");
}

/***************************************************************************
  Function		: vGetParam( )
  Description	: get param
  INput		: char* pstrm			: store input stream name
  			   sRTP_INFO* pinfo	: store sRTP_INFO info
  			   int argc				: input args number
  			   char* argv[]		: input args data
  OUTput		: NULL
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static void
vGetParam(int argc, char* argv[])
{
	strcpy(srtpinfo.strm,"output.h264");
	strcpy(srtpinfo.cDest_ip,DEST_IP_ADDRESS);
	srtpinfo.ulDest_port = DEST_PORT;
	srtpinfo.eEnc_fmt = 0;
	srtpinfo.ulFrmrate = ENCODE_FRAMERATE;

	for(int i=0; i< argc;i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'i':
				case 'I':
					if(argv[i][2] != 0)
						strcpy(srtpinfo.cDest_ip, &argv[i][2]);
					break;
				case 'p':
				case 'P':
					if(argv[i][2] != 0)
						srtpinfo.ulDest_port = atoi(&argv[i][2]);
					break;
				case 'e':
				case 'E':
					if(argv[i][2] != 0)
						srtpinfo.eEnc_fmt = atoi(&argv[i][2]);
					break;
				case 'f':
				case 'F':
					if(argv[i][2] != 0)
						srtpinfo.ulFrmrate = atoi(&argv[i][2]);
					break;
				case 's':
				case 'S':
					if(argv[i][2] != 0)
						strcpy(srtpinfo.strm,&argv[i][2]);
					break;
				case 'h':
				case 'H':
					vHelpinfo();
					exit(1);
					break;
				default:
					printf("ERROR: Input undefined parameter! \n");
					vHelpinfo();
					exit(1);
					break;
			}
		}
	}
	
	printf("    destination ip address: %s\n",srtpinfo.cDest_ip);
	printf("    destination port.     : %d\n",srtpinfo.ulDest_port);
	printf("    encoder format.       : %d\n",srtpinfo.eEnc_fmt);
	printf("    frame rate.           : %d\n",srtpinfo.ulFrmrate);
	printf("    Input stream name.    : %s\n",srtpinfo.strm);
	printf("\n");
}

/***************************************************************************
  Function		: lGetFrmlen( )
  Description	: get frame size
  INput		: unsigned int ulcnt			: total byte of stream
  			   unsigned int* puloffest	: offset to the stream beginning
  			   FILE* fp				: input file pointer
  OUTput		: int					: return frame size
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static int 
lGetFrmlen(unsigned int ulcnt, unsigned int* puloffest, FILE* fp)
{
	unsigned int uloffest = *puloffest;
	unsigned int ulleft = ulcnt - uloffest;
	unsigned int loadsize = 0;
	int llen = 0;

	fseek(fp, uloffest, SEEK_SET);
	//debug_info("Left size : %u! \n", ulleft);
	
	if(ulleft >= H264_FRAME_BUFFER_SIZE)
	{
		loadsize = H264_FRAME_BUFFER_SIZE;
	}else{
		loadsize = ulleft;
	}
	fread((void*)BUFFER, 1, loadsize, fp);

	/*check current syncword*/
	unsigned char *pBUFFER = BUFFER;
	if((pBUFFER[0] == 0x00) && (pBUFFER[1] == 0x00)
		&& (pBUFFER[2] == 0x00) && (pBUFFER[3] == 0x01))//0x00000001
	{
		//debug_info("Find current frame syncword: 0x00000001 \n");
		pBUFFER += 4;
		llen +=4;
	}else if((pBUFFER[0] == 0x00) && (pBUFFER[1] == 0x00) 
			&& (pBUFFER[2] == 0x01))//0x000001
	{
		debug_info("Find current frame syncword: 0x000001 \n");
		pBUFFER += 3;
		llen +=3;
	}else{
		//debug_info("Find current frame syncword error! \n");
		//return 0; //test code not process error
	}

	/*check next syncword*/
	for(;llen < (int)loadsize; llen++)
	{
		if( ((pBUFFER[0] == 0x00) && (pBUFFER[1] == 0x00) 
			&& (pBUFFER[2] == 0x01)) || //0x000001
			((pBUFFER[0] == 0x00) && (pBUFFER[1] == 0x00)
			&& (pBUFFER[2] == 0x00) && (pBUFFER[3] == 0x01))//0x00000001
			)
			{
			//debug_info("Find next frame syncword! \n");
			break;
		}else{
			pBUFFER++;
		}
	}
	//test code not process next syncword error
	
	uloffest += llen;
	*puloffest = uloffest;
	
	return llen;
}

/***************************************************************************
  Function		: build_queue( )
  Description	: build queue thread
  INput		: void *strm		: pointer of stream
  OUTput		: NULL
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static void
*build_queue(void *)
{
	unsigned int ulcount = 0; //left size to seed
	unsigned int offest = 0; // already send data offset
	unsigned int frmsize = 0; // frame size
	int frmcnt = 0;

	FILE *fpin;
	fpin = fopen(srtpinfo.strm,"rb");
	if(fpin == NULL)
	{
		printf("input H264 file is not found\n");
		goto fail;
	}
	fseek(fpin, 0, SEEK_END);
	ulcount = ftell(fpin);
	fseek(fpin, 0, SEEK_SET);

	do
	{
		/*Get current frame data and size*/
		frmsize = lGetFrmlen(ulcount,&offest,fpin);

		debug_info("frame[%d].size: %u\n", ++frmcnt, frmsize);
		/*Send packet to queue*/
		(void) RTPSend_Queue(BUFFER, frmsize, srtpinfo.ulFrmrate, (eFmtType)srtpinfo.eEnc_fmt);

		/*rtp wait*/
		//RTPTime::Wait(RTPTime(0,40));
		usleep(40000);
	}while((offest < ulcount)&&(LastCmd == 1));
	printf("Sending stream finishing...\n");

	fclose(fpin);
	
fail:
	pthread_exit(NULL);
}

/***************************************************************************
  Function		: RTP_Quit( )
  Description	: when get ctrl+c signal, quit
  INput		: int sign_no		: signal type
  OUTput		: NULL
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static void RTP_Quit(int sign_no)
{
	if(sign_no == SIGINT){
		printf("\n");
		printf("Use ctrl+c to quit!\n");
	}else if(sign_no == SIGTSTP){
		printf("\n");
		printf("Use ctrl+z to quit!\n");
	}

	if(LastCmd == 1){
		printf("Change start to quit!\n");
		LastCmd = 0;

		/*build_queue exit*/
		(void) pthread_join(pid, NULL);
		debug_info("build_queue thread exit!\n");
		
		(void) RTPSend_Close();
		debug_info("RTPSend_Thread thread exit!\n");

		printf("Queue and RTPSend both close OK!\n");
	}

	printf("Quit OK!\n");
	exit(0);
}

/***************************************************************************
  Function		: main( )
  Description	: main function
  INput		: int argc	
  			  char* argv[]
  OUTput		: 0,-1
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
int main(int argc, char* argv[])
{
	int lRet;
	char Cmd;
	
	/*Initialize*/
	vGetParam(argc,argv);

	(void) signal(SIGINT, RTP_Quit);
	(void) signal(SIGTSTP, RTP_Quit);
	
	do{
		printf("Start to send video data('s' or 'q'):");
		Cmd = getchar();
		getchar();

		if(Cmd == 's'){
			if(LastCmd == 0){
				printf("Change quit to start!\n");
				LastCmd = 1;
				/*init and set params*/
				lRet = RTPSend_Init(srtpinfo.cDest_ip, srtpinfo.ulDest_port);
				if(lRet < 0){
					debug_info("[ RTP ]: Init error\n");
					return RTP_RET_FAIL;
				}
				
				lRet = pthread_create(&pid, NULL, build_queue, NULL);
				debug_info("build_queue thread OK!\n");
				if(lRet != 0){
					debug_info("[ RTP ]: pthread build_quene error\n");
					return RTP_RET_FAIL;
				}
				#ifdef THREAD_ENB	
				(void) RTPSend_Thread();
				#endif
				debug_info("RTPSend_Thread thread OK!\n");
				
			}else{
				printf("Current start: %c. State not change!\n", Cmd);
			}
		}else if(Cmd == 'q'){
			if(LastCmd == 1){
				printf("Change start to quit!\n");
				LastCmd = 0;

				/*build_queue exit*/
				(void) pthread_join(pid, NULL);
				debug_info("build_queue thread exit!\n");
				
				lRet = RTPSend_Close();
				debug_info("RTPSend_Thread thread exit!\n");
				if(lRet < 0){
					debug_info("[ RTP ]: destory error");
					return RTP_RET_FAIL;
				}
				printf("Queue and RTPSend both close OK!\n");
			}else{
				printf("Current start: %c. State not change!\n", Cmd);
			}
		}else{
			printf("Please input 's' or 'q' !\n");
		}
	}while(1);
	
	return 0;
}
