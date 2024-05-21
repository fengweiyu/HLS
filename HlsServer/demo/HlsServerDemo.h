/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerDemo.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_SERVER_DEMO_H
#define HLS_SERVER_DEMO_H

#include <mutex>
#include <string>
#include <list>
#include <map>
#include "HlsServerIO.h"

using std::map;
using std::string;
using std::list;
using std::mutex;

/*****************************************************************************
-Class			: WebRtcServer
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HlsServerDemo : public TcpServer
{
public:
	HlsServerDemo(int i_iServerPort);
	virtual ~HlsServerDemo();
    int Proc();
    
private:
    int CheckMapServerIO();
    int AddMapServerIO(HlsServerIO * i_pHlsServerIO,int i_iClientSocketFd);
    
    map<int, HlsServerIO *>  m_HlsServerIOMap;
    mutex m_MapMtx;
};

#endif
