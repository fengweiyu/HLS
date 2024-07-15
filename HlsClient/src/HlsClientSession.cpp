/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClientSession.cpp
* Description           : 	
* Created               :       2022.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsClientSession.h"
#include "HlsClientCom.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <functional>


/*****************************************************************************
-Fuction        : HlsClientSession
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HlsClientSession::HlsClientSession()
{
    m_pMediaLocation=NULL;
}

/*****************************************************************************
-Fuction        : ~HlsClientSession
-Description    : ~HlsClientSession
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
HlsClientSession::~HlsClientSession()
{
    if(NULL!= m_pMediaLocation)
    {
        HLS_LOGW("~HlsClientSession start exit\r\n");
        delete m_pMediaLocation;
        m_pMediaLocation = NULL;
    }
}
/*****************************************************************************
-Fuction        : HandleM3U8
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsClientSession::HandleM3U8(const char * i_strM3U8,int *m_iOffset,int * o_iInfTime,string *o_strFileName)
{
    int iRet = -1;
    string strM3U8("");
    string strFind("");
    int iPos=0;
    const char * strExtUri="#EXT-X-MAP:URI=\"";
    const char * strExtInf="#EXTINF:";

    
    if(NULL == i_strM3U8 || NULL == m_iOffset || NULL == o_iInfTime || NULL == o_strFileName)
    {
        HLS_LOGE("HandleM3U8 NULL \r\n");
        return iRet;
    }
    strM3U8.assign(i_strM3U8);
    iPos=*m_iOffset;
    auto iUriPos=strM3U8.find(strExtUri,iPos);
    if(string::npos != iUriPos)//
    {
        auto iUriEndPos=strM3U8.find("\",",iUriPos);
        if(string::npos == iUriEndPos)//
        {
            HLS_LOGE("strHttpURL.find URI err \r\n");
            return iRet;
        }
        o_strFileName->assign(strM3U8.substr(iUriPos+strlen(strExtUri),iUriEndPos-(iUriPos+strlen(strExtUri))).c_str());
        *o_iInfTime=0;
        *m_iOffset=(int)iUriEndPos;
        HLS_LOGD("strExtUri %s,o_iInfTime%d,m_iOffset%d \r\n",o_strFileName->c_str(),*o_iInfTime,*m_iOffset);
        iRet=0;
        return iRet;
    }
    auto iInfTimePos=strM3U8.find(strExtInf,iPos);
    if(string::npos == iInfTimePos)//
    {
        HLS_LOGE("strHttpURL.find iInfTime err \r\n");
        return iRet;
    }
    auto iInfTimeEndPos=strM3U8.find(".",iInfTimePos);
    if(string::npos == iInfTimeEndPos)//
    {
        HLS_LOGE("strHttpURL.find iInfTimeEndPos err \r\n");
        return iRet;
    }
    *o_iInfTime=atoi(strM3U8.substr(iInfTimePos+strlen(strExtInf),iInfTimeEndPos-(iInfTimePos+strlen(strExtInf))).c_str());
    auto iFileNamePos=strM3U8.find("\n",iInfTimeEndPos);
    if(string::npos == iFileNamePos)//
    {
        HLS_LOGE("strHttpURL.find iFileNamePos err \r\n");
        return iRet;
    }
    auto iFileNameEndPos=strM3U8.find("\r\n",iFileNamePos+strlen("\n"));
    if(string::npos == iFileNameEndPos)//
    {
        iFileNameEndPos=strM3U8.find("\n",iFileNamePos+strlen("\n"));
        if(string::npos == iFileNameEndPos)//
        {
            HLS_LOGE("strHttpURL.find iFileNameEndPos err \r\n");
            return iRet;
        }
    }
    o_strFileName->assign(strM3U8.substr(iFileNamePos+strlen("\n"),iFileNameEndPos-(iFileNamePos+strlen("\n"))).c_str());
    *m_iOffset=(int)iFileNameEndPos;
    iRet=0;
    HLS_LOGD("o_strFileName %s,o_iInfTime%d,m_iOffset%d \r\n",o_strFileName->c_str(),*o_iInfTime,*m_iOffset);
    return iRet;
}

/*****************************************************************************
-Fuction        : Proc
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsClientSession::GetFileURL(const char * i_strFileName,string * o_strFileURL)
{
    int iRet = -1;

    
    if(NULL== i_strFileName||NULL== o_strFileURL)
    {
        HLS_LOGE("GetMedia NULL== i_strFileName||NULL== o_strFileURL\r\n");
        return iRet;
    }
    if(NULL== m_pMediaLocation)
    {
        HLS_LOGE("GetMedia m_pMediaLocation NULL\r\n");
        return iRet;
    }
    o_strFileURL->assign(m_pMediaLocation->c_str());
    o_strFileURL->append(i_strFileName);
    return 0;
}

/*****************************************************************************
-Fuction        : SetMediaLocation
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsClientSession::SetMediaLocation(const char *i_strLocate)
{
    int iRet = -1;
    if(NULL== i_strLocate)
    {
        HLS_LOGE("SetMediaLocation err NULL\r\n");
        return iRet;
    }
    HLS_LOGD("SetMediaLocation %s\r\n",i_strLocate);
    m_pMediaLocation = new string(i_strLocate);
    return 0;
}

