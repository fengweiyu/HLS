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
    m_M3U8FileList.clear();
    m_M3U8FileHandledList.clear();
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
    m_M3U8FileList.clear();
    m_M3U8FileHandledList.clear();
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
    const char * strVodFlag = "#EXT-X-ENDLIST";

    
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
    if(string::npos != strM3U8.find(strVodFlag))//
    {
        *o_iInfTime=0;//点播流不需要睡眠时间，直接不断去下载
    }
    else
    {
        *o_iInfTime=atoi(strM3U8.substr(iInfTimePos+strlen(strExtInf),iInfTimeEndPos-(iInfTimePos+strlen(strExtInf))).c_str());
    }
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
-Fuction        : HandleM3U8
-Description    : 
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsClientSession::ParseM3U8(const char * i_strM3U8)
{
    int iRet = -1;
    string strM3U8("");
    string strFind("");
    int iPos=0;
    const char * strExtUri="#EXT-X-MAP:URI=\"";
    const char * strExtInf="#EXTINF:";
    const char * strVodFlag = "#EXT-X-ENDLIST";
    const char * strExtMaxDuration="#EXT-X-TARGETDURATION:";
    M3U8File oM3U8File;
    unsigned int dwInfTime;//s
    
    if(NULL == i_strM3U8)
    {
        HLS_LOGE("HandleM3U8 NULL \r\n");
        return iRet;
    }
    strM3U8.assign(i_strM3U8);
    do
    {
        auto iUriPos=strM3U8.find(strExtUri,iPos);
        if(string::npos != iUriPos)//
        {
            auto iUriEndPos=strM3U8.find("\",",iUriPos);
            if(string::npos == iUriEndPos)//
            {
                HLS_LOGE("strHttpURL.find URI err \r\n");
                return iRet;
            }
            oM3U8File.strName.assign(strM3U8.substr(iUriPos+strlen(strExtUri),iUriEndPos-(iUriPos+strlen(strExtUri))).c_str());
            oM3U8File.dwDuration=0;
            HLS_LOGD("%s, %s,o_iInfTime%d\r\n",strExtUri,oM3U8File.strName.c_str(),oM3U8File.dwDuration);
            m_M3U8FileList.push_back(oM3U8File);
            iRet=0;
            iPos=(int)iUriEndPos;
        }
        auto iInfTimePos=strM3U8.find(strExtInf,iPos);
        if(string::npos == iInfTimePos)//
        {
            HLS_LOGE("strHttpURL.find %s NULL \r\n",strExtInf);
            return 0;
        }
        auto iInfTimeEndPos=strM3U8.find(".",iInfTimePos);
        if(string::npos == iInfTimeEndPos)//
        {
            HLS_LOGE("strHttpURL.find %s iInfTimeEndPos err \r\n",strExtInf);
            return -1;
        }
        if(string::npos != strM3U8.find(strVodFlag))//
        {
            dwInfTime=0;//点播流不需要睡眠时间，直接不断去下载
        }
        else
        {
            dwInfTime=atoi(strM3U8.substr(iInfTimePos+strlen(strExtInf),iInfTimeEndPos-(iInfTimePos+strlen(strExtInf))).c_str());
        }
        auto iFileNamePos=strM3U8.find("\n",iInfTimeEndPos);
        if(string::npos == iFileNamePos)//
        {
            HLS_LOGE("strHttpURL.find iFileNamePos err \r\n");
            return -1;
        }
        auto iFileNameEndPos=strM3U8.find("\r\n",iFileNamePos+strlen("\n"));
        if(string::npos == iFileNameEndPos)//
        {
            iFileNameEndPos=strM3U8.find("\n",iFileNamePos+strlen("\n"));
            if(string::npos == iFileNameEndPos)//
            {
                HLS_LOGE("strHttpURL.find iFileNameEndPos err \r\n");
                return -1;
            }
        }
        iPos=(int)iFileNameEndPos;
        oM3U8File.strName.assign(strM3U8.substr(iFileNamePos+strlen("\n"),iFileNameEndPos-(iFileNamePos+strlen("\n"))).c_str());
        oM3U8File.dwDuration=dwInfTime;
        if(0 != FindSameFileName(&oM3U8File))
            m_M3U8FileList.push_back(oM3U8File);
        iRet=1;
        HLS_LOGD("o_strFileName %s,o_iInfTime%d \r\n",oM3U8File.strName.c_str(),oM3U8File.dwDuration);
    }while(iRet>0);
    return iRet;
}

/*****************************************************************************
-Fuction        : GetFileInfo
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsClientSession::GetMaxDurationTime(const char * i_strM3U8,int * o_iMaxDurationTime)
{
    int iRet = -1;
    const char * strExtMaxDuration="#EXT-X-TARGETDURATION:";
    string strM3U8("");
    int iMaxDurationTime=0;

    if(NULL== i_strM3U8||NULL== o_iMaxDurationTime)
    {
        HLS_LOGE("GetMaxDurationTime NULL== i_strM3U8||NULL== o_iMaxDurationTime\r\n");
        return iRet;
    }
    strM3U8.assign(i_strM3U8);
    auto iMaxDurationPos=strM3U8.find(strExtMaxDuration);
    if(string::npos == iMaxDurationPos)//
    {
        HLS_LOGE("strHttpURL.find %s iMaxDurationPos err \r\n",strExtMaxDuration);
        return -1;
    }
    auto iMaxDurationEndPos=strM3U8.find("\r\n",iMaxDurationPos+strlen(strExtMaxDuration));
    if(string::npos == iMaxDurationEndPos)//
    {
        iMaxDurationEndPos=strM3U8.find("\n",iMaxDurationPos+strlen(strExtMaxDuration));
        if(string::npos == iMaxDurationEndPos)//
        {
            HLS_LOGE("strHttpURL.find %s err \r\n",strExtMaxDuration);
            return -1;
        }
    }
    iMaxDurationTime=atoi(strM3U8.substr(iMaxDurationPos+strlen(strExtMaxDuration),iMaxDurationEndPos-(iMaxDurationPos+strlen(strExtMaxDuration))).c_str());
    HLS_LOGD("%s, iMaxDurationTime%d\r\n",strExtMaxDuration,iMaxDurationTime);
    *o_iMaxDurationTime=iMaxDurationTime;
    return 0;
}

/*****************************************************************************
-Fuction        : GetFileInfo
-Description    : //return ResLen,<0 err
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/13      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int HlsClientSession::GetFileInfo(int * o_iDurationTime,string * o_strFileName,string * o_strFileURL)
{
    int iRet = -1;

    if(NULL== o_iDurationTime||NULL== o_strFileURL)
    {
        HLS_LOGE("GetFileInfo NULL== o_iDuration||NULL== o_strFileURL\r\n");
        return iRet;
    }
    if(NULL== m_pMediaLocation)
    {
        HLS_LOGE("GetMedia m_pMediaLocation NULL\r\n");
        return iRet;
    }
    if(m_M3U8FileList.size()<=0)
    {
        HLS_LOGE("GetFileInfo m_M3U8FileList NULL\r\n");
        return iRet;
    }
    o_strFileName->assign(m_M3U8FileList.front().strName.c_str());
    o_strFileURL->assign(m_pMediaLocation->c_str());
    o_strFileURL->append(o_strFileName->c_str());//m_M3U8FileList.front()返回引用
    *o_iDurationTime=(int)m_M3U8FileList.front().dwDuration;
    m_M3U8FileHandledList.push_back(m_M3U8FileList.front());
    m_M3U8FileList.pop_front();
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
int HlsClientSession::FindSameFileName(M3U8File *i_pM3U8File)
{
    int iRet = -1;

    
    if(NULL== i_pM3U8File)
    {
        HLS_LOGE("FindSameFileName err NULL\r\n");
        return iRet;
    }
    
    for (auto it = m_M3U8FileHandledList.begin(); it != m_M3U8FileHandledList.end(); ++it) 
    {  
        if(it->strName==i_pM3U8File->strName)  
        {
            return 0;
        }
    } 
    return iRet;
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

