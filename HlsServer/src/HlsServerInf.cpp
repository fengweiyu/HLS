/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerInf.cpp
* Description           : 	    接口层，防止曝露内部文件
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsServerInf.h"
#include "HlsServer.h"

/*****************************************************************************
-Fuction        : WebRtcInterface
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HlsServerInf::HlsServerInf()
{
    m_pHandle = NULL;
    m_pHandle = new HlsServer();
}
/*****************************************************************************
-Fuction        : ~WebRtcInterface
-Description    : ~WebRtcInterface
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HlsServerInf::~HlsServerInf()
{
    if(NULL != m_pHandle)
    {
        HlsServer *pHlsServer = (HlsServer *)m_pHandle;
        delete pHlsServer;
    }  
}

/*****************************************************************************
-Fuction        : Proc
-Description    : Proc
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsServerInf::HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen)
{
    HlsServer *pHlsServer = (HlsServer *)m_pHandle;
    return pHlsServer->HandleHttpReq(i_strReq,o_strRes,i_iResMaxLen);
}

