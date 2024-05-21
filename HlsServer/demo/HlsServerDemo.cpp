/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerDemo.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsServerDemo.h"
#include "HlsServerInf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <utility>

using std::make_pair;

/*****************************************************************************
-Fuction		: HlsServerDemo
-Description	: HlsServerDemo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HlsServerDemo :: HlsServerDemo(int i_iServerPort)
{
    TcpServer::Init(NULL,i_iServerPort);
}

/*****************************************************************************
-Fuction		: ~HlsServerDemo
-Description	: ~HlsServerDemo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HlsServerDemo :: ~HlsServerDemo()
{
}

/*****************************************************************************
-Fuction		: Proc
-Description	: ����
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HlsServerDemo :: Proc()
{
    int iClientSocketFd=-1;
    HlsServerInf *pHlsServer = new HlsServerInf();
    HlsServerIO *pHlsServerIO = NULL;
    while(1)
    {
        iClientSocketFd=TcpServer::Accept();
        if(iClientSocketFd<0)  
        {  
            SleepMs(10);
            CheckMapServerIO();
            continue;
        } 
        pHlsServerIO = new HlsServerIO(iClientSocketFd,pHlsServer);
        AddMapServerIO(pHlsServerIO,iClientSocketFd);
    }
    delete pHlsServer;
    return 0;
}

/*****************************************************************************
-Fuction        : CheckMapServerIO
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HlsServerDemo::CheckMapServerIO()
{
    int iRet = -1;
    HlsServerIO *pHlsServerIO=NULL;

    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard������������������ʱ�Զ��ͷŻ�����
    for (map<int, HlsServerIO *>::iterator iter = m_HlsServerIOMap.begin(); iter != m_HlsServerIOMap.end(); )
    {
        pHlsServerIO=iter->second;
        if(0 == pHlsServerIO->GetProcFlag())
        {
            delete pHlsServerIO;
            iter=m_HlsServerIOMap.erase(iter);// ����Ԫ�ز�������һ��Ԫ�صĵ�����
        }
        else
        {
            iter++;// ����������һ��Ԫ��
        }
    }
    return 0;
}

/*****************************************************************************
-Fuction        : AddMapHttpSession
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version        Author           Modification
* -----------------------------------------------
* 2023/09/21      V1.0.0         Yu Weifeng       Created
******************************************************************************/
int HlsServerDemo::AddMapServerIO(HlsServerIO * i_pHlsServerIO,int i_iClientSocketFd)
{
    int iRet = -1;

    if(NULL == i_pHlsServerIO)
    {
        HLS_LOGE("AddMapServerIO NULL!!!%p\r\n",i_pHlsServerIO);
        return -1;
    }
    std::lock_guard<std::mutex> lock(m_MapMtx);//std::lock_guard������������������ʱ�Զ��ͷŻ�����
    m_HlsServerIOMap.insert(make_pair(i_iClientSocketFd,i_pHlsServerIO));
    return 0;
}


