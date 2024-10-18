/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServer.cpp
* Description           : 	    Ϊ��Ӧ�Զ����ӣ�server��Ƴɳ�פ����
ע�⣬��Ա����Ҫ�����߳̾�������
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

#define HLS_MAX_MATCH_NUM       8



#define HLS_UN_SUPPORT_FUN (-2)
#ifndef _WIN32
#include <regex.h> //C++ regexҪ��gcc 4.9���ϰ汾������linux������c��
/*****************************************************************************
-Fuction		: Regex
-Description	: ������ʽ
.��				ƥ�����\r\n��֮����κε����ַ�
*				ƥ��ǰ����ӱ��ʽ����Ρ����磬zo*��ƥ�䡰z����Ҳ��ƥ�䡰zo���Լ���zoo����*�ȼ���o{0,}
				����.*��ƥ��������洢�����������
(pattern)		ƥ��ģʽ��pattern����ȡ��һƥ�䡣����ȡ��ƥ����ԴӲ�����Matches���ϵõ�
[xyz]			�ַ����ϡ�ƥ��������������һ���ַ������磬��[abc]������ƥ�䡰plain���еġ�a����
+				ƥ��ǰ����ӱ��ʽһ�λ���(���ڵ���1�Σ������磬��zo+����ƥ�䡰zo���Լ���zoo����������ƥ�䡰z����+�ȼ���{1,}��
				//���������в���+��Ĭ����һ�Σ���ֻ��ƥ�䵽һ������6
				
[A-Za-z0-9] 	26����д��ĸ��26��Сд��ĸ��0��9����
[A-Za-z0-9+/=]	26����д��ĸ��26��Сд��ĸ0��9�����Լ�+/= �����ַ�


-Input			: i_strPattern ģʽ��,i_strBuf��ƥ���ַ���,
-Output 		: o_ptMatch �洢ƥ�䴮λ�õ�����,���ڴ洢ƥ�����ڴ�ƥ�䴮�е��±귶Χ
//����0��Ԫ�����������ʽƥ������λ��,�������������������ƥ��������ߵĵ�Ԫ���δ����������ʽƥ������λ��
-Return 		: -1 err,other cnt
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/01	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int CRegex(const char *i_strPattern,char *i_strBuf,regmatch_t *o_ptMatch,int i_iMatchMaxNum)
{
    char acErrBuf[256];
    int iRet=-1;
    regex_t tReg;    //����һ������ʵ��
    //const size_t dwMatch = 6;    //����ƥ�������������       //��ʾ������ƥ��


    //REG_ICASE ƥ����ĸʱ���Դ�Сд��
    iRet =regcomp(&tReg, i_strPattern, REG_EXTENDED);    //��������ģʽ��
    if(iRet != 0) 
    {
        regerror(iRet, &tReg, acErrBuf, sizeof(acErrBuf));
        HLS_LOGE("Regex Error:\r\n");
        return -1;
    }
    
    iRet = regexec(&tReg, i_strBuf, i_iMatchMaxNum, o_ptMatch, 0); //ƥ����
    if (iRet == REG_NOMATCH)
    { //���ûƥ����
        HLS_LOGE("Regex No Match!\r\n");
        iRet = 0;
    }
    else if (iRet == REG_NOERROR)
    { //���ƥ������
        HLS_LOGD("Match\r\n");
        int i=0,j=0;
        for(i=0;i<i_iMatchMaxNum && o_ptMatch[i].rm_so != -1;i++)
        {
            for (j= o_ptMatch[i].rm_so; j < o_ptMatch[i].rm_eo;j++)
            { //�������ƥ�䷶Χ���ַ���
                //printf("%c", i_strBuf[j]);
            }
            //printf("\n");
        }
        iRet = i;
    }
    else
    {
        HLS_LOGE("Regex Unknow err%d:\r\n",iRet);
        iRet = -1;
    }
    regfree(&tReg);  //�ͷ�������ʽ
    
    return iRet;
}
/*****************************************************************************
-Fuction		: HttpParseReqHeader
-Description	: Send
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int LinuxCRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    int iRet = -1;
	string strBuf;
	regmatch_t atMatch[HLS_MAX_MATCH_NUM];
	
    if(NULL == i_strPattern || NULL == i_strBuf || NULL == o_aMatch || i_iMatchMaxCnt <= 0 || i_iMatchMaxCnt <= 0)
    {
        HLS_LOGE("LinuxCRegex NULL \r\n");
        return iRet;
    }
    strBuf.assign(i_strBuf);
    
    memset(atMatch,0,sizeof(atMatch));
    iRet= CRegex(i_strPattern,i_strBuf,atMatch,HLS_MAX_MATCH_NUM);
    if(iRet <= 0 || i_iMatchMaxCnt < iRet)//ȥ������������-1
    {
        HLS_LOGE("LinuxCRegex %d,%d, iRet <= 0 || i_iMatchMaxCnt < iRet err \r\n",i_iMatchMaxCnt,iRet);
        return iRet;
    }
    // �������ƥ�䵽���Ӵ�
    for (int i = 0; i < iRet; ++i) 
    {
        o_aMatch[i].assign(strBuf,atMatch[i].rm_so,atMatch[i].rm_eo-atMatch[i].rm_so);//0������
    }
    return iRet;
} 
#else
/*****************************************************************************
-Fuction		: HttpParseReqHeader
-Description	: Send
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int LinuxCRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    return HLS_UN_SUPPORT_FUN;
} 
#endif







#define HLS_M3U8_MAX_LEN       (10*1024)
#define HLS_MEDIA_MAX_LEN       (3*1024*1024)

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
        iter=m_HlsSessionMap.erase(iter);// ����Ԫ�ز�������һ��Ԫ�صĵ�����
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
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Headers", "access-control-allow-headers,accessol-allow-origin,content-type");//����������������
        iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin","*");
        iRet|=pHttpServer->SetResHeaderValue("Connection", "Keep-Alive");
        iRet=pHttpServer->FormatResToStream(NULL,0,o_strRes,i_iResMaxLen);
        delete pHttpServer;
        return iRet;
    }
    if(0 == strcmp(tHttpReqPacket.strMethod,HTTP_METHOD_GET))
    {
        string astrRegex[HLS_MAX_MATCH_NUM];
        const char * strM3U8Pattern = "/([A-Za-z]+)/([A-Za-z0-9._]+)/([A-Za-z0-9_]+).m3u8";//http://localhost:9210/file/H264AAC.flv/testFMP4.m3u8
        iRet=this->HlsRegex(strM3U8Pattern,tHttpReqPacket.strURL,astrRegex,HLS_MAX_MATCH_NUM);
        if (iRet>2) //0������
        {
            string strStreamType(astrRegex[1].c_str());//file
            string strFileName(astrRegex[2].c_str());
            string strStreamPacket(astrRegex[3].c_str());
            HLS_LOGW("%d,file m_pFileName %s,strStreamPacket %s\r\n",iRet,strFileName.c_str(),strStreamPacket.c_str());
            iRet = HandleReqGetM3U8(&strFileName,&strStreamPacket,o_strRes,i_iResMaxLen);
            return iRet;
        } 
        const char * strMP4Pattern = "/([A-Za-z]+)/([A-Za-z0-9._]+)/([A-Za-z0-9_.]+).mp4";//http://localhost:9210/file/H264AAC.flv/H264AAC.flv_init.mp4
        iRet=this->HlsRegex(strMP4Pattern,tHttpReqPacket.strURL,astrRegex,HLS_MAX_MATCH_NUM);
        if (iRet>3) //0������
        {
            string strStreamType(astrRegex[1].c_str());//file
            string strSSRC(astrRegex[2].c_str());
            string strMp4FileName(astrRegex[3].c_str());
            strMp4FileName.append(".mp4");
            HLS_LOGW("%d,file mp4 strSSRC %s strMp4FileName %s\r\n",iRet,strSSRC.c_str(),strMp4FileName.c_str());
            iRet = HandleReqGetMedia(&strSSRC,&strMp4FileName,o_strRes,i_iResMaxLen);
            return iRet;
        } 
        const char * strTsPattern = "/([A-Za-z]+)/([A-Za-z0-9._]+)/([A-Za-z0-9_.]+).ts";//http://localhost:9210/file/H264AAC.flv/H264AAC.flv_init.ts
        iRet=this->HlsRegex(strTsPattern,tHttpReqPacket.strURL,astrRegex,HLS_MAX_MATCH_NUM);
        if (iRet>3) //0������
        {
            string strStreamType(astrRegex[1].c_str());//file
            string strSSRC(astrRegex[2].c_str());
            string strFileName(astrRegex[3].c_str());
            strFileName.append(".ts");
            HLS_LOGW("%d,file ts strSSRC %s strTs %s\r\n",iRet,strSSRC.c_str(),strFileName.c_str());
            iRet = HandleReqGetMedia(&strSSRC,&strFileName,o_strRes,i_iResMaxLen);
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
-Description    : SavePlaySrc �����Ż�Ϊi_pPlaySrc/i_pPlayType��
���ڲ�����ͬʱ����ͬһ�ļ���ts��fmp4��
-Input          : 
-Output         : 
-Return         : //return ResLen,<0 err
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServer::HandleReqGetM3U8(string *i_pPlaySrc,string *i_pPlayType,char *o_strRes,int i_iResMaxLen)
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
        pHlsServerSession = new HlsServerSession(i_pPlaySrc->c_str(),i_pPlayType->c_str());
        SavePlaySrc(i_pPlaySrc,pHlsServerSession);
    }
    pcM3U8 = new char [HLS_M3U8_MAX_LEN];//�����Ż�
    iM3U8Len = pHlsServerSession->GetM3U8(i_pPlayType,pcM3U8,HLS_M3U8_MAX_LEN);
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
int HlsServer::HandleReqGetMedia(string *i_pPlaySrc,string *i_pMediaName,char *o_strRes,int i_iResMaxLen)
{
    int iRet = -1;
    HlsServerSession *pHlsServerSession = NULL;
    char *pcMedia = NULL;
    int iLen = -1;

    if(NULL == i_pPlaySrc || NULL == i_pMediaName || NULL == o_strRes|| i_iResMaxLen <= 0)
    {
        HLS_LOGE("HandleReqGetMP4 NULL \r\n");
        return iRet;
    }
    
    pHlsServerSession=GetSamePlaySession(*i_pPlaySrc);
    if(NULL == pHlsServerSession)
    {
        //pHlsServerSession = new HlsServerSession((char *)i_pPlaySrc->c_str());
        //SavePlaySrc(i_pPlaySrc,pHlsServerSession);
        HLS_LOGE("HandleReqGet NULL == pHlsServerSession \r\n");
        return iRet;
    }
    pcMedia = new char [HLS_MEDIA_MAX_LEN];//�����Ż�
    if(string::npos!= i_pMediaName->find("ts"))
    {
        iLen = pHlsServerSession->GetTS(i_pMediaName->c_str(),pcMedia,HLS_MEDIA_MAX_LEN);
    }
    else
    {
        iLen = pHlsServerSession->GetMP4(i_pMediaName->c_str(),pcMedia,HLS_MEDIA_MAX_LEN);
    }
    if(iLen <= 0)
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
    iRet|=pHttpServer->SetResHeaderValue("Content-Length", iLen);
    iRet|=pHttpServer->SetResHeaderValue("Access-Control-Allow-Origin", "*");
    iRet=pHttpServer->FormatResToStream(pcMedia,iLen,o_strRes,i_iResMaxLen);
    delete pHttpServer;
    delete [] pcMedia;
    return iRet;
}

/*****************************************************************************
-Fuction        : SavePlaySrc
-Description    : ������
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
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard������������������ʱ�Զ��ͷŻ�����
    m_HlsSessionMap.insert(make_pair(*i_strPlaySrc,i_pHlsServerSession));
    return 0;
}
/*****************************************************************************
-Fuction        : FindSamePlaySrc
-Description    : ������
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
    
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard������������������ʱ�Զ��ͷŻ�����
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
-Description    : ���÷�ʽ�����Ż�������ʹ���߳���ѯ
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
    
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard������������������ʱ�Զ��ͷŻ�����
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
            iter=m_HlsSessionMap.erase(iter);// ����Ԫ�ز�������һ��Ԫ�صĵ�����
        }
        else
        {
            iter++;// ����������һ��Ԫ��
        }
    }
    return 0;
}

/*****************************************************************************
-Fuction        : HlsRegex
-Description    : 
-Input          : 
-Output         : 
-Return         : -1 err,>0 cnt
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HlsServer::HlsRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
{
    int iRet = -1;

    if(NULL == i_strPattern || NULL == i_strBuf || NULL == o_aMatch || i_iMatchMaxCnt <= 0)
    {
        HLS_LOGE("HlsRegex NULL \r\n");
        return iRet;
    }
    iRet = LinuxCRegex(i_strPattern,i_strBuf,o_aMatch,i_iMatchMaxCnt);
    if(HLS_UN_SUPPORT_FUN!=iRet)
    {
        return iRet;
    }
    string strBuf(i_strBuf);
    regex Pattern(i_strPattern);//http://localhost:9212/file/H264AAC.flv/test.m3u8
    smatch Match;
    if (std::regex_search(strBuf,Match,Pattern)) 
    {
        if((int)Match.size()<=0 || i_iMatchMaxCnt < (int)Match.size())
        {
            HLS_LOGE("HlsRegex err i_iMatchMaxCnt %d<%d Match.size()-1 \r\n",i_iMatchMaxCnt,Match.size()-1);
            return iRet;
        }
        iRet = (int)Match.size();
        // �������ƥ�䵽���Ӵ�
        for (int i = 0; i < iRet; ++i) 
        {
            o_aMatch[i].assign( Match[i].str());
        }
    } 
    return iRet;
}


