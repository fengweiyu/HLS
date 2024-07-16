/*****************************************************************************
* Copyright (C) 2020-2025 Hanson Yu  All rights reserved.
------------------------------------------------------------------------------
* File Module           :       HlsClientDemo.c
* Description           : 	
* Created               :       2023.01.13.
* Author                :       Yu Weifeng
* Function List         : 	
* Last Modified         : 	
* History               : 	
******************************************************************************/
#include "HlsClientDemo.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <utility>

using std::make_pair;

/*****************************************************************************
-Fuction		: HlsClientDemo
-Description	: HlsClientDemo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HlsClientDemo :: HlsClientDemo()
{
}

/*****************************************************************************
-Fuction		: ~HlsClientDemo
-Description	: ~HlsClientDemo
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
HlsClientDemo :: ~HlsClientDemo()
{
}

/*****************************************************************************
-Fuction		: Proc
-Description	: ×èÈû
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2023/10/10	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int HlsClientDemo :: Proc(const char * i_strHttpURL,const char *i_strFlag)
{
    return m_HlsClientIO.Proc(i_strHttpURL,i_strFlag);
}


