/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClient.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_CLIENT_H
#define HLS_CLIENT_H

#include "HlsClientSession.h"
#include "HttpClient.h"


/*****************************************************************************
-Class          : HlsServer
-Description    : HlsServer
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HlsClient : public HttpClient
{
public:
	HlsClient();
	virtual ~HlsClient();
    int ParseHttpURL(const char * i_strHttpURL,string *o_strIP,int *o_iPort,string * o_strURL);
    int GenerateGetM3U8Req(const char * i_strM3U8URL,char *o_strReq,int i_iReqMaxLen);//return ReqLen,<0 err
    int HandleHttpM3U8(char * i_strHttpM3U8,int *m_iOffset,int * o_iTryTime,string *o_strFileName,char *o_strReq,int i_iReqMaxLen);//考虑和下面接口合并
    int ParseHttpM3U8(char * i_strHttpM3U8,int * o_iMaxDurationTime);
    int GetFileInfo(int * o_iDurationTime,string *o_strFileName,char *o_strReq,int i_iReqMaxLen);
    int HandleHttpMedia(char * i_pcHttpMedia,int i_iMediaLen,char **o_ppMediaData,int *o_iDataLen);
private:
    int HlsRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt);
    
    HlsClientSession m_HlsClientSession;
    char * m_strM3U8;//暂时放栈里
};













#endif
