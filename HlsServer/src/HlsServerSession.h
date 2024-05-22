/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerSession.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_SERVER_SESSION_H
#define HLS_SERVER_SESSION_H

#include "HttpServer.h"
#include "MediaHandle.h"
#include <thread>
#include <mutex>
#include <string>
#include <list>
#include <map>

using std::map;
using std::string;
using std::list;
using std::mutex;
using std::thread;


#define HLS_MP4_HEADER_MAX_LEN	(10*1024) 
#define HLS_MP4_BUF_MAX_LEN	(3*1024*1024) 

/*****************************************************************************
-Class          : MP4Header
-Description    : MP4Header
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2022/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class MP4Header
{
public:
    unsigned char abBuf[HLS_MP4_HEADER_MAX_LEN];
    unsigned int dwBufCurLen;
    //unsigned int dwBufMaxLen;
};

/*****************************************************************************
-Class          : MP4Stream
-Description    : MP4Stream
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2022/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class MP4Stream
{
public:
	MP4Stream(){strName.clear();};
	virtual ~MP4Stream(){};
    string strName;
    unsigned int dwDuration;//ms
    unsigned char abBuf[HLS_MP4_BUF_MAX_LEN];
    unsigned int dwBufCurLen;
    //unsigned int dwSeq;
};

/*****************************************************************************
-Class          : HlsServerSession
-Description    : HlsServerSession
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2022/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HlsServerSession
{
public:
	HlsServerSession(char * i_strPlaySrc);
	virtual ~HlsServerSession();
	int Proc();//打包线程
    int GetM3U8(char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
    int GetMP4(const char * i_strMp4Name,char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
    int GetProcFlag();
private:
    int GetStreamMapSize();
    int SaveContainerData(unsigned char * i_pbBuf, unsigned int i_dwBufLen, unsigned int i_dwDuration);
    int GetContainerData(const char * i_strMp4Name,char *o_strRes,int i_iResMaxLen);
        
    string *m_pPlaySrc;
    MediaHandle *m_pMediaHandle;
    thread * m_pSessionProc;
	int m_iSessionProcFlag;

	int m_iM3U8Seq;
	int m_iSegmentNum;
	MP4Header * m_pMP4Header;
    map<int, MP4Stream *>  m_MP4StreamMap;//string 插入排序不准，要用int
    mutex m_MapMtx;
    unsigned int m_dwMaxDuration;//ms
};













#endif
