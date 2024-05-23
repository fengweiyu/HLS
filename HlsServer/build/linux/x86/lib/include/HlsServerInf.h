/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsServerInf.h
* Description           : 	
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_SERVER_INF_H
#define HLS_SERVER_INF_H




/*****************************************************************************
-Class          : HlsServerInf
-Description    : HlsServerInf
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/11      V1.0.0              Yu Weifeng       Created
******************************************************************************/
class HlsServerInf
{
public:
	HlsServerInf();
	virtual ~HlsServerInf();
    int HandleHttpReq(const char * i_strReq,char *o_strRes,int i_iResMaxLen);//return ResLen,<0 err
private:
    void * m_pHandle;
};










#endif
