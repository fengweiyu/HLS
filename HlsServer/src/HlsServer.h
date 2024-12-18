/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServer.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_SERVER_H
#define HLS_SERVER_H

#include "HttpServer.h"
#include "HlsServerSession.h"


/*****************************************************************************
-Class          : HlsServer
-Description    : HlsServer
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HlsServer : public HttpServer
{
public:
	HlsServer();
	virtual ~HlsServer();
    int HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
    
private:
    int HandleReqGetM3U8(string *i_pPlaySrc,string *i_pPlayType,char *o_strRes,int i_iResMaxLen);
    int HandleReqGetMedia(string *i_pPlaySrc,string *i_pMediaName,char *o_strRes,int i_iResMaxLen);
    int SavePlaySrc(string *i_strPlayID,HlsServerSession *i_pHlsServerSession);
    HlsServerSession * GetSamePlaySession(string &i_strPlayID);
    int CheckPlaySrc();
    int HlsRegex(const char *i_strPattern,char *i_strBuf,string * o_aMatch,int i_iMatchMaxCnt);
    
    map<string, HlsServerSession *>  m_HlsSessionMap;
    mutex m_MapMtx;
};













#endif
