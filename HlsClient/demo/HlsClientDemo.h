/*****************************************************************************
* Copyright (C) 2023-2028 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClientDemo.h
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_CLIENT_DEMO_H
#define HLS_CLIENT_DEMO_H

#include <mutex>
#include <string>
#include <list>
#include <map>
#include "HlsClientIO.h"

using std::map;
using std::string;
using std::list;
using std::mutex;

/*****************************************************************************
-Class			: HlsClientDemo
-Description	: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2019/09/21	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
class HlsClientDemo
{
public:
	HlsClientDemo();
	virtual ~HlsClientDemo();
    int Proc(const char * i_strHttpURL);
    
private:
    HlsClientIO m_HlsClientIO;
};

#endif
