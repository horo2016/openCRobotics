/**********************************************************************
Copyright (C) 2015 CASKY eTech Co., Ltd .
------------------------------------------------------------
Project    : UAV
************************************************************************
   File Name	: base_rtp.cpp
   Module Name	: rtp common function
   Description	: 
   History:
  No.		Ver.		Date		Designed by		Comment
-----+----------+-----------------+--------------+-----------------
   1		2015.07.24		CASKY			new created

************************************************************************/
#include <semaphore.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#ifndef WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif // WIN32
#include <iostream>
#include <memory.h>

#include "jrtplib3/rtpdefines.h"
#include "jrtplib3/rtpsession.h"
#include "jrtplib3/rtppacket.h"
#include "jrtplib3/rtpudpv4transmitter.h"
#include "jrtplib3/rtpipv4address.h"
#include "jrtplib3/rtpsessionparams.h"
#include "jrtplib3/rtperrors.h"
#include "jrtplib3/rtpsourcedata.h"

#include "base_rtp.h"

/* RTP defination */
#define RTP_LOCALPORTBASE	6000
#define RTP_VIDEO_CLOCKRATE	90000.0
//#define RTP_DEFCODEC		98   //zxl
#define RTP_DEFCODEC		96

/* support video  framerate */
typedef enum {
	RTP_FRAMERATE_24 = 24,
	RTP_FRAMERATE_30 = 30,
	RTP_FRAMERATE_10 = 10
} eFrmRate;

//#define DEBUG
#ifdef DEBUG
#define debug_info(fmt, args...) \
	fprintf(stdout, fmt, ##args)
#else
#define debug_info(fmt, args...) 
#endif



#define MAX_SESS_BND		20000000.0

#define RTP_PACKET_SIZE \
	(RTP_DEFAULTPACKETSIZE-12)


#define RTP_PACKET_SIZE_NAL \
	(RTP_DEFAULTPACKETSIZE-14)
/*
#define IPC_PRT(fmt...)   \
    do {\
        printf("[%s]-%d: ", __FUNCTION__, __LINE__);\
        printf(fmt);\
       }while(0)
*/
#define IPC_PRT(fmt...)

#define FNRI_TYPE		0x1C
#define SER_START		0x80
#define SER_MIDLLE		0x00
#define SER_END			0x40
#define TYPE_MSK		0x1F
#define FNRI_SER_MSK	0xE0
#define NIDR_TYPE		0x01
#define IDR_TYPE		0x05

typedef struct _RtpPack{
	unsigned char fui;
	unsigned char fuh;
	unsigned char data[RTP_PACKET_SIZE-2];
}RtpPack;

#ifdef THREAD_ENB
static int ulSendFlg = 0;
#endif /* THREAD_ENB */
static void* pvSession = NULL;
static char* pDest_ip = NULL;
static unsigned int Dest_port = 7078;
#ifdef THREAD_ENB
static pthread_t Rtp_pid = 0;
#endif /* THREAD_ENB */

using namespace jrtplib;



void s_rtpsession_check_error(int err)
{
    if (err < 0) {
        debug_info("rtpsession Error:%s\n", RTPGetErrorString(err).c_str());
    }
}

/***************************************************************************
  Function	: RTPSession_delport( )
  Description: to delete the port from specific session
  INput		: void* vp						: point to the rtp session
			  char* remote_ip4addr			: the remote ip address
			  unsigned short int destport	: the remote destinate port
  OUTput	: int							: 0 for success, -1 for failure
  Notation	:

 History:
  No.	Date			Designed by		Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static int RTPSession_delport(void* vp, char *remote_ip4addr,
			unsigned short int destport)
{
	int lRet = 0;
	unsigned int destip;
	destip = inet_addr(remote_ip4addr);

	if( (destip == INADDR_NONE) ){
		error_check("Bad IP address specified\n");
		/* invalid address, return -1 */
		lRet = -1;
		return lRet;
	}
	
	/* malloc RTPSession */
	RTPSession* psess;
	psess = ( RTPSession* ) vp;
	destip = ntohl(destip);

	/* Delete remote address and port*/
	lRet = psess -> DeleteDestination(RTPIPv4Address(destip,  destport));
	if( (lRet < 0) ){
		error_check("add remote address and port failed %d\n",lRet);
	} else {
		IPC_PRT("[ Del ] : ip, %s port, %d\n", remote_ip4addr, destport);
	}

	return lRet;
}


/***************************************************************************
  Function		: RTPSession_addport( )
  Description	: to add the port from specific session
  INput			: void* vp						: point to the rtp session
				  char* remote_ip4addr			: the remote ip address
				  unsigned short int destport	: the remote destinate port
  OUTput		: int							: 0 for success, -1 for failure
  Notation	:

 History:
  No.	Date			Designed by		Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static int RTPSession_addport(void* vp, char *remote_ip4addr,
			unsigned short int destport)
{
	int lRet = 0;
	unsigned int destip;
	destip = inet_addr(remote_ip4addr);
	
	if( (destip == INADDR_NONE) ){
		error_check("Bad IP address specified\n");
		/* invalid address, return -1 */
		lRet = -1;
		return lRet;
	}
	
	/* malloc RTPSession */
	RTPSession* psess;
	psess = ( RTPSession* ) vp;
	destip = ntohl(destip);

	/* Add remote address and port*/
	lRet = psess -> AddDestination(RTPIPv4Address(destip,  destport));
	if( (lRet < 0) ){
		error_check("add remote address and port failed %d\n",lRet);
	} else {
		debug_info("[ Add ] : ip, %s port, %d\n", remote_ip4addr, destport);
	}

	return lRet;
}

/***************************************************************************
  Function		: RTPSession_serverinit( )
  Description	: init the server session of rtp
  INput			: void** vp						: point to the point of rtp session
				  unsigned short int portbase	: the remote destinate port
				  double dHz					: the Hz of the session
  OUTput		: int							: 0 for success, -1 for failure
  Notation		:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static int RTPSession_serverinit(void** vp,
			unsigned short int portbase,
			double dHz)
{
#define MIN_SENDBUF_SIZE	(1*1024*1024)
	int lRet = 0;
	bool acceptflg = true;

	/* initial the return value */
	*vp = NULL;
	
	RTPSessionParams sessparams;
	RTPUDPv4TransmissionParams transparams;

	/* malloc RTPSession */
	RTPSession* psess;
	psess = new RTPSession;

	/* Setting time stamp unit */
	sessparams.SetOwnTimestampUnit(1.0/dHz);
	
	/* Setting accept flag */
	sessparams.SetAcceptOwnPackets(acceptflg);
	/* Setting rtp band width*/
	//sessparams.SetSessionBandwidth(MAX_SESS_BND);

	int size = transparams.GetRTPSendBuffer();
	printf("before set send buffer size[%d]\n", size);
	if( (size < MIN_SENDBUF_SIZE) ){
		size = MIN_SENDBUF_SIZE;
	}
	transparams.SetRTPSendBuffer(size);
	printf("after set send buffer size[%d]\n", size);
	
	/* Setting local port */
	transparams.SetPortbase(portbase);
	//IPC_PRT("Begin to careat\n");  
	/* Create session */
	lRet = psess -> Create(sessparams, &transparams);
	//IPC_PRT("Creat finish\n"); 
	if( (lRet < 0) ){
		error_check("create rtp session failed %d\n",lRet);
		delete psess;
	}else{
		/* create ok */
		*vp = ( void* ) psess;
	}
	return lRet;
}

/***************************************************************************
  Function		: RTPSession_setpayloadtype( )
  Description	: set the payload type of the specific session
  INput			: void* vp				: point to the rtp session
				  int payloadtype		: the payload type
  OUTput		: int					: 0 for success, -1 for failure
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
   1		2015.07.24		CASKY			new created

***************************************************************************/
static int RTPSession_setpayloadtype(void* vp, int payloadtype)
{
	int lRet = 0;
	/* initial RTPSession */
	RTPSession* psess;
	psess = ( RTPSession* ) vp;
	debug_info("playloadtype p %p\n", vp);
	/* Setting playload type */
	//int lRet = psess->SetDefaultPayloadType(playloadtype);
	lRet = psess -> SetDefaultPayloadType(payloadtype);
	if( (lRet < 0) ){
		error_check("set playload type failed,%d\n",lRet);
	}
	return lRet;
}

/***************************************************************************
  Function		: RTPSession_destroy( )
  Description	: destory the specific session
  INput			: void* vp		: point to the rtp session
				  int sec		: to construct rtptime
				  int usec		: to construct rtptime
  OUTput		: void
  Notation		:

  History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static void RTPSession_destroy(
	void* vp, unsigned int sec, unsigned int usec)
{
	RTPSession* psess;
	psess = ( RTPSession* ) vp;
	
	//psess->BYEDestroy(time, 0, 0);
	psess -> BYEDestroy(RTPTime(sec, usec), 0, 0);
	delete psess;
}

/***************************************************************************
* function name  : s_rtpsession_nalu_alloc( )
* description    : alloc memory
* param[in]      : int buffersize      : alloc buffer size
* param[out]     : none
* return         : nalu_t for success, NULL for failure
***************************************************************************/
static nalu_t *s_rtpsession_nalu_alloc(int buffersize)
{
    nalu_t *n;

    if ((n = (nalu_t*)calloc (1, sizeof (nalu_t))) == NULL) {
        debug_info("[ Err ]calloc error\n");
        return NULL;
    }

    n->max_size=buffersize;

    if ((n->buf = (unsigned char*)calloc(buffersize, sizeof(char))) == NULL) {
        free (n);
        debug_info("[ Err ]calloc error\n");
        return NULL;
    }

    return n;
}

/***************************************************************************
* function name  : s_rtpsession_nalu_free( )
* description    : free memory
* param[in]      : nalu_t *n      : free buffer struct
* param[out]     : none
* return         : none
***************************************************************************/
static void s_rtpsession_nalu_free(nalu_t *n)
{
    if (n) {
        if (n->buf) {
            free(n->buf);
            n->buf=NULL;
        }
        free (n);
    }
}

/***************************************************************************
* function name  : s_rtpsession_send( )
* description    : send rtp packet of the specific session
* param[in]      : void* vp          : point to the rtp session
                   int timestamp     : time stamp
                   void *pBuffer     : the head of the memery to be send
                   int length        : the length of the memery to be send
* param[out]     : none
* return         : 0 for success, -1 for failure
***************************************************************************/
static int s_rtpsession_send(void* vp, int timpstamp,
                            void *pBuffer,unsigned int length)
{
    nalu_header     *nalu_hdr;
    fu_indicator    *fu_ind;
    fu_header       *fu_hdr;
    int status = 0;
    char sendbuf[RTP_DEFAULTPACKETSIZE];
    char* nalu_payload;
    nalu_t *n;
    RTPSession *psess  = (RTPSession *)vp;
    unsigned char *pucData = (unsigned char *)pBuffer;
    unsigned int microseconds;

    n = s_rtpsession_nalu_alloc((int)length);
    if (NULL == n) {
        debug_info("alloc error\n");
        return -1;
    }

    if ((4 >= length) || (NULL == pucData)) {
        s_rtpsession_nalu_free(n);
        debug_info("rtp_send para error\n");
        return -1;
    }

    /* H.264 NALU -> RTP PACKET
    [Start Code] [NALU Header] [NALU Payload] ->  [NALU Header] [NALU Payload] */
    n->len = length-4;
    memcpy(n->buf, &pucData[4], n->len);

    n->forbidden_bit = n->buf[0] & 0x80;// 1 bit
    n->nal_reference_idc = n->buf[0] & 0x60; // 2 bit
    n->nal_unit_type = n->buf[0] & 0x1f;// 5 bit
    if (n->nal_unit_type != 1) {
        //debug_info("nal_unit_type: %d, length: %u\n",n->nal_unit_type,length);
    }

    if (RTP_PACKET_SIZE_NAL>= n->len) {
        nalu_hdr = (nalu_header*)&sendbuf[0];
        nalu_hdr->F = n->forbidden_bit>>7;
        nalu_hdr->NRI = n->nal_reference_idc>>5;
        nalu_hdr->TYPE = n->nal_unit_type;

        nalu_payload = &sendbuf[1];
        memcpy(nalu_payload, n->buf+1, n->len-1);

        if (n->nal_unit_type==1 || n->nal_unit_type==5) {
            status = psess->SendPacket((void *)sendbuf, n->len, RTP_DEFCODEC, true, timpstamp);
        } else {
            status = psess->SendPacket((void *)sendbuf, n->len, RTP_DEFCODEC, false, 0);
        }
        if (status < 0) {
            DEBUG(LOG_ERR,"SendPacket error\n");
            s_rtpsession_check_error(status);
            return -1;
        }
    } else if (RTP_PACKET_SIZE_NAL< n->len) {
        int packetNum = 0;
        int lastPackSize = 0;
        int packetIndex = 0;

        packetNum = n->len/RTP_PACKET_SIZE_NAL;
        lastPackSize = n->len%RTP_PACKET_SIZE_NAL;

        if (lastPackSize == 0) {
            packetNum = packetNum - 1;
            lastPackSize = RTP_PACKET_SIZE_NAL;
        }

        microseconds = 25000/packetNum;
	//	RTPTime wait(0, microseconds);

        while(packetIndex <= packetNum) {
            if(!packetIndex) {
                memset(sendbuf, 0, RTP_DEFAULTPACKETSIZE);
                fu_ind = (fu_indicator*)&sendbuf[0];
                fu_ind->F = n->forbidden_bit>>7;
                fu_ind->NRI = n->nal_reference_idc>>5;
                fu_ind->TYPE = 28;

                fu_hdr = (fu_header*)&sendbuf[1];
                fu_hdr->S = 1;
                fu_hdr->E = 0;
                fu_hdr->R = 0;
                fu_hdr->TYPE = n->nal_unit_type;

                nalu_payload = &sendbuf[2];
                memcpy(nalu_payload, n->buf+1, RTP_PACKET_SIZE_NAL);
                status = psess->SendPacket((void *)sendbuf, RTP_PACKET_SIZE_NAL+2, RTP_DEFCODEC, true, timpstamp);
                if (status < 0) {
                    DEBUG(LOG_ERR,"SendPacket error\n");
                    s_rtpsession_check_error(status);
                    return -1;;
                }

                packetIndex++;
              //  RTPTime::Wait(wait);
            } else if(packetNum == packetIndex) {
                memset(sendbuf, 0, RTP_DEFAULTPACKETSIZE);
                fu_ind = (fu_indicator*)&sendbuf[0];
                fu_ind->F = n->forbidden_bit>>7;
                fu_ind->NRI = n->nal_reference_idc>>5;
                fu_ind->TYPE = 28;

                fu_hdr = (fu_header*)&sendbuf[1];
                fu_hdr->S = 0;
                fu_hdr->E = 1;
                fu_hdr->R = 0;
                fu_hdr->TYPE = n->nal_unit_type;

                nalu_payload = &sendbuf[2];
                memcpy(nalu_payload, n->buf+packetIndex*RTP_PACKET_SIZE_NAL+1, lastPackSize-1);
                status = psess->SendPacket((void *)sendbuf, lastPackSize+1, RTP_DEFCODEC, false, 0);
                if (status < 0) {
                    DEBUG(LOG_ERR,"SendPacket error\n");
                    s_rtpsession_check_error(status);
                    return -1;;
                }

                packetIndex++;
                //RTPTime::Wait(wait);
            } else if(packetIndex < packetNum && 0!=packetIndex) {
                memset(sendbuf, 0, RTP_DEFAULTPACKETSIZE);
                fu_ind = (fu_indicator*)&sendbuf[0];
                fu_ind->F = n->forbidden_bit>>7;
                fu_ind->NRI = n->nal_reference_idc>>5;
                fu_ind->TYPE = 28;

                fu_hdr = (fu_header*)&sendbuf[1];
                fu_hdr->S = 0;
                fu_hdr->E = 0;
                fu_hdr->R = 0;
                fu_hdr->TYPE = n->nal_unit_type;

                nalu_payload = &sendbuf[2];
                memcpy(nalu_payload, n->buf+packetIndex*RTP_PACKET_SIZE_NAL+1, RTP_PACKET_SIZE_NAL);
                status = psess->SendPacket((void *)sendbuf, RTP_PACKET_SIZE_NAL+2, RTP_DEFCODEC, false, 0);
                if (status < 0) {
                    DEBUG(LOG_ERR,"SendPacket error\n");
                    s_rtpsession_check_error(status);
                    return -1;;
                }

                packetIndex++;
              //  RTPTime::Wait(wait);
            }
        }//while
    }//elseif

    s_rtpsession_nalu_free(n);

    return 0;
}
/***************************************************************************
  Function		: RTPSession_send( )
  Description	: send rtp packet of the specific session
  INput			: void* vp			: point to the rtp session
				  int timestamp		: time stamp
				  void *pBuffer		: the head of the memery to be send
				  int length		: the length of the memery to be send
  OUTput		: int				: -1 for failure
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
static int RTPSession_send(
	void* vp, int timpstamp,void *pBuffer,unsigned int length)
{
	long lRet;
	bool mark = false;
	unsigned char *pucData = 
		(unsigned char *)pBuffer;
	static  unsigned int q_count=0;
	q_count ++;
	/* initial RTPSession */
	RTPSession* psess;
	psess = ( RTPSession* ) vp;
	//printf("[zxl_log][%d]timpstamp 0x%x len is %d \n",q_count,timpstamp,length);
	//printf("[zxl_log][%d]pucData[] %x %x %x %x %x %x %x %x %x %x %x\n",q_count,pucData[0],pucData[1],pucData[2],pucData[3],pucData[4],pucData[5],pucData[6],pucData[7],pucData[8],pucData[9],pucData[10]);
	//debug_info("length=%d\n",length);
	/* Remove sync word */
	while( (*pucData++ == 0x00) ){
		length--;
	};
	length--;
	//debug_info("length=%d\n",length);
	
	/* Set playload type */
	//lRet = psess->SetDefaultMark(mark);
	lRet = psess -> SetDefaultMark(mark);

	if( (lRet < 0) ){
		error_check("set default mark failed,%ld\n",lRet);
	}else{
		/* Seding packet */
		if( (length <= RTP_PACKET_SIZE) ){
			//lRet = psess->SetDefaultTimestampIncrement(timpstamp);
			lRet = psess -> SetDefaultTimestampIncrement(timpstamp);
			if( (lRet < 0) ){
				error_check("set time stamp failed,%ld\n", lRet);
			}else{				
				//lRet = psess->SendPacket(pucData,length);
				lRet = psess -> SendPacket(pucData,length);
				if( (lRet < 0) ){
					error_check("rtp send packet failed,%ld\n",lRet);
				}
			}
		}else{
			//lRet = psess->SetDefaultTimestampIncrement(0);
			lRet = psess -> SetDefaultTimestampIncrement(0);
			if( (lRet < 0) ){
				error_check("set time stamp failed,%ld\n", lRet);
			}else{
				RtpPack pakt;
				pakt.fui = ((pucData[0] & FNRI_SER_MSK) | FNRI_TYPE);
				pakt.fuh = ((pucData[0] & TYPE_MSK) | SER_START);
				memcpy(pakt.data,pucData+1,(RTP_PACKET_SIZE-2));		
			
				//lRet = psess->SendPacket((void*)&pakt,RTP_PACKET_SIZE);
				//printf("[zxl_log][%d]sendpacket %x %x %x %x %x %x %x %x\n",q_count,pakt.data[0],pakt.data[1],pakt.data[2],pakt.data[3],pakt.data[4],pakt.data[5],pakt.data[6],pakt.data[7]);
				lRet = psess -> SendPacket((void*)&pakt,RTP_PACKET_SIZE);
				if( (lRet < 0) ){
					error_check("rtp send packet failed,%ld\n",lRet);
				}else{
					pucData += (RTP_PACKET_SIZE-1);
					length  -= (RTP_PACKET_SIZE-1);
					
					for(;length > (int)(RTP_PACKET_SIZE-2);)
					{
						pakt.fuh = (pakt.fuh & TYPE_MSK);
						memcpy(pakt.data,pucData,(RTP_PACKET_SIZE-2));
						//lRet = psess->SendPacket((void*)&pakt,RTP_PACKET_SIZE);
						lRet = psess -> SendPacket((void*)&pakt,RTP_PACKET_SIZE);
						if( (lRet < 0) ){
							error_check("rtp send packet failed,%ld\n",lRet);
							break;
						}
						pucData += (RTP_PACKET_SIZE-2);
						length  -= (RTP_PACKET_SIZE-2);
					}

					/* Send last package and update time stamp */
					if( lRet < 0 ){
					}else{
						//lRet = psess->SetDefaultTimestampIncrement(timpstamp);
						lRet = psess -> SetDefaultTimestampIncrement(timpstamp);
						if( (lRet < 0) ){
							error_check("set time stamp failed,%ld\n", lRet);
						}else{
							pakt.fuh = ( (pakt.fuh & TYPE_MSK) | SER_END);
							memcpy(pakt.data,pucData,length);
							//lRet = psess->SendPacket((void*)&pakt,length+2);
							lRet = psess -> SendPacket((void*)&pakt,length+2);
							//IPC_PRT("Send This\n");
							if( (lRet < 0) ){
								error_check("rtp send packet failed,%ld\n",lRet);
							}
						}
					}
				}
			}
		}
	}
	return lRet;
}

/***************************************************************************
  Function		: RTPSend_Main( )
  Description	: get data from queue ,and send rtp packet of the specific session 
  INput		: void*
  OUTput		: NULL
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
#ifdef THREAD_ENB
 void *RTPSend_Main(void *)
{
	int lRet;

	debug_info("[ RTP ]:Start to send!\n");
	
	do{
		AVPacket *packet = video_packet_queue_pop_tail();
		debug_info("start to pop in queue's packet size: 0x%x\n", packet->size);
		if(packet->size == 0){
			free(packet);
			continue;
		}

		/*check frame rate*/
		if((packet->ulFrmrate != RTP_FRAMERATE_24) &&
			(packet->ulFrmrate != RTP_FRAMERATE_30)){
			error_check("[ RTP ] : Frame rate not support\n");
			if(packet->data != NULL)
				free(packet->data);
			free(packet);
			continue;
		}

		/*check encode codec*/
		if(packet->eEnc_fmt != FORMAT_TYPE_H264){
			error_check("[ RTP ] : Encode codec not support\n");
			if(packet->data != NULL)
				free(packet->data);
			free(packet);
			continue;
		}

		/* RTP func: rtp send*/
		lRet = RTPSession_send(pvSession,
			(int)RTP_VIDEO_CLOCKRATE/packet->ulFrmrate, 
			(void *)packet->data, packet->size);
		if (lRet < 0)
		{
			error_check("[ RTP ] : RTP send error\n");
			break;
		}

		if(packet->data != NULL)
			free(packet->data);
		free(packet);
	}while(1);
	debug_info("[ RTP ]:Stop to send!\n");

	pthread_exit(NULL);
}
#endif /* THREAD_ENB */
/***************************************************************************
  Function		: RTPSend_Main( )
  Description	: get data from queue ,and send rtp packet of the specific session 
  INput		: void*
  OUTput		: NULL
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
void RTPSend_Loop()
{
	int lRet;

	DEBUG(LOG_DEBUG,"[ RTP ]:Start to send!\n");
	
	do{
		AVPacket *packet = video_packet_queue_pop_tail();
		debug_info("start to pop in queue's packet size: 0x%x\n", packet->size);
		if(packet->size == 0){
			free(packet);
			continue;
		}

		/*check frame rate*/
		if((packet->ulFrmrate != RTP_FRAMERATE_24) &&
			(packet->ulFrmrate != RTP_FRAMERATE_30)){
			error_check("[ RTP ] : Frame rate not support\n");
			if(packet->data != NULL)
				free(packet->data);
			free(packet);
			continue;
		}

		/*check encode codec*/
		if(packet->eEnc_fmt != FORMAT_TYPE_H264){
			error_check("[ RTP ] : Encode codec not support\n");
			if(packet->data != NULL)
				free(packet->data);
			free(packet);
			continue;
		}

		/* RTP func: rtp send*/
		lRet = RTPSession_send(pvSession,
			(int)RTP_VIDEO_CLOCKRATE/packet->ulFrmrate, 
			(void *)packet->data, packet->size);
		if (lRet < 0)
		{
			error_check("[ RTP ] : RTP send error\n");
			break;
		}
              usleep(100);
		if(packet->data != NULL)
			free(packet->data);
		free(packet);
	}while(1);
	debug_info("[ RTP ]:Stop to send!\n");

	pthread_exit(NULL);
}
/***************************************************************************
  Function		: RTPSend_Queue( )
  Description	: put packet to queue
  INput		: AVPacket *packet	: packet data and size
  OUTput		: int				: -1 for failure
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
int RTPSend_Queue(unsigned char *pbuffer, 
	unsigned int frmsize, unsigned int frmrate, eFmtType enc_fmt)
{
	int lRet = 0;
	
#ifdef THREAD_ENB
	AVPacket *packet = NULL;
	packet = (AVPacket*)malloc(sizeof(AVPacket));
	if(packet == NULL){
		error_check("AVPacket memory allocate error!\n");
		return -1;
	}

	packet->data = (unsigned char*)malloc(frmsize);
	if(packet->data == NULL){
		error_check("packet->data memory allocate error!\n");
		return -1;
	}
	/*copy current frame data and size into packet buffer*/
	memcpy(packet->data, pbuffer, frmsize);
	packet->size = frmsize;
	packet->ulFrmrate = frmrate;
	packet->eEnc_fmt = enc_fmt;
	
	lRet = video_packet_queue_push_head(packet);
	if(lRet < 0){	
		error_check("[ RTP ] : RTP  queue build error!\n");
		return -1;
	}
#else
	/*check frame rate*/
/*	if((frmrate != RTP_FRAMERATE_24) &&
		(frmrate != RTP_FRAMERATE_10)){
		error_check("[ RTP ] : Frame rate not support\n");
		return -1;
	}*/

	/*check encode codec*/
	if(enc_fmt != FORMAT_TYPE_H264){
		error_check("[ RTP ] : Encode codec not support\n");
		return -1;
	}

	/* RTP func: rtp send*/
/*	lRet =  s_rtpsession_send(pvSession,
		(int)RTP_VIDEO_CLOCKRATE/frmrate,pbuffer,frmsize);*/
	lRet = RTPSession_send(pvSession,
		(int)RTP_VIDEO_CLOCKRATE/(frmrate),pbuffer,frmsize);
	if (lRet < 0){
		error_check("[ RTP ] : RTP send error\n");
		return -1;
	}
#endif /* THREAD_ENB */
	return lRet;
}

/***************************************************************************
  Function		: RTPSend_Init( )
  Description	: rtp init and set params
  INput		: char* destip			: destination ip address
  			  unsigned int destport	: destination port
  OUTput		: int				: -1 for failure
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
int RTPSend_Init(char* destip, unsigned int destport)
{
	int lRet = 0;

#ifdef THREAD_ENB
	/*Set send flag to true*/
	ulSendFlg = 1;
#endif /* THREAD_ENB */

	pDest_ip = (char *)malloc(strlen(destip)+1);
	if(pDest_ip == NULL){
		error_check("[ RTP ] : Memory allocate error!\n");
		return -1;
	}
	strcpy(pDest_ip, destip);
	Dest_port = destport;

#ifdef THREAD_ENB
	/*Initial queue*/
	lRet = queue_init();
	if(lRet < 0){
		error_check("[ RTP ] : Queue init error!\n");
		return -1;
	}
#endif /* THREAD_ENB */

	/* RTP func: initialize */
	lRet = RTPSession_serverinit(
		&pvSession, RTP_LOCALPORTBASE, RTP_VIDEO_CLOCKRATE);
	if(lRet < 0){	
		error_check("[ RTP ] : RTP  serverinit error!\n");
		return -1;
	}

	/* RTP func: set rtp payload type */
	lRet = RTPSession_setpayloadtype(pvSession, RTP_DEFCODEC);
	if(lRet < 0){
		error_check("[ RTP ] : RTP  setplayloadtype failed!\n");
		return -1;
	}

	/* RTP func: add destination ip and port */
	lRet = RTPSession_addport(pvSession, pDest_ip, Dest_port);
	if (lRet < 0){
		error_check("[ RTP ] : restore to add ip and port error\n");
		return -1;
	}

	DEBUG(LOG_DEBUG,"[ RTP ] : RTPSend_Init OK!\n\n");
	return lRet;
}

/***************************************************************************
  Function		: RTPSend_Thread( )
  Description	: rtp send thread
  INput		: unsigned int frmrate	: video frame rate
  			  eFmtType enccodec	: video codec
  OUTput		: int				: -1 for failure
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
#ifdef THREAD_ENB
int RTPSend_Thread()
{
	int lRet = 0; 

	lRet = pthread_create(&Rtp_pid, NULL, RTPSend_Main, NULL);
	if(lRet != 0){
		error_check("[ RTP ]: pthread RTPSend_Main error\n");
		return -1;
	}

	return lRet;
}
#endif /* THREAD_ENB */

/***************************************************************************
  Function		: RTPSend_Close( )
  Description	: close rtp thread and release memory
  INput		: NULL
  OUTput		: int				: -1 for failure
  Notation	:

 History:
  No.	Date			Designed by   Comment
-----+---------------+--------------+---------------
  1		2015.07.24		CASKY			new created

***************************************************************************/
int RTPSend_Close()
{
	int lRet = 0;

#ifdef THREAD_ENB
	/*Stop to send*/
	ulSendFlg = 0;
	debug_info("[ RTP ]:Set flag to stop!\n");
	
	AVPacket *packet = (AVPacket*)malloc(sizeof(AVPacket));
	packet->data = NULL;
	packet->size = 0;
	packet->ulFrmrate = RTP_FRAMERATE_24;
	packet->eEnc_fmt= FORMAT_TYPE_H264;
	(void) video_packet_queue_push_head(packet);
	
	(void) pthread_join(Rtp_pid, NULL);
	debug_info("RTP send thread exit!\n");
	
	/*free memory*/
	queue_free();
#endif /* THREAD_ENB */

	/* RTP func: if valid, delete */
	lRet = RTPSession_delport(pvSession, pDest_ip, Dest_port);
	if (lRet < 0)
	{
		error_check("[ RTP ]:Delete the given ip:%s port %d error!\n",
					pDest_ip, Dest_port);
		return -1;
	}
	free(pDest_ip);
	
	/* RTP func: release resource */
	if(NULL != pvSession)
	{
		RTPSession_destroy(pvSession, 10, 0);
	}

	return lRet;
}

