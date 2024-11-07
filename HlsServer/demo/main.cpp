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

#include "HlsServerDemo.h"

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
    
    int dwServerPort=9210;
    
    if(argc !=2)
    {
        PrintUsage(argv[0]);
    }
    else
    {
        dwServerPort=atoi(argv[1]);
    }
    HlsServerDemo *pHlsServerDemo = new HlsServerDemo(dwServerPort);
    iRet=pHlsServerDemo->Proc();//×èÈû
    
    return iRet;
}
/*****************************************************************************
-Fuction        : PrintUsage
-Description    : http://localhost:9210/file/H264AAC.flv/testFMP4.m3u8
https://localhost:9211/file/H264AAC.flv/testFMP4.m3u8
-Input          : 
-Output         : 
-Return         : 
* Modify Date     Version             Author           Modification
* -----------------------------------------------
* 2020/01/01      V1.0.0              Yu Weifeng       Created
******************************************************************************/
static void PrintUsage(char *i_strProcName)
{
    printf("Usage: %s ServerPort \r\n",i_strProcName);
    printf("run default args: %s 9210 \r\n",i_strProcName);
    printf("hls url eg: http://localhost:9210/file/H264AAC.flv/testTS.m3u8\r\n");
    printf("hls url eg: http://localhost:9210/file/H265AAC.flv/testTS.m3u8\r\n");
    printf("hls url eg: http://localhost:9210/file/H264AAC.flv/testFMP4.m3u8\r\n");
    printf("hls url eg: http://localhost:9210/file/H265AAC.flv/testFMP4.m3u8\r\n");
    printf("hls url eg: https://yuweifeng.top:9211/file/H264AAC.flv/testTS.m3u8\r\n");
    printf("hls url eg: https://yuweifeng.top:9211/file/H265AAC.flv/testTS.m3u8\r\n");
    printf("hls url eg: https://yuweifeng.top:9211/file/H264AAC.flv/testFMP4.m3u8\r\n");
    printf("hls url eg: https://yuweifeng.top:9211/file/H265AAC.flv/testFMP4.m3u8\r\n");
}

