/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       main.cpp
* Description           : 	    
* Created               :       2020.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>

#include "HlsClientDemo.h"

static void PrintUsage(char *i_strProcName);

/*****************************************************************************
-Fuction        : main
-Description    : main
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/01      V1.0.0              Yu Weifeng       Created
******************************************************************************/
int main(int argc, char* argv[]) 
{
    int iRet = -1;
    
    if(argc < 2)
    {
        PrintUsage(argv[0]);
        return iRet;
    }
    HlsClientDemo *pHlsClientDemo = new HlsClientDemo();
    if(argc == 2)
    {
        iRet=pHlsClientDemo->Proc(argv[1]);//����
        delete pHlsClientDemo;
        return iRet;
    }
    iRet=pHlsClientDemo->Proc(argv[1],argv[2]);//����
    delete pHlsClientDemo;
    return iRet;
}
/*****************************************************************************
-Fuction        : PrintUsage
-Description    : PrintUsage
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/01      V1.0.0              Yu Weifeng       Created
******************************************************************************/
static void PrintUsage(char *i_strProcName)
{
    printf("Usage: %s inputURL(this will Generate files in ./xx/xx/*.*)\r\n",i_strProcName);
    printf("eg: %s http://localhost:9212/live/sn/test.m3u8\r\n",i_strProcName);
    printf("or: %s inputURL outputFILE (this will merge files to one file)\r\n",i_strProcName);
    printf("eg: %s http://localhost:9212/live/sn/test.m3u8 test\r\n",i_strProcName);
}

