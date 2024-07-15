/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :    HlsClientCom.h
* Description           :    模块内部与外部调用者共同的依赖，放到对外的include里
* Created               :    2020.01.13.
* Author                :    Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_CLIENT_COM_H
#define HLS_CLIENT_COM_H


#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#define SleepMs(val) Sleep(val)
#define MakeDir(val) _mkdir(val)
#define DeleteDir(val) _rmdir(val)
#else
#include <unistd.h>
#include <sys/stat.h> 
#define SleepMs(val) usleep(val*1000)
#define MakeDir(val) mkdir(val, 0777)
#define DeleteDir(val) rmdir(val)
#endif




#define  HLS_LOGW2(val,fmt,...)      printf("<%d>:"fmt,val,##__VA_ARGS__)
#define  HLS_LOGE2(val,fmt,...)      printf("<%d>:"fmt,val,##__VA_ARGS__)
#define  HLS_LOGD2(val,fmt,...)      printf("<%d>:"fmt,val,##__VA_ARGS__)
#define  HLS_LOGW(...)     printf(__VA_ARGS__)
#define  HLS_LOGE(...)     printf(__VA_ARGS__)
#define  HLS_LOGD(...)     printf(__VA_ARGS__)
#define  HLS_LOGI(...)     printf(__VA_ARGS__)

















#endif
