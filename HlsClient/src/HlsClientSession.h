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
-Class          : M3U8File
-Description    : M3U8File
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2022/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class M3U8File
{
public:
	M3U8File(){strName.clear();};
	virtual ~M3U8File(){};
    string strName;
    unsigned int dwDuration;//s
};

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
    int ParseM3U8(const char * i_strM3U8);
    int GetFileInfo(int * o_iDurationTime,string * o_strFileName,string * o_strFileURL);
    int SetMediaLocation(const char *i_strLocate);
    int GetMaxDurationTime(const char * i_strM3U8,int * o_iMaxDurationTime);
private:
    int FindSameFileName(M3U8File *i_pM3U8File);

	list<M3U8File> m_M3U8FileList;//将要下载文件的列表
	list<M3U8File> m_M3U8FileHandledList;//已经下载过的文件列表
    string *m_pMediaLocation;
};













#endif
