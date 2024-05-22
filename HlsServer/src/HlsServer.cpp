/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServer.cpp
* Description           : 	    为了应对短连接，server设计成常驻对象
注意，成员变量要做多线程竞争保护
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsServer.h"
#include "HlsServerCom.h"
#include <regex>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "cJSON.h"

using std::string;
using std::smatch;
using std::regex;

#define HLS_M3U8_MAX_LEN       (10*1024)
#define HLS_MP4_MAX_LEN       (3*1024*1024)

/*****************************************************************************
-Fuction        : HlsServer
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HlsServer::HlsServer()
{
    m_HlsSessionMap.clear();
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
HlsServer::~HlsServer()
{
    for (map<string, HlsServerSession *>::iterator iter = m_HlsSessionMap.begin(); iter != m_HlsSessionMap.end();)
    {
        delete iter->second;
        iter=m_HlsSessionMap.erase(iter);// 擦除元素并返回下一个元素的迭代器
    }
}
/*****************************************************************************
-Fuction        : HandleHttpReq
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServer::HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    T_HttpReqPacket tHttpReqPacket;

    CheckPlaySrc();
    if(NULL == i_strReq || NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        HLS_LOGW("HandleHttpReq NULL \r\n");
        return iRet;
    }
    memset(&tHttpReqPacket,0,sizeof(T_HttpReqPacket));
    iRet=HttpServer::ParseRequest((char *)i_strReq,strlen(i_strReq),&tHttpReqPacket);
    if(iRet < 0)
    {
        HLS_LOGE("HttpServer::ParseRequest err%d\r\n",iRet);
        return iRet;
    }
    if(0 == strcmp(tHttpReqPacket.strMethod,HTTP_METHOD_OPTIONS))
    {
        HLS_LOGW("HandleHttpReq HTTP_METHOD_OPTIONS\r\n");
        HttpServer *pHttpServer=new HttpServer();
        iRet=pHttpServer->CreateResponse();
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Method", "POST, GET, OPTIONS, DELETE, PUT");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Max-Age", "600");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Headers", "access-control-allow-headers,accessol-allow-origin,content-type");//解决浏览器跨域问题
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin","*");
        iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
        iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
        delete pHttpServer;
        return iRet;
    }
    if(0 == strcmp(tHttpReqPacket.strMethod,HTTP_METHOD_GET))
    {
        string strURL(tHttpReqPacket.strURL);
        regex M3U8Pattern("/([A-Za-z]+)/([A-Za-z0-9._]+)/([A-Za-z0-9_]+).m3u8");//http://localhost:9212/file/H264AAC.flv/test.m3u8
        smatch Match;
        if (std::regex_search(strURL,Match, M3U8Pattern)) 
        {
            string strStreamType(Match[1].str());//file
            string strFileName(Match[2].str());
            HLS_LOGW("file m_pFileName %s\r\n",strFileName.c_str());
            iRet = HandleReqGetM3U8(&strFileName,o_strRes,i_iResMaxLen);
            return iRet;
        } 
        regex MP4Pattern("/([A-Za-z]+)/([A-Za-z0-9._]+)/([A-Za-z0-9_.]+).mp4");//http://localhost:9212/file/H264AAC.flv/H264AAC.flv_init.mp4
        if (std::regex_search(strURL,Match, MP4Pattern)) 
        {
            string strStreamType(Match[1].str());//file
            string strFileName(Match[2].str());
            string strMp4Name(Match[3].str());
            strMp4Name.append(".mp4");
            HLS_LOGW("file mp4 m_pFileName %s strMp4Name %s\r\n",strFileName.c_str(),strMp4Name.c_str());
            iRet = HandleReqGetMP4(&strFileName,&strMp4Name,o_strRes,i_iResMaxLen);
            return iRet;
        } 
    }
    HLS_LOGE("unsupport HTTP_METHOD_ %s,url %s\r\n",tHttpReqPacket.strMethod,tHttpReqPacket.strURL);

    HttpServer *pHttpServer=new HttpServer();
    iRet=pHttpServer->CreateResponse(400,"bad request");
    iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
    iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
    iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
    delete pHttpServer;
    
    return iRet;
}
/*****************************************************************************
-Fuction        : HandleReqGetM3U8
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServer::HandleReqGetM3U8(string *i_pPlaySrc,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    HlsServerSession *pHlsServerSession = NULL;
    char *pcM3U8 = NULL;
    int iM3U8Len = -1;
    
    if(NULL == i_pPlaySrc || NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        HLS_LOGE("HandleReqGetM3U8 NULL \r\n");
        return iRet;
    }
    
    pHlsServerSession=GetSamePlaySession(*i_pPlaySrc);
    if(NULL == pHlsServerSession)
    {
        pHlsServerSession = new HlsServerSession((char *)i_pPlaySrc->c_str());
        SavePlaySrc(i_pPlaySrc,pHlsServerSession);
    }
    pcM3U8 = new char [HLS_M3U8_MAX_LEN];//后续优化
    iM3U8Len = pHlsServerSession->GetM3U8(pcM3U8,HLS_M3U8_MAX_LEN);
    if(iM3U8Len <= 0)
    {
        HLS_LOGE("HandleReqGetMP4 err \r\n");
        HttpServer *pHttpServer=new HttpServer();
        iRet=pHttpServer->CreateResponse(400,"bad request");
        iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
        iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
        delete pHttpServer;
        return iRet;
    }
    HttpServer *pHttpServer=new HttpServer();
    iRet=pHttpServer->CreateResponse();
    iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
    iRet|=pHttpServer->SetResHeaderValue("Content-Type", "application/vnd.apple.mpegurl");
    iRet|=pHttpServer->SetResHeaderValue("Cache-Control", "no-cache, no-store, must-revalidate");
    iRet|=pHttpServer->SetResHeaderValue("Pragma", "no-cache");
    iRet|=pHttpServer->SetResHeaderValue("Accept-Ranges", "bytes");//
    iRet|=pHttpServer->SetResHeaderValue("Content-Length", iM3U8Len);
    iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
    iRet=pHttpServer->FormatResToStream(pcM3U8,iM3U8Len,o_strRes,i_iResMaxLen);
    delete pHttpServer;
    delete [] pcM3U8;
    return iRet;
}
/*****************************************************************************
-Fuction        : HandleReqGetMP4
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServer::HandleReqGetMP4(string *i_pPlaySrc,string *i_pMp4Name,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    HlsServerSession *pHlsServerSession = NULL;
    char *pcMP4 = NULL;
    int iMP4Len = -1;

    if(NULL == i_pPlaySrc || NULL == i_pMp4Name || NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        HLS_LOGE("HandleReqGetMP4 NULL \r\n");
        return iRet;
    }
    
    pHlsServerSession=GetSamePlaySession(*i_pPlaySrc);
    if(NULL == pHlsServerSession)
    {
        pHlsServerSession = new HlsServerSession((char *)i_pPlaySrc->c_str());
        SavePlaySrc(i_pPlaySrc,pHlsServerSession);
    }
    pcMP4 = new char [HLS_MP4_MAX_LEN];//后续优化
    iMP4Len = pHlsServerSession->GetMP4(i_pMp4Name->c_str(),pcMP4,HLS_MP4_MAX_LEN);
    if(iMP4Len <= 0)
    {
        HLS_LOGE("HandleReqGetMP4 err \r\n");
        HttpServer *pHttpServer=new HttpServer();
        iRet=pHttpServer->CreateResponse(402,"Media Limited");
        iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
        iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
        delete pHttpServer;
        return iRet;
    }
    HttpServer *pHttpServer=new HttpServer();
    iRet=pHttpServer->CreateResponse();
    iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
    iRet|=pHttpServer->SetResHeaderValue("Content-Type", "video/mp2t");
    iRet|=pHttpServer->SetResHeaderValue("Accept-Ranges", "bytes");//
    iRet|=pHttpServer->SetResHeaderValue("Content-Length", iMP4Len);
    iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
    iRet=pHttpServer->FormatResToStream(pcMP4,iMP4Len,o_strRes,i_iResMaxLen);
    delete pHttpServer;
    delete [] pcMP4;
    return iRet;
}

/*****************************************************************************
-Fuction        : SavePlaySrc
-Description    : 非阻塞
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HlsServer::SavePlaySrc(string *i_strPlaySrc,HlsServerSession *i_pHlsServerSession)
{
    if(NULL == i_pHlsServerSession)
    {
        HLS_LOGE("HandleReqGetM3U8 NULL \r\n");
        return -1;
    }
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    m_HlsSessionMap.insert(make_pair(*i_strPlaySrc,i_pHlsServerSession));
    return 0;
}
/*****************************************************************************
-Fuction        : FindSamePlaySrc
-Description    : 非阻塞
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
HlsServerSession * HlsServer::GetSamePlaySession(string &i_strPlaySrc)
{
    HlsServerSession *pHlsServerSession = NULL;
    
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    if(true == m_HlsSessionMap.empty())
    {
        return pHlsServerSession;
    }
    for (map<string, HlsServerSession *>::iterator iter = m_HlsSessionMap.begin(); iter != m_HlsSessionMap.end(); ++iter)
    {
        if(i_strPlaySrc == iter->first)
        {
            pHlsServerSession = iter->second;
            break;
        }
    }
    return pHlsServerSession;
}

/*****************************************************************************
-Fuction        : CheckPlaySrc
-Description    : 调用方式后续优化，考虑使用线程轮询
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HlsServer::CheckPlaySrc()
{
    HlsServerSession *pHlsServerSession = NULL;
    
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard对象会在其作用域结束时自动释放互斥量
    if(true == m_HlsSessionMap.empty())
    {
        return 0;
    }
    for (map<string, HlsServerSession *>::iterator iter = m_HlsSessionMap.begin(); iter != m_HlsSessionMap.end(); )
    {
        pHlsServerSession = iter->second;
        if(0 == pHlsServerSession->GetProcFlag())
        {
            delete pHlsServerSession;
            iter=m_HlsSessionMap.erase(iter);// 擦除元素并返回下一个元素的迭代器
        }
        else
        {
            iter++;// 继续遍历下一个元素
        }
    }
    return 0;
}


