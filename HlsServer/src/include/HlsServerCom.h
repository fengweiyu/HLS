/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :    HlsServerCom.h
* Description           :    模块内部与外部调用者共同的依赖，放到对外的include里
* Created               :    2020.01.13.
* Author                :    Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#ifndef HLS_SERVER_COM_H
#define HLS_SERVER_COM_H


#ifdef _WIN32
#include <Windows.h>
#define SleepMs(val) Sleep(val)
#else
#include <sys/time.h>
#define SleepMs(val) usleep(val*1000)
#endif




#define  HLS_LOGW2(val,fmt,...)      printf("<%d>:"fmt,val,##__VA_ARGS__)
#define  HLS_LOGE2(val,fmt,...)      printf("<%d>:"fmt,val,##__VA_ARGS__)
#define  HLS_LOGD2(val,fmt,...)      printf("<%d>:"fmt,val,##__VA_ARGS__)
#define  HLS_LOGW(...)     printf(__VA_ARGS__)
#define  HLS_LOGE(...)     printf(__VA_ARGS__)
#define  HLS_LOGD(...)     printf(__VA_ARGS__)
#define  HLS_LOGI(...)     printf(__VA_ARGS__)

















#endif
