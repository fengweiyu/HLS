/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerIO.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsServerIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <thread>

using std::thread;

#define HLS_IO_RECV_MAX_LEN (10240)
#define HLS_IO_SEND_MAX_LEN (3*1024*1024)

/*****************************************************************************
-Fuction		: WebRtcServer
-Description	: WebRtcServer
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HlsServerIO :: HlsServerIO(int i_iClientSocketFd,HlsServerInf * i_pHlsServer)
{
    m_iClientSocketFd=i_iClientSocketFd;
    m_pHlsServer=i_pHlsServer;
    
    m_iHlsServerIOFlag = 0;
    m_pHlsServerIOProc = new thread(&HlsServerIO::Proc, this);
    //m_pHttpSessionProc->detach();//注意线程回收
}

/*****************************************************************************
-Fuction		: ~WebRtcServer
-Description	: ~WebRtcServer
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HlsServerIO :: ~HlsServerIO()
{
    if(NULL!= m_pHlsServerIOProc)
    {
        HLS_LOGW("HlsServerIO start exit\r\n");
        m_iHlsServerIOFlag = 0;
        //while(0 == m_iExitProcFlag){usleep(10);};
        m_pHlsServerIOProc->join();//
        delete m_pHlsServerIOProc;
        m_pHlsServerIOProc = NULL;
    }
    HLS_LOGW("~~HlsServerIO exit\r\n");
}

/*****************************************************************************
-Fuction		: Proc
-Description	: 阻塞
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HlsServerIO :: Proc()
{
    int iRet=-1;
    char *pcRecvBuf=NULL;
    char *pcSendBuf=NULL;
    int iRecvLen=-1;
    timeval tTimeValue;
    
    if(m_iClientSocketFd < 0)
    {
        HLS_LOGE("WebRtcHttpSession m_iClientSocketFd < 0 err\r\n");
        return -1;
    }
    pcRecvBuf = new char[HLS_IO_RECV_MAX_LEN];
    if(NULL == pcRecvBuf)
    {
        HLS_LOGE("WebRtcHttpSession NULL == pcRecvBuf err\r\n");
        return -1;
    }
    pcSendBuf = new char[HLS_IO_SEND_MAX_LEN];
    if(NULL == pcSendBuf)
    {
        HLS_LOGE("WebRtcHttpSession NULL == pcSendBuf err\r\n");
        delete[] pcRecvBuf;
        return -1;
    }
    m_iHlsServerIOFlag = 1;
    HLS_LOGW("HlsServerIO start Proc\r\n");
    while(m_iHlsServerIOFlag)
    {
        iRecvLen = 0;
        memset(pcRecvBuf,0,HLS_IO_RECV_MAX_LEN);
        milliseconds timeMS(30);// 表示30毫秒
        iRet=TcpServer::Recv(pcRecvBuf,&iRecvLen,HLS_IO_RECV_MAX_LEN,m_iClientSocketFd,&timeMS);
        if(iRet < 0)
        {
            HLS_LOGE("TcpServer::Recv err exit %d\r\n",iRecvLen);
            break;
        }
        if(iRecvLen<=0)
        {
            continue;
        }
        memset(pcSendBuf,0,HLS_IO_SEND_MAX_LEN);
        iRet=m_pHlsServer->HandleHttpReq(pcRecvBuf,pcSendBuf,HLS_IO_SEND_MAX_LEN);
        if(iRet > 0)
        {
            TcpServer::Send(pcSendBuf,iRet,m_iClientSocketFd);
            continue;
        }
    }
    
    if(m_iClientSocketFd>=0)
    {
        TcpServer::Close(m_iClientSocketFd);//主动退出,
        HLS_LOGW("HlsServerIO::Close m_iClientSocketFd Exit%d\r\n",m_iClientSocketFd);
    }
    if(NULL != pcSendBuf)
    {
        delete[] pcSendBuf;
    }
    if(NULL != pcRecvBuf)
    {
        delete[] pcRecvBuf;
    }
    
    m_iHlsServerIOFlag=0;
    return 0;
}

/*****************************************************************************
-Fuction		: GetProcFlag
-Description	: HlsServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HlsServerIO :: GetProcFlag()
{
    return m_iHlsServerIOFlag;//多线程竞争注意优化
}

