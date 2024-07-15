/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClient.cpp
* Description           : 	    Ϊ��Ӧ�Զ����ӣ�server��Ƴɳ�פ����
ע�⣬��Ա����Ҫ�����߳̾�������
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsClient.h"
#include "HlsClientCom.h"
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
HlsClient::HlsClient()
{
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
HlsClient::~HlsClient()
{
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
int HlsClient::ParseHttpURL(const char * i_strHttpURL,string *o_strIP,int *o_iPort,string * o_strURL)
{
    int iRet = -1;
    string strHttpURL;
    
    if(NULL == i_strHttpURL || NULL == o_strIP || NULL == o_iPort || NULL == o_strURL)
    {
        HLS_LOGE("HandleHttpReq NULL \r\n");
        return iRet;
    }
    if(string::npos == strHttpURL.find(".m3u8"))//
    {
        HLS_LOGE("strHttpURL.find m3u8 err \r\n");
        return iRet;
    }
    
    strHttpURL.assign(i_strHttpURL);
    auto dwHttpPos = strHttpURL.find("http://");
    if(string::npos == dwHttpPos)//
    {
        HLS_LOGE("strHttpURL.find http://err \r\n");
        return iRet;
    }
    auto dwIpEndPos = strHttpURL.substr(dwHttpPos+strlen("http://")).find(":");
    if(string::npos == dwIpEndPos)//
    {
        HLS_LOGE("strHttpURL.find dwIpEndPos err \r\n");
        return iRet;
    }
    o_strIP->assign(strHttpURL.substr(dwHttpPos+strlen("http://"),dwIpEndPos-(dwHttpPos+strlen("http://"))).c_str());
    auto dwPortEndPos = strHttpURL.substr(dwIpEndPos+strlen(":")).find("/");
    if(string::npos == dwPortEndPos)//
    {
        HLS_LOGE("strHttpURL.find dwPortEndPos err \r\n");
        return iRet;
    }
    o_strIP->assign(strHttpURL.substr(dwIpEndPos+strlen(":"),dwPortEndPos-(dwIpEndPos+strlen(":"))).c_str());
    o_strURL->assign(strHttpURL.substr(dwPortEndPos).c_str());
    auto dwLocatePos = strHttpURL.substr(dwPortEndPos+strlen("/")).find("/");
    if(string::npos == dwLocatePos)//
    {
        HLS_LOGE("strHttpURL.find dwLocatePos err \r\n");
        return iRet;
    }
    auto dwLocateEndPos = strHttpURL.substr(dwLocatePos+strlen("/")).find("/");
    if(string::npos == dwLocateEndPos)//
    {
        HLS_LOGE("strHttpURL.find dwLocateEndPos err \r\n");
        return iRet;
    }
    m_HlsClientSession.SetMediaLocation(strHttpURL.substr(dwPortEndPos,dwLocateEndPos-dwPortEndPos+strlen("/")).c_str());
    return 0;
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
int HlsClient::GenerateGetM3U8Req(const char * i_strM3U8URL,char *o_strReq,int i_iReqMaxLen)
{
    int iRet = -1;
    char *pcM3U8 = NULL;
    int iM3U8Len = -1;
    
    if(NULL == i_strM3U8URL || NULL == o_strReq|| i_iReqMaxLen <= 0)
    {
        HLS_LOGE("GenerateGetM3U8Req NULL \r\n");
        return iRet;
    }
    iRet=HttpClient::CreateRequest("GET",i_strM3U8URL);
    iRet=HttpClient::SetReqHeaderValue("User-Agent","ywfPalyer");
    iRet=HttpClient::SetReqHeaderValue("Accept","*/*");
    iRet=HttpClient::SetReqHeaderValue("Range","bytes=0-");
    iRet=HttpClient::SetReqHeaderValue("Connection","keep-alive");// close
    iRet=HttpClient::FormatReqToStream(NULL,0,o_strReq,i_iReqMaxLen);
    return iRet;
}
/*****************************************************************************
-Fuction        : HandleHttpM3U8
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsClient::HandleHttpM3U8(char * i_strHttpM3U8,int *m_iOffset,int * o_iTryTime,string *o_strFileName,char *o_strReq,int i_iReqMaxLen)
{
    int iRet = -1;
    char strM3U8[512];
    int iM3U8Len = 0;
    T_HttpResPacket tHttpResPacket;
    string strFileName("");
    string strFileURL("");
    
    if(NULL == i_strHttpM3U8 || NULL == o_strFileName|| NULL == o_strReq|| i_iReqMaxLen <= 0)
    {
        HLS_LOGE("HandleHttpM3U8 NULL \r\n");
        return iRet;
    }
    memset(&strM3U8,0,sizeof(strM3U8));
    memset(&tHttpResPacket,0,sizeof(T_HttpResPacket));
    tHttpResPacket.pcBody=strM3U8;
    tHttpResPacket.iBodyMaxLen=(int)sizeof(strM3U8);
    iRet=HttpClient::ParseResponse(i_strHttpM3U8,strlen(i_strHttpM3U8),&tHttpResPacket);
    if(iRet < 0)
    {
        HLS_LOGE("HttpClient::ParseResponse err \r\n");
        return iRet;
    }
    if(NULL == strstr(tHttpResPacket.strContentType,"apple.mpegurl"))
    {
        HLS_LOGE("HttpClient::tHttpResPacket.strContentType err %s\r\n",tHttpResPacket.strContentType);
    }
    iRet=m_HlsClientSession.HandleM3U8((const char *)tHttpResPacket.pcBody,m_iOffset,o_iTryTime,&strFileName);
    if(iRet < 0)
    {
        HLS_LOGE("HttpClient::HandleM3U8 err \r\n");
        return iRet;
    }
    iRet=m_HlsClientSession.GetFileURL(strFileName.c_str(),&strFileURL);
    if(iRet < 0)
    {
        HLS_LOGE("HttpClient::GetFileURL err \r\n");
        return iRet;
    }

    iRet=HttpClient::CreateRequest("GET",strFileURL.c_str());
    iRet=HttpClient::SetReqHeaderValue("User-Agent","ywfPalyer");
    iRet=HttpClient::SetReqHeaderValue("Accept","*/*");
    iRet=HttpClient::SetReqHeaderValue("Range","bytes=0-");
    iRet=HttpClient::SetReqHeaderValue("Connection","keep-alive");

    iRet=HttpClient::FormatReqToStream(NULL,0,o_strReq,i_iReqMaxLen);
    o_strFileName->assign(strFileName.c_str());
    return iRet;
}
/*****************************************************************************
-Fuction        : HandleHttpM3U8
-Description    : Content-Type: video/mp2t
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsClient::HandleHttpMedia(char * i_strHttpMedia,int i_iMediaLen,char **o_ppMediaData,int *o_iDataLen)
{
    int iRet = -1;
    T_HttpResPacket tHttpResPacket;
    
    if(NULL == i_strHttpMedia || NULL == o_ppMediaData|| NULL == o_iDataLen|| i_iMediaLen <= 0)
    {
        HLS_LOGE("HandleHttpMedia NULL \r\n");
        return iRet;
    }
    memset(&tHttpResPacket,0,sizeof(T_HttpResPacket));
    iRet=HttpClient::ParseResponse(i_strHttpMedia,i_iMediaLen,&tHttpResPacket);
    if(iRet < 0)
    {
        HLS_LOGE("HandleHttpMedia::ParseResponse err \r\n");
        return iRet;
    }
    if(tHttpResPacket.iContentLength!=tHttpResPacket.iBodyCurLen)
    {
        HLS_LOGE("tHttpResPacket.iContentLength%d!=tHttpResPacket.iBodyCurLen%d\r\n",tHttpResPacket.iContentLength,tHttpResPacket.iBodyCurLen);
    }
    *o_ppMediaData=tHttpResPacket.pcBody;
    *o_iDataLen=tHttpResPacket.iBodyCurLen;
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
int HlsClient::HlsRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt)
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


