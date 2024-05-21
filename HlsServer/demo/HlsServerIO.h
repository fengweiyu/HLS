/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerIO.h
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
#include "HlsServerInf.h"
#include "HlsServerCom.h"
#include <thread>
#include <mutex>

using std::thread;
using std::mutex;

/*****************************************************************************
-Class			: HlsServerIO
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HlsServerIO : TcpServer
{
public:
	HlsServerIO(int i_iClientSocketFd,HlsServerInf * i_pHlsServer);
	virtual ~HlsServerIO();
    int Proc();
    int GetProcFlag();
private:
    HlsServerInf * m_pHlsServer;
	int m_iClientSocketFd;

    thread * m_pHlsServerIOProc;
	int m_iHlsServerIOFlag;
};

#endif
