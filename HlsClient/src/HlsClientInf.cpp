/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClientInf.cpp
* Description           : 	    接口层，防止曝露内部文件
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsClientInf.h"
#include "HlsClient.h"

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
HlsClientInf::HlsClientInf()
{
    m_pHandle = NULL;
    m_pHandle = new HlsClient();
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
HlsClientInf::~HlsClientInf()
{
    if(NULL != m_pHandle)
    {
        HlsClient *pHlsClient = (HlsClient *)m_pHandle;
        delete pHlsClient;
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
int HlsClientInf::ParseHttpURL(const char * i_strHttpURL,string *o_strIP,int *o_iPort,string * o_strURL)
{
    HlsClient *pHlsClient = (HlsClient *)m_pHandle;
    return pHlsClient->ParseHttpURL(i_strHttpURL,o_strIP,o_iPort,o_strURL);
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
int HlsClientInf::GenerateGetM3U8Req(const char * i_strM3U8URL,char *o_strReq,int i_iReqMaxLen)
{
    HlsClient *pHlsClient = (HlsClient *)m_pHandle;
    return pHlsClient->GenerateGetM3U8Req(i_strM3U8URL,o_strReq,i_iReqMaxLen);
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
int HlsClientInf::HandleHttpM3U8(char * i_strHttpM3U8,int *m_iOffset,int * o_iTryTime,string *o_strFileName,char *o_strReq,int i_iReqMaxLen)
{
    HlsClient *pHlsClient = (HlsClient *)m_pHandle;
    return pHlsClient->HandleHttpM3U8(i_strHttpM3U8,m_iOffset,o_iTryTime,o_strFileName,o_strReq,i_iReqMaxLen);
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
int HlsClientInf::ParseHttpM3U8(char * i_strHttpM3U8,int * o_iMaxDurationTime)
{
    HlsClient *pHlsClient = (HlsClient *)m_pHandle;
    return pHlsClient->ParseHttpM3U8(i_strHttpM3U8,o_iMaxDurationTime);
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
int HlsClientInf::GetFileInfo(int * o_iDurationTime,string *o_strFileName,char *o_strReq,int i_iReqMaxLen)
{
    HlsClient *pHlsClient = (HlsClient *)m_pHandle;
    return pHlsClient->GetFileInfo(o_iDurationTime,o_strFileName,o_strReq,i_iReqMaxLen);
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
int HlsClientInf::HandleHttpMedia(char * i_strHttpMedia,int i_iMediaLen,char **o_ppMediaData,int *o_iDataLen)
{
    HlsClient *pHlsClient = (HlsClient *)m_pHandle;
    return pHlsClient->HandleHttpMedia(i_strHttpMedia,i_iMediaLen,o_ppMediaData,o_iDataLen);
}

