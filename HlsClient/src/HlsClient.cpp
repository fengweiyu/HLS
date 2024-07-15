/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClient.cpp
* Description           : 	    为了应对短连接，server设计成常驻对象
注意，成员变量要做多线程竞争保护
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
#include <regex.h> //C++ regex要求gcc 4.9以上版本，所以linux还是用c的
/*****************************************************************************
-Fuction		: Regex
-Description	: 正则表达式
.点				匹配除“\r\n”之外的任何单个字符
*				匹配前面的子表达式任意次。例如，zo*能匹配“z”，也能匹配“zo”以及“zoo”。*等价于o{0,}
				其中.*的匹配结果不会存储到结果数组里
(pattern)		匹配模式串pattern并获取这一匹配。所获取的匹配可以从产生的Matches集合得到
[xyz]			字符集合。匹配所包含的任意一个字符。例如，“[abc]”可以匹配“plain”中的“a”。
+				匹配前面的子表达式一次或多次(大于等于1次）。例如，“zo+”能匹配“zo”以及“zoo”，但不能匹配“z”。+等价于{1,}。
				//如下例子中不用+，默认是一次，即只能匹配到一个数字6
				
[A-Za-z0-9] 	26个大写字母、26个小写字母和0至9数字
[A-Za-z0-9+/=]	26个大写字母、26个小写字母0至9数字以及+/= 三个字符


-Input			: i_strPattern 模式串,i_strBuf待匹配字符串,
-Output 		: o_ptMatch 存储匹配串位置的数组,用于存储匹配结果在待匹配串中的下标范围
//数组0单元存放主正则表达式匹配结果的位置,即所有正则组合起来的匹配结果，后边的单元依次存放子正则表达式匹配结果的位置
-Return 		: -1 err,other cnt
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/11/01	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int CRegex(const char *i_strPattern,char *i_strBuf,regmatch_t *o_ptMatch,int i_iMatchMaxNum)
{
    char acErrBuf[256];
    int iRet=-1;
    regex_t tReg;    //定义一个正则实例
    //const size_t dwMatch = 6;    //定义匹配结果最大允许数       //表示允许几个匹配


    //REG_ICASE 匹配字母时忽略大小写。
    iRet =regcomp(&tReg, i_strPattern, REG_EXTENDED);    //编译正则模式串
    if(iRet != 0) 
    {
        regerror(iRet, &tReg, acErrBuf, sizeof(acErrBuf));
        HLS_LOGE("Regex Error:\r\n");
        return -1;
    }
    
    iRet = regexec(&tReg, i_strBuf, i_iMatchMaxNum, o_ptMatch, 0); //匹配他
    if (iRet == REG_NOMATCH)
    { //如果没匹配上
        HLS_LOGE("Regex No Match!\r\n");
        iRet = 0;
    }
    else if (iRet == REG_NOERROR)
    { //如果匹配上了
        HLS_LOGD("Match\r\n");
        int i=0,j=0;
        for(i=0;i<i_iMatchMaxNum && o_ptMatch[i].rm_so != -1;i++)
        {
            for (j= o_ptMatch[i].rm_so; j < o_ptMatch[i].rm_eo;j++)
            { //遍历输出匹配范围的字符串
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
    regfree(&tReg);  //释放正则表达式
    
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
    if(iRet <= 0 || i_iMatchMaxCnt < iRet)//去掉整串，所以-1
    {
        HLS_LOGE("LinuxCRegex %d,%d, iRet <= 0 || i_iMatchMaxCnt < iRet err \r\n",i_iMatchMaxCnt,iRet);
        return iRet;
    }
    // 输出所有匹配到的子串
    for (int i = 0; i < iRet; ++i) 
    {
        o_aMatch[i].assign(strBuf,atMatch[i].rm_so,atMatch[i].rm_eo-atMatch[i].rm_so);//0是整行
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
        // 输出所有匹配到的子串
        for (int i = 0; i < iRet; ++i) 
        {
            o_aMatch[i].assign( Match[i].str());
        }
    } 
    return iRet;
}


