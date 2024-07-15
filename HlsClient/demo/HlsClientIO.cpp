/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClientIO.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsClientIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <thread>
#include <stdio.h>  
#include <sys/types.h>  
#include <errno.h>  
#include <string.h>

using std::thread;

#define HLS_IO_RECV_MAX_LEN (3*1024*1024)
#define HLS_IO_SEND_MAX_LEN (10240)
#define HLS_IO_FILE_MAX_LEN (2*1024*1024)

typedef enum HlsClientState
{
    HLS_CLIENT_INIT=0,
    HLS_CLIENT_GET_M3U8,
    HLS_CLIENT_HANDLE_M3U8,
}E_HlsClientState;


/*****************************************************************************
-Fuction		: HlsClientIO
-Description	: HlsClientIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HlsClientIO :: HlsClientIO()
{
    m_pHlsClient=new HlsClientInf();
    m_iClientSocketFd=-1;
    m_iHlsClientIOFlag = 0;
}

/*****************************************************************************
-Fuction		: ~HlsClientIO
-Description	: ~HlsClientIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HlsClientIO :: ~HlsClientIO()
{
    if(NULL!= m_pHlsClient)
    {
        HLS_LOGW("~HlsClientIO start exit\r\n");
        delete m_pHlsClient;
        m_pHlsClient = NULL;
    }
    HLS_LOGW("~~HlsClientIO exit\r\n");
}

/*****************************************************************************
-Fuction		: Proc
-Description	: ����
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HlsClientIO :: Proc(const char * i_strHttpURL)
{
    int iRet=-1;
    char *pcRecvBuf=NULL;
    char *pcSendBuf=NULL;
    int iRecvLen=-1;
    timeval tTimeValue;
    string strIP("");
    int iPort=0;
    string strURL("");
    E_HlsClientState eHlsClientState=HLS_CLIENT_INIT;
    milliseconds timeMS(300);// ��ʾ300����
    int iOffset=0,iTryTime=0;
    int iFileLen=0;
    char *pcFileBuf=NULL;
    string strFileDir("");
    string strFileName("");
    char strFilePath[128]={0,};

    
    pcRecvBuf = new char[HLS_IO_RECV_MAX_LEN];
    if(NULL == pcRecvBuf)
    {
        HLS_LOGE("HlsClientIO NULL == pcRecvBuf err\r\n");
        return -1;
    }
    pcSendBuf = new char[HLS_IO_SEND_MAX_LEN];
    if(NULL == pcSendBuf)
    {
        HLS_LOGE("HlsClientIO NULL == pcSendBuf err\r\n");
        delete[] pcRecvBuf;
        return -1;
    }
    m_iHlsClientIOFlag = 1;
    HLS_LOGW("HlsClientIO start Proc\r\n");
    while(m_iHlsClientIOFlag)
    {
        iRet=m_pHlsClient->ParseHttpURL(i_strHttpURL,&strIP,&iPort,&strURL);
        if(iRet < 0)
        {
            HLS_LOGE("m_pHlsClient->ParseHttpURL err exit %s\r\n",i_strHttpURL);
            break;
        }
        iRet=CreateSaveDir(strURL.c_str()+1,&strFileDir);
        if(iRet < 0)
        {
            HLS_LOGE("m_pHlsClient->CreateSaveDir err exit %s\r\n",i_strHttpURL);
            break;
        }
        switch(eHlsClientState)
        {
            case HLS_CLIENT_INIT:
            {
                iRet=TcpClient::Init(&strIP,(unsigned short)iPort);
                if(iRet < 0)
                {
                    HLS_LOGE("TcpClient::Init err exit %s,%d\r\n",strIP.c_str(),iPort);
                    break;
                }
                m_iClientSocketFd=TcpClient::GetClientSocket();
                
                eHlsClientState=HLS_CLIENT_GET_M3U8;
                break;
            }
            case HLS_CLIENT_GET_M3U8:
            {
                memset(pcSendBuf,0,HLS_IO_SEND_MAX_LEN);
                iRet=m_pHlsClient->GenerateGetM3U8Req(strURL.c_str(),pcSendBuf,HLS_IO_SEND_MAX_LEN);
                if(iRet < 0)
                {
                    HLS_LOGE("m_pHlsClient->GenerateGetM3U8Req err exit %s\r\n",i_strHttpURL);
                    break;
                }
                iRet=TcpClient::Send(pcSendBuf,iRet);
                if(iRet < 0)
                {
                    HLS_LOGE("TcpClient::Send err exit %s,%d\r\n",strIP.c_str(),iPort);
                    break;
                }
                iRecvLen = 0;
                memset(pcRecvBuf,0,HLS_IO_RECV_MAX_LEN);
                iRet=TcpClient::Recv(pcRecvBuf,&iRecvLen,HLS_IO_RECV_MAX_LEN,m_iClientSocketFd,&timeMS);
                if(iRet < 0)
                {
                    HLS_LOGE("TcpClient::Recv err exit %d\r\n",iRecvLen);
                    break;
                }
                if(iRecvLen<=0)
                {
                    HLS_LOGE("TcpClient::Recv iRecvLen err exit %d\r\n",iRecvLen);
                    iRet=-1;
                    break;
                }
                iOffset=0;
                iTryTime=0;
                eHlsClientState=HLS_CLIENT_HANDLE_M3U8;
                break;
            }
            case HLS_CLIENT_HANDLE_M3U8:
            {
                iRet=m_pHlsClient->HandleHttpM3U8(pcRecvBuf,&iOffset,&iTryTime,&strFileName,pcSendBuf,HLS_IO_SEND_MAX_LEN);
                if(iRet <= 0)
                {
                    HLS_LOGE("HandleHttpM3U8 err %s,%d\r\n",strIP.c_str(),iPort);
                    iRet=0;
                    eHlsClientState=HLS_CLIENT_GET_M3U8;
                    break;
                }

                do
                {
                    iRet=TcpClient::Send(pcSendBuf,iRet);
                    if(iRet < 0)
                    {
                        HLS_LOGE("TcpClient::Send err exit %d\r\n",eHlsClientState);
                        break;
                    }
                    iRecvLen = 0;
                    memset(pcRecvBuf,0,HLS_IO_RECV_MAX_LEN);
                    iRet=TcpClient::Recv(pcRecvBuf,&iRecvLen,HLS_IO_RECV_MAX_LEN,m_iClientSocketFd,&timeMS);
                    if(iRet < 0)
                    {
                        HLS_LOGE("TcpClient::Recv err exit %d\r\n",iRecvLen);
                        break;
                    }
                    if(iRecvLen<=0)
                    {
                        HLS_LOGE("TcpClient::Recv iRecvLen err %d\r\n",iTryTime);
                        if(iTryTime > 0)
                        {
                            SleepMs(iTryTime);
                            iTryTime=0;
                            continue;
                        }
                        break;
                    }
                    iRet=m_pHlsClient->HandleHttpMedia(pcRecvBuf,iRecvLen,&pcFileBuf,&iFileLen);//m3u8Ƕ��m3u8����������ٴ���
                    if(iRet < 0)
                    {
                        HLS_LOGE("HandleHttpM3U8 err %s,%d\r\n",strIP.c_str(),iPort);
                        break;
                    }
                    snprintf(strFilePath,sizeof(strFilePath),"%s/%s",strFileDir.c_str(),strFileName.c_str());
                    this->SaveFile((const char *)strFilePath,pcFileBuf,iFileLen);
                }while(iTryTime>0);

                break;
            }
        }        
        if(iRet < 0)
        {
            HLS_LOGE("eHlsClientState err exit %s,%d\r\n",strIP.c_str(),eHlsClientState);
            break;
        }
    }
    
    if(m_iClientSocketFd>=0)
    {
        TcpClient::Close();
        HLS_LOGW("TcpClient::Close m_iClientSocketFd Exit%d\r\n",m_iClientSocketFd);
    }
    if(NULL != pcSendBuf)
    {
        delete[] pcSendBuf;
    }
    if(NULL != pcRecvBuf)
    {
        delete[] pcRecvBuf;
    }
    m_iHlsClientIOFlag=0;
    return 0;
}

/*****************************************************************************
-Fuction		: SaveFile
-Description	: HlsServerIO
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HlsClientIO :: SaveFile(const char * strFileName,char *i_pData,int i_iLen)
{
    int iRet = -1;
    FILE  *pMediaFile=NULL;

    if(NULL == strFileName || NULL == i_pData || i_iLen <= 0)
    {
        HLS_LOGE("SaveFile NULL \r\n");
        return iRet;
    }
    pMediaFile = fopen(strFileName,"wb");//
    if(NULL == pMediaFile)
    {
        HLS_LOGE("fopen %s err\r\n",strFileName);
        return iRet;
    } 
    iRet = fwrite(i_pData, 1,i_iLen, pMediaFile);
    if(iRet != i_iLen)
    {
        HLS_LOGE("fwrite err %d iWriteLen%d\r\n",iRet,i_iLen);
        return iRet;
    }
    if(NULL != pMediaFile)
    {
        fclose(pMediaFile);//fseek(m_pMediaFile,0,SEEK_SET); 
    } 
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
int HlsClientIO :: CreateSaveDir(const char * i_strLocation,string *o_strSaveDir)
{
    int iRet = -1;
    char strLocation[128];

    if(NULL == i_strLocation || NULL == o_strSaveDir)
    {
        HLS_LOGE("CreateSaveDir NULL \r\n");
        return iRet;
    }
    memset(strLocation,0,sizeof(strLocation));
    memcpy(strLocation,i_strLocation,strlen(i_strLocation));
    char *sep = strchr(strLocation, '/');
    if (sep != NULL) 
    {  
        *sep = '\0';  
        if (MakeDir(strLocation) == -1) 
        {  
            if (errno == EEXIST) 
            {  
            } 
            else if (errno == ENOENT) 
            {  
                return iRet;
            } 
        } 
        *sep = '/';  
        char *sep2 = strchr(sep+1, '/');
        *sep2 = '\0';  
        if (MakeDir(strLocation) == -1) 
        {  
            if (errno == EEXIST) 
            {  
            } 
            else if (errno == ENOENT) 
            {  
                return iRet;
            } 
        } 
        o_strSaveDir->assign(strLocation);
        *sep2 = '/';  
    }

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
int HlsClientIO :: GetProcFlag()
{
    return m_iHlsClientIOFlag;//���߳̾���ע���Ż�
}

