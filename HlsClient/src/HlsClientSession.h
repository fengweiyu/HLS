/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClientSession.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_CLIENT_SESSION_H
#define HLS_CLIENT_SESSION_H

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



/*****************************************************************************
-Class          : HlsClientSession
-Description    : HlsClientSession
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2022/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HlsClientSession
{
public:
	HlsClientSession();
	virtual ~HlsClientSession();
    int HandleM3U8(const char * i_strM3U8,int *m_iOffset,int * o_iInfTime,string *o_strFileName);
    int GetFileURL(const char * i_strFileName,string * o_strFileURL);
    int SetMediaLocation(const char *i_strLocate);

private:
        
    string *m_pMediaLocation;
};













#endif
