/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClientIO.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_SERVER_IO_H
#define HLS_SERVER_IO_H

#include "TcpSocket.h"
#include "HlsClientInf.h"
#include "HlsClientCom.h"
#include <thread>
#include <mutex>

using std::thread;
using std::mutex;

/*****************************************************************************
-Class			: HlsClientIO
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HlsClientIO : public TcpClient
{
public:
	HlsClientIO();
	virtual ~HlsClientIO();
    int Proc(const char * i_strHttpURL,const char *i_strFlag=NULL);
    int GetProcFlag();
private:
    int SaveFile(const char * strFileName,char *i_pData,int i_iLen);
    int CreateSaveDir(const char * i_strLocation,string *o_strSaveDir);

    HlsClientInf * m_pHlsClient;
	int m_iClientSocketFd;

	int m_iHlsClientIOFlag;
};

#endif
