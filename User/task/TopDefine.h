#pragma once


#ifdef TOP_USER_CFG
#include TOP_USER_CFG
#endif

//===================用户配置===================
//是否使用freertos
#ifndef _FREERTOS
#define _FREERTOS 1
#endif
//是否开启调试
#ifndef _DEBUG
#define _DEBUG 1
#endif

