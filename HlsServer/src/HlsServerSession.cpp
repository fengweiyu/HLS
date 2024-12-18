/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerSession.cpp
* Description           : 	
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsServerSession.h"
#include "HlsServerCom.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <functional>

//#include <sys/time.h>
//#include "cJSON.h"

#define HLS_FRAME_BUF_MAX_LEN	(2*1024*1024) 

/*****************************************************************************
-Fuction        : HlsServerSession
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HlsServerSession::HlsServerSession(const char * i_strPlaySrc,const char * i_strPlayType)
{
    m_iM3U8Seq=0;
    m_dwMaxDuration = 0;

    m_iSegmentNum = 0;
    m_pMP4Header = new MP4Header();
    m_pPlaySrc = new string(i_strPlaySrc);
    m_pPlayType = new string(i_strPlayType);
    m_pMediaHandle = new MediaHandle();
    m_iSessionProcFlag = 0;
    //m_pSessionProc = new thread(&HlsServerSession::Proc, this,0,4*1024*1024);//这种方式需要线程函数带参数
    m_pSessionProc = new thread(std::bind(&HlsServerSession::Proc, this),2*1024*1024);//指定栈大小2MB,win 默认1MB
    //m_pSessionProc->detach();//注意线程回收
}
/*****************************************************************************
-Fuction        : ~HlsServer
-Description    : ~HlsServer
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HlsServerSession::~HlsServerSession()
{
    delete m_pMP4Header;
    delete m_pPlaySrc;
    delete m_pMediaHandle;
    if(NULL!= m_pSessionProc)
    {
        HLS_LOGW("HLSSession start exit\r\n");
        m_iSessionProcFlag = 0;//m_pWebRTC->StopProc();
        m_pSessionProc->join();//自己join自己肯定不行
        delete m_pSessionProc;
        m_pSessionProc = NULL;
    }
}
/*****************************************************************************
-Fuction        : Proc
-Description    : 使用视频帧时间戳休眠来打包，耗时会比段时间久，播放器后面都会卡
使用段(gop)时间来休眠打包，耗时准但是不够稳定，对于追最新包的播放器来说，
    会跳包拿最新的包，但是最新的包有时打的会慢一些，则会造成卡顿
    但是对于有缓存的播放器，还是比较流畅
后续可以改为请求数据的时候立马打包，这样打包的逻辑处理是比较快的
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServerSession::Proc()
{
    int iRet = -1;
    T_MediaFrameInfo tFileFrameInfo;
	unsigned int dwFileLastTimeStamp=0;
	unsigned int dwLastSegTimeStamp=0;
	int iSleepTimeMS=0;
    unsigned char * pbContainerBuf=NULL;
	int iContainerHeaderLen=0;
    E_StreamType eStreamType=STREAM_TYPE_FMP4_STREAM;
	unsigned int dwFileCurTick=0;
	unsigned int dwFileLastTick=0;
	unsigned int dwSleepStamp=0;

    m_iSessionProcFlag = 1;
    HLS_LOGW("HlsServerSession start Proc %s\r\n",m_pPlaySrc->c_str());
    
    pbContainerBuf = new unsigned char [HLS_MP4_BUF_MAX_LEN];
    memset(&tFileFrameInfo,0,sizeof(T_MediaFrameInfo));
    tFileFrameInfo.pbFrameBuf = new unsigned char [HLS_FRAME_BUF_MAX_LEN];
    tFileFrameInfo.iFrameBufMaxLen = HLS_FRAME_BUF_MAX_LEN;
    m_pMediaHandle->Init((char *)m_pPlaySrc->c_str());//默认取文件流
    if(string::npos!=m_pPlayType->find("TS"))
    {
        eStreamType=STREAM_TYPE_TS_STREAM;
    }
    while(m_iSessionProcFlag)
    {
        iRet=m_pMediaHandle->GetFrame(&tFileFrameInfo);//非文件流可直接调用此接口
        if(iRet<0)
        {
            HLS_LOGE("TestProc exit %d[%s]\r\n",iRet,m_pPlaySrc->c_str());
            break;
        }
        if(tFileFrameInfo.dwTimeStamp<dwFileLastTimeStamp)
        {
            HLS_LOGE("dwTimeStamp err exit %d,%d\r\n",tFileFrameInfo.dwTimeStamp,dwFileLastTimeStamp);
            break;
        }
        if(0 == dwFileLastTimeStamp && 0 == dwFileLastTick)
        {
            dwFileLastTimeStamp=tFileFrameInfo.dwTimeStamp;
            dwFileLastTick = GetTickCount();
            dwSleepStamp=tFileFrameInfo.dwTimeStamp;
        }
        if(tFileFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_I_FRAME||tFileFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_P_FRAME||tFileFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_B_FRAME)
        {//音视频时间可能不同源，所以要判断
            dwSleepStamp=tFileFrameInfo.dwTimeStamp;//即使同源，有可能也是不准的，以视频的为准。这样还是会慢一点
        }//如果只判断I帧则和下面使用segment时间休眠效果一样，
        iSleepTimeMS=(int)(dwSleepStamp-dwFileLastTimeStamp);
        dwFileCurTick=GetTickCount();
        if((int)(dwFileCurTick-dwFileLastTick) <= iSleepTimeMS)
        {
            //SleepMs((iSleepTimeMS-(dwFileCurTick-dwFileLastTick)));//模拟实时流(直播)，点播和当前的处理机制不匹配，需要后续再开发
        }
        dwFileLastTick = GetTickCount();
        iContainerHeaderLen = 0;
        iRet=m_pMediaHandle->FrameToContainer(&tFileFrameInfo,eStreamType,pbContainerBuf,HLS_MP4_BUF_MAX_LEN,&iContainerHeaderLen);
        if(iRet<0)
        {
            HLS_LOGE("FrameToContainer err exit %d[%s]\r\n",iRet,m_pPlaySrc->c_str());
            break;
        }
        if(iRet>0)
        {
            if(iContainerHeaderLen>0)//m_pMediaHandle->GetMuxHeader
            {
                memcpy(m_pMP4Header->abBuf,pbContainerBuf,iContainerHeaderLen);
                m_pMP4Header->dwBufCurLen=iContainerHeaderLen;
                SaveContainerData(pbContainerBuf+iContainerHeaderLen,iRet-iContainerHeaderLen,(tFileFrameInfo.dwTimeStamp-dwLastSegTimeStamp));
            }
            else
            {
                SaveContainerData(pbContainerBuf,iRet,(tFileFrameInfo.dwTimeStamp-dwLastSegTimeStamp));
            }
            HLS_LOGD("%d FrameToContainer [%d]\r\n",(dwFileLastTick&0x7fffffff)/1000,m_iSegmentNum);
            iSleepTimeMS=(int)(tFileFrameInfo.dwTimeStamp-dwLastSegTimeStamp);
            if(iSleepTimeMS > 0)
            {
                SleepMs((iSleepTimeMS-0));//这样更准，比客户端取流过来的时间会快一点
            }
            dwLastSegTimeStamp = tFileFrameInfo.dwTimeStamp;
        }
        if(tFileFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_I_FRAME||tFileFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_P_FRAME||tFileFrameInfo.eFrameType == MEDIA_FRAME_TYPE_VIDEO_B_FRAME)
        {
            dwFileLastTimeStamp = tFileFrameInfo.dwTimeStamp;
        }
    }
    if(NULL!= pbContainerBuf)
    {
        delete[] pbContainerBuf;
        pbContainerBuf = NULL;//
    }
    if(NULL!= tFileFrameInfo.pbFrameBuf)
    {
        delete[] tFileFrameInfo.pbFrameBuf;
        tFileFrameInfo.pbFrameBuf = NULL;//
    }
    m_iSessionProcFlag = 0;
    return iRet;
}
/*****************************************************************************
-Fuction        : GetM3U8
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServerSession::GetM3U8(string *i_pPlayType,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    char strMp4List[256];
    int iListLen=0;
    unsigned int dwMaxDuration=0;//ms
    float  fTime = 0;
    int iStreamType=7;//7 fmp4,3 ts

    if(NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        HLS_LOGE("GetM3U8 NULL \r\n");
        return iRet;
    }
    while(0 == GetProcFlag())
    {
        SleepMs(10);//防止线程一开始还没启动
        if(0 == GetProcFlag())
        {
            HLS_LOGE("GetM3U8 exit \r\n");
            return iRet;
        }
    }

    while(GetStreamMapSize()<3)
    {
        SleepMs(10);
    }
    
    if(string::npos!=m_pPlayType->find("TS"))
    {
        iStreamType=3;
    }
    memset(strMp4List,0,sizeof(strMp4List));
    //if(0 == m_iM3U8Seq)//ios 要求每段前面都要有moov，否则无法播放
    if(7==iStreamType)
    {
        iListLen = snprintf(strMp4List,sizeof(strMp4List),"#EXT-X-MAP:URI=\"%s_init.mp4\",\r\n",m_pPlaySrc->c_str());
    }
    
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    for (map<int, MP4Stream*>::iterator iter = m_MP4StreamMap.begin(); iter != m_MP4StreamMap.end();iter++)
    {
        if(iter->second->dwDuration > dwMaxDuration)
        {
            dwMaxDuration = iter->second->dwDuration;
        }
        fTime = (float)(iter->second->dwDuration / 1000.0f);
        iListLen += snprintf(strMp4List+iListLen,sizeof(strMp4List)-iListLen,"#EXTINF:%0.3f,\r\n%s\r\n",fTime,iter->second->strName.c_str());
    }
    iRet = snprintf(o_strRes,i_iResMaxLen,
        "#EXTM3U\r\n"
        "#EXT-X-VERSION:%d\r\n"
        "#EXT-X-ALLOW-CACHE:NO\r\n"
        "#EXT-X-MEDIA-SEQUENCE:%d\r\n"
        "#EXT-X-TARGETDURATION:%d\r\n"
        "%s",
        iStreamType,//7 fmp4,3 ts
        ++m_iM3U8Seq,dwMaxDuration/1000,strMp4List);//考虑让m_iM3U8Seq使用第一个包的iter->second->dwSeq
    
    return iRet;
}
/*****************************************************************************
-Fuction        : GetMP4
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServerSession::GetMP4(const char * i_strMp4Name,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    char strName[128];
    
    if(NULL == i_strMp4Name|| NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        HLS_LOGE("GetMP4 NULL \r\n");
        return iRet;
    }
    if(0 == GetProcFlag())
    {
        HLS_LOGE("GetMP4 exit \r\n");
        return iRet;
    }
    HLS_LOGD("GetMP4 i_strMp4Name %s \r\n",i_strMp4Name);
    memset(strName,0,sizeof(strName));
    snprintf(strName,sizeof(strName),"%s_init.mp4",m_pPlaySrc->c_str());
    if(0 == strcmp(strName,i_strMp4Name))
    {
        if(m_pMP4Header->dwBufCurLen>0 && m_pMP4Header->dwBufCurLen<=i_iResMaxLen)
        {
            memcpy(o_strRes,m_pMP4Header->abBuf,m_pMP4Header->dwBufCurLen);
            iRet = m_pMP4Header->dwBufCurLen;
        }
        else
        {
            HLS_LOGE("GetMP4   m_pMP4Header->dwBufCurLen%d,i_iResMaxLen %d err \r\n",m_pMP4Header->dwBufCurLen,i_iResMaxLen);
        }
    }
    else
    {
        iRet = GetContainerData(i_strMp4Name,o_strRes,i_iResMaxLen);
    }
    return iRet;
}
/*****************************************************************************
-Fuction        : GetTS
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServerSession::GetTS(const char * i_strName,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    
    if(NULL == i_strName|| NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        HLS_LOGE("GetTS NULL \r\n");
        return iRet;
    }
    if(0 == GetProcFlag())
    {
        HLS_LOGE("GetTS exit \r\n");
        return iRet;
    }
    HLS_LOGD("GetTS i_strName %s \r\n",i_strName);
    iRet = GetContainerData(i_strName,o_strRes,i_iResMaxLen);
    return iRet;
}

/*****************************************************************************
-Fuction        : SaveContainerData
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServerSession::GetStreamMapSize()
{
    int iRet = -1;
    
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    iRet =m_MP4StreamMap.size() ;
    return iRet;
}

/*****************************************************************************
-Fuction        : SaveContainerData
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServerSession::SaveContainerData(unsigned char * i_pbBuf, unsigned int i_dwBufLen, unsigned int i_dwDuration)
{
    int iRet = -1;
    char strName[128];
    string strSegName;
    MP4Stream *pMP4Stream = new MP4Stream();
    
    if(NULL == pMP4Stream ||NULL == i_pbBuf|| i_dwBufLen <= 0)
    {
        HLS_LOGE("SaveContainerData NULL \r\n");
        return iRet;
    }
    memset(strName,0,sizeof(strName));
    if(string::npos!=m_pPlayType->find("TS"))
    {
        snprintf(strName,sizeof(strName),"%s_%d.ts",m_pPlaySrc->c_str(),++m_iSegmentNum);
    }
    else
    {
        snprintf(strName,sizeof(strName),"%s_%d.mp4",m_pPlaySrc->c_str(),++m_iSegmentNum);
    }
    strSegName.assign(strName);
    memcpy(pMP4Stream->abBuf,i_pbBuf,i_dwBufLen);
    pMP4Stream->dwBufCurLen=i_dwBufLen;
    pMP4Stream->dwDuration=i_dwDuration;
    pMP4Stream->strName.assign(strName);
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    if(m_MP4StreamMap.size()>=3)//>=3
    {
        map<int, MP4Stream*>::iterator iter = m_MP4StreamMap.begin();
        MP4Stream *pStream = iter->second;
        delete pStream;
        m_MP4StreamMap.erase(iter);
    }
    //map按照键值strSegName从小到大排序
    m_MP4StreamMap.insert(std::make_pair(m_iSegmentNum,pMP4Stream));//要使用MP4Stream *，MP4Stream会把数据拷贝到栈里

    return 0;
}

/*****************************************************************************
-Fuction        : GetContainerData
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServerSession::GetContainerData(const char * i_strName,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    
    if(NULL == i_strName ||NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        HLS_LOGE("GetContainerData NULL \r\n");
        return iRet;
    }
    string strStreamName(i_strName);
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    for (map<int, MP4Stream*>::iterator iter = m_MP4StreamMap.begin(); iter != m_MP4StreamMap.end();)
    {
        if(strStreamName == iter->second->strName)
        {
            HLS_LOGW("GetContainerData %s\r\n", strStreamName.c_str());
            if(iter->second->dwBufCurLen>0 && iter->second->dwBufCurLen<=i_iResMaxLen)
            {
                memcpy(o_strRes,iter->second->abBuf,iter->second->dwBufCurLen);
                iRet = iter->second->dwBufCurLen;
            }
            else
            {
                HLS_LOGE("GetMP4   iter->second->dwBufCurLen%d,i_iResMaxLen %d err \r\n",iter->second->dwBufCurLen,i_iResMaxLen);
            }
            break;
        }
        else
        {
            iter++;// 继续遍历下一个元素
        }
    }
    //map<int, MP4Stream*>::iterator iter = m_MP4StreamMap.find(dwSeq);//后续优化为strMp4StreamName取出序列号然后查找
    //if (iter == m_MP4StreamMap.end()){iRet=-1;};
    if(iRet<=0)
    {//没找到则用最近的数据
        map<int, MP4Stream *>::iterator iter = m_MP4StreamMap.begin();//map按照键值first从小到大排序
        HLS_LOGW("GetContainerData no find %s,%d,%s\r\n", strStreamName.c_str(),iter->first,iter->second->strName.c_str());
        if(iter->second->dwBufCurLen>0 && iter->second->dwBufCurLen<=i_iResMaxLen)
        {
            memcpy(o_strRes,iter->second->abBuf,iter->second->dwBufCurLen);
            iRet = iter->second->dwBufCurLen;
        }
        else
        {
            HLS_LOGE("GetContainerData   err iter->second->dwBufCurLen%d,i_iResMaxLen %d err \r\n",iter->second->dwBufCurLen,i_iResMaxLen);
        }
    }
    return iRet;
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
int HlsServerSession :: GetProcFlag()
{
    return m_iSessionProcFlag;//多线程竞争注意优化
}
/*****************************************************************************
-Fuction        : GetTickCount
-Description    : // 返回自系统开机以来的毫秒数
-Input          : 
-Output         : 
-Return         : len
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
unsigned int HlsServerSession::GetTickCount()	// milliseconds
{
#ifdef _WIN32
	return ::GetTickCount64() & 0xFFFFFFFF;
#else  
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	uint64_t cnt = ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
	return cnt & 0xFFFFFFFF;
#endif
}

