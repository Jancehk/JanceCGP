/*
 * =====================================================================================
 *
 *       Filename:  cpss_common.c
 *
 *    Description:  通用函数实现
 *
 *        Version:  1.0
 *        Created:  2011年06月04日 21时41分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#include "cpss_public.h"
#include "cpss_tm_timer.h"
#define CPSS_CGP_LARGE  	1
#define CPSS_CGP_MIDDLE		0
#define CPSS_CGP_SMALL		0

static VOS_CHAR g_CGPVersion[32] = {0};
/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_getline
 *  Description:   得到一行
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_getver()
{
	sprintf(g_CGPVersion,"%d.%d.%d",CPSS_CGP_LARGE, CPSS_CGP_MIDDLE, CPSS_CGP_SMALL);
	return g_CGPVersion;
}
/*===  FUNCTION  ==============================================================
 *         Name:  get_cpuid_from_str
 *  Description:	16进制倒序过来
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
static VOS_UINT32 get_back_info(VOS_UINT32 ulInfo)
{
	VOS_UINT32 ulTmp1 = 0;
	VOS_UINT32 ulTmp2 = 0;
	if (0 == ulInfo)
	{
		return ulInfo;
	}
	ulTmp1 = ulInfo >>8;
	ulInfo = ulInfo <<16;
	ulTmp2 = (ulTmp1 & 0x000000FF) <<8;
	ulInfo = (ulInfo + ulTmp2)<< 8;
	ulTmp2 = ((ulTmp1 >>8)&0x000000FF)<<8;
	ulInfo = ulInfo + ulTmp2 + (ulTmp1 >> 16);
	return ulInfo;
}
/*===  FUNCTION  ==============================================================
 *         Name:  get_cpuid_from_str
 *  Description:	字符串ip转换为cpuid
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  check_is_ipaddress
 *  Description:  
 * =====================================================================================
 */
static VOS_BOOL check_is_ipaddress(VOS_CHAR* pszIp)
{
	VOS_CHAR * szIptmp = pszIp;
	VOS_INT32 nDotnumber = 0;
	VOS_INT32 nNumber =0;
	while(0 != *szIptmp)
	{
		if(0 != memcmp(".", szIptmp,1))
		{
			if (VOS_OK == isdigit(*szIptmp))
			{
				return VOS_ERR;
			}
		}
		else
		{
			nDotnumber ++;
			nNumber =-1;
		}
		nNumber++;
		szIptmp++;
		if (3 < nNumber)
		{
			return VOS_ERR;
		}
	}
	if(3 != nDotnumber)
	{
		return VOS_ERR;
	}
	return VOS_OK;
}		/* -----  end of function check_is_ipaddress  ----- */

/*===  FUNCTION  ==============================================================
 *         Name:  get_cpuid_from_str
 *  Description:	字符串ip转换为cpuid
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 get_cpuid_from_str(VOS_CHAR* pszIp)
{
	VOS_UINT32 result = 0,nIptmp = 0;
	VOS_CHAR * tmpIp = NULL,* pszIpTmp = NULL;
	VOS_UINT32 nIp1 =0,nIp2 =0,nIp3 =0,nIp4 =0;
	VOS_CHAR   InPutIp[32];	
	if (VOS_OK != check_is_ipaddress(pszIp))
	{
		return 0;
	}
	pszIpTmp = InPutIp;
	BZERO(InPutIp,strlen(pszIp));
	strcpy(pszIpTmp,pszIp);
	//scanf();
	sscanf(pszIpTmp, "%d.%d.%d.%d", &nIp1, &nIp2, &nIp3, &nIp4);
	result = nIp1<<24; 
	result += nIp2<<16;// 
	result += nIp3<<8;
	result += nIp4;
	if(0 != result)
	{
		return result;
	}
	else
	{
		return 0;
	}
	//	memcpy(InPutIp, pszIp,strlen(pszIp));
	while(0 != *pszIpTmp)
	{
		tmpIp = strstr(pszIpTmp,".");
		if(NULL == tmpIp)
		{
			nIptmp = atoi(pszIpTmp);
			result = (result <<8)+nIptmp;
			if(0 != result)
			{
				return result;
			}
			else
			{
				return 0;
			}
		}
		*tmpIp = 0;
		nIptmp = atoi(pszIpTmp);
		if (0 == nIptmp )
		{
			return 0;
		}
		result = (result <<8)+nIptmp;
		pszIpTmp = tmpIp +1;
	}
	return 0;
}


/*===  FUNCTION  ==============================================================
 *         Name:  get_cpuid_from_ip
 *  Description:	32位ip转换为cpuid
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 get_cpuid_from_ip(VOS_UINT32 ulIp)
{
	return get_back_info(ulIp);
}
/*===  FUNCTION  ==============================================================
 *         Name:  get_ip_from_cpuid
 *  Description:	32位cpuid转换为ip
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 get_ip_from_cpuid(VOS_UINT32 ulCpuid)
{
	return get_back_info(ulCpuid);
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_get_str_before
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_get_str_before (VOS_STRING pszInputStr, VOS_STRING szFind)
{
	VOS_STRING szStrTmp = NULL;
	if(NULL == pszInputStr || NULL == szFind)
	{
		return szStrTmp;
	}
	//VOS_STRING szStrTmp = strstr(pszInputStr, szFind);
	szStrTmp = strtok(pszInputStr,szFind);
	while(0 != *szStrTmp)
	{
		szStrTmp ++;
	}
	szStrTmp++;
	return szStrTmp;
}		/* -----  end of function cpss_get_str_before  ----- */


/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Mutex_Init
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :   VOS_MUTEX * pMutex     
				   VOS_CHAR  * pstr
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Mutex_Init(VOS_MUTEX * pMutex, VOS_CHAR * pstr)
{
	VOS_UINT32 nRet = VOS_ERR;
	
	VOS_CHAR   szTime[CPSS_MAX_TIME] = {0};
	VOS_UINT32 nTimes = 0;
	//strcpy(pEvent->strmutex,pStrEnevt);
	
	cpss_get_current_time_to_str(szTime);		//得到当前系统时间	
	_snprintf(pMutex->strmutex, VOS_MUTEX_LEN, "%s:%p:%s:%08x",pstr,pMutex,szTime,nTimes++);
#if (OS_TYPE == OS_TYPE_WINDOW)
#if VOS_LOCK_TYPE == VOS_LOCK_CRITE
	InitializeCriticalSection(&pMutex->mcs);
#elif VOS_LOCK_TYPE == VOS_LOCK_MUTEX
	
	pMutex->hMutex = CreateMutex(NULL, TRUE, pMutex->strmutex);
	if (NULL == pMutex->hMutex)
	{
		return nRet;
	}
	/*pMutex->mutexRet = WaitForSingleObject(pMutex->hMutex,INFINITE);
	if (WAIT_FAILED == pMutex->mutexRet)
	{
		return nRet;
	}
	else if (WAIT_TIMEOUT == pMutex->mutexRet)
	{
		return nRet;
	}
	ReleaseMutex(pMutex->hMutex);*/
	ReleaseMutex(pMutex->hMutex);
#elif VOS_LOCK_TYPE == VOS_LOCK_EVENT
	pMutex->hEvent = CreateEvent(NULL,FALSE, TRUE, pMutex->strmutex);
#endif
#elif (OS_TYPE == OS_TYPE_LINUX)
	pthread_mutex_init(&pMutex->hMutex);
#endif
	nRet = VOS_OK;
	return nRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Mutex_Lock
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :   VOS_MUTEX * pMutex  
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Mutex_Lock(VOS_MUTEX * pMutex)
{
	VOS_UINT32 nRet = VOS_ERR;
#if (OS_TYPE == OS_TYPE_WINDOW)
#if VOS_LOCK_TYPE == VOS_LOCK_CRITE
	EnterCriticalSection(&pMutex->mcs);
#elif VOS_LOCK_TYPE == VOS_LOCK_MUTEX
	pMutex->hMutex = CreateMutex(NULL, TRUE, pMutex->strmutex);
	if (NULL == pMutex->hMutex)
	{
		return nRet;
	}
	pMutex->mutexRet = WaitForSingleObject(pMutex->hMutex,INFINITE);
	if (WAIT_FAILED == pMutex->mutexRet)
	{
		return nRet;
	}
	else if (WAIT_TIMEOUT == pMutex->mutexRet)
	{
		return nRet;
	}
#elif VOS_LOCK_TYPE == VOS_LOCK_EVENT	
	
	pMutex->mutexRet = WaitForSingleObject(pMutex->hEvent, INFINITE);
	if (WAIT_FAILED == pMutex->mutexRet)
	{
		return -1;
	}
	else if (WAIT_TIMEOUT == pMutex->mutexRet)
	{
		return -1;
	}
//	ResetEvent(pstrm->hEvent);
#endif
#elif (OS_TYPE == OS_TYPE_LINUX)
	pthread_mutex_lock(&pMutex->hMutex);
#endif
	nRet = VOS_OK;
	return nRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Mutex_Unlock
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Mutex_Unlock(VOS_MUTEX * pMutex)
{
	VOS_UINT32 nRet = VOS_ERR;
#if (OS_TYPE == OS_TYPE_WINDOW)
#if VOS_LOCK_TYPE == VOS_LOCK_CRITE
	LeaveCriticalSection(&pMutex->mcs);
#elif VOS_LOCK_TYPE == VOS_LOCK_MUTEX
	ReleaseMutex(pMutex->hMutex);
	if (0 != pMutex->mutexRet)
	{
		CloseHandle(pMutex->mutexRet);
	}
#elif VOS_LOCK_TYPE == VOS_LOCK_EVENT
	SetEvent(pMutex->hEvent);
#endif
#elif (OS_TYPE == OS_TYPE_LINUX)
	pthread_mutex_unlock(&pMutex->hMutex);
#endif
	nRet = VOS_OK;
	return nRet;
}

/* ===  FUNCTION  ==============================================================
*         Name:  VOS_Mutex_Destroy
 *  Description:	得到字符串之' '之前的内容 
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Mutex_Destroy(VOS_MUTEX * pMutex)
{
	VOS_UINT32 nRet = VOS_ERR;
#if (OS_TYPE == OS_TYPE_WINDOW)
#if VOS_LOCK_TYPE == VOS_LOCK_CRITE
	DeleteCriticalSection(&pMutex->mcs);
#elif VOS_LOCK_TYPE == VOS_LOCK_MUTEX
	ReleaseMutex(pMutex->hMutex);
	if (0 != pMutex->mutexRet)
	{
		CloseHandle(pMutex->mutexRet);
	}
#elif VOS_LOCK_TYPE == VOS_LOCK_EVENT
	CloseHandle(pMutex->hEvent);
#endif
#elif (OS_TYPE == OS_TYPE_LINUX)
	pthread_mutex_destroy(&pMutex->hMutex);
#endif
	nRet = VOS_OK;
	return nRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Init_Event
 *  Description:	初始化Event
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Init_Event(VOS_Event * pEvent,VOS_STRING pStrEnevt)
{
	VOS_UINT32 nRet = VOS_ERR;
	VOS_CHAR   szTime[CPSS_MAX_TIME] = {0};
	static VOS_UINT32 nEventTime = 0;
#if (OS_TYPE == OS_TYPE_WINDOW)
	//strcpy(pEvent->strmutex,pStrEnevt);

	cpss_get_current_time_to_str(szTime);		//得到当前系统时间	
	_snprintf(pEvent->strmutex, VOS_MUTEX_LEN, "%s:%p:%s:%08x", pStrEnevt, pEvent, szTime, nEventTime++);
	pEvent->hEvent = CreateEvent(NULL,FALSE, FALSE, pEvent->strmutex);
	if (NULL != pEvent->hEvent)
	{
		nRet = VOS_OK;
	}
#elif (OS_TYPE == OS_TYPE_LINUX)
	if (sem_init(&pEvent->sem,0,0))
	{
		nRet = VOS_OK;
	}
#endif
	return nRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Set_Event
 *  Description:	设置 Event
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Set_Event(VOS_Event * pEvent)
{	
	VOS_UINT32 nRet = VOS_ERR;
#if (OS_TYPE == OS_TYPE_WINDOW)
	if (SetEvent(pEvent->hEvent))
	{
		nRet = VOS_OK;
	}
#elif (OS_TYPE == OS_TYPE_LINUX)
	if (sem_post(&pEvent->sem))
	{
		nRet = VOS_OK;
	} 
#endif
	return nRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Wait_Event
 *  Description:	等待Event
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Wait_Event(VOS_Event * pEvent, VOS_UINT32 uTime)
{
	VOS_UINT32 nRet = VOS_ERR; 
#if (OS_TYPE == OS_TYPE_LINUX)
	struct timeval tt;
	struct timespec ts;
#endif

#if (OS_TYPE == OS_TYPE_WINDOW)
	VOS_UINT32 pWRet = VOS_ERR;
	pWRet = WaitForSingleObject(pEvent->hEvent, uTime);//INFINITE
	if (WAIT_FAILED  != pWRet)// &&	
	{
		nRet = VOS_OK;
	}
#elif (OS_TYPE == OS_TYPE_LINUX)
	gettimeofday(&tt,NULL);  
	ts.tv_sec = tt.tv_sec;  
	ts.tv_nsec = tt.tv_usec*1000 + uTime * 1000 * 1000;
	ts.tv_sec += ts.tv_nsec/(1000 * 1000 *1000);
	ts.tv_nsec %= (1000 * 1000 *1000);
	if(sem_timedwait(&pEvent->sem,&ts))//if (sem_wait(&pEvent->sem))
	{
		nRet = VOS_OK;
	} 
#endif
	return nRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  VOS_Destory_Event
 *  Description:	销毁Event
 *  Input      :   VOS_MUTEX * pMutex    
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 VOS_Destory_Event(VOS_Event * pEvent, VOS_UINT32 uTime)
{
	VOS_UINT32 nRet = VOS_ERR;
#if (OS_TYPE == OS_TYPE_WINDOW)
	if (NULL != pEvent->hEvent)
	{
		ResetEvent(pEvent->hEvent);
		CloseHandle(pEvent->hEvent);
		nRet = VOS_OK;
	}
#elif (OS_TYPE == OS_TYPE_LINUX)
	if (sem_destroy(&pEvent->sem))
	{
		nRet = VOS_OK;
	} 
#endif
	return nRet;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim
 *  Description:   左右截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_CHAR *cpss_trim(VOS_STRING str, VOS_STRING sep)
{
	return cpss_trim_left(cpss_trim_right(str, sep), sep);
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim_left
 *  Description:   右截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_trim_left(VOS_STRING str, VOS_STRING sep)
{
	VOS_STRING pstr = str;
	VOS_CHAR	*p = NULL;
	
	if (NULL == pstr)
	{
		return NULL;
	}
	
	if ((NULL == sep) || (0 == *sep))
	{
		sep = TRIME_KEY;
	}
	
	for (; *pstr != 0; pstr++)
	{
		p = sep;
		while ((*p != 0) && (*p != *pstr)) p++;
		if (0 == *p)
		{
			break;
		}
	}
	//if (pstr > str) strcpy(str, pstr);
	if (pstr > str)
	{
		memmove(str, pstr, (_tcslen(pstr)+1)*sizeof(VOS_CHAR));
	}
	return str;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_trim_right
 *  Description:   左截取
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_STRING cpss_trim_right(VOS_STRING str, VOS_STRING sep)
{
	VOS_STRING pstr = str + VOS_Strlen(str) - 1;
	VOS_STRING p = NULL;
	
	if (NULL == pstr)
	{
		return NULL;
	}
	
	if ((NULL == sep) || (0 == *sep))
	{
		sep = TRIME_KEY;
	}
	
	for (; pstr >= str; pstr--)
	{
		p = sep;
		while ((*p != 0) && (*p != *pstr)) p++;
		if (0 == *p)
		{
			break;
		}
	}
	pstr[1] = 0;
	return str;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_split
 *  Description:   分割
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 cpss_split(VOS_STRING str, VOS_STRING sep, VOS_INT32 flag)
{
	VOS_STRING p = str;
	VOS_STRING ps = str;
	// the count of seperate string
	int		count = 0;

	if ((NULL == sep) || (0 == sep[0]))
		sep = TRIME_KEY;

	while ((*p != 0) && (VOS_Strstr(sep, *p) != NULL)) p++;
	if(0 == flag)
	{
		/*
		* flag == 0:  remove empty string
		* arithmetic:
		* 1. loop for each char of src-->p
		*    2. copy until src char is not in sep chars
		*	  3. break when src end
		*    4. set terminal char(0) for split string
		*    5. count++;
		*    6. skip until src char is not in sep chars
		* 7. end for loop
		*/
		while (*p != 0)
		{
			while ((0 != *p) && (NULL == VOS_Strstr(sep, *p)))
			{
				*ps++ = *p++;
			}
			if (0 != *p)
			{
				p++;
			}
			*ps++ = 0;
			count++;
			while ((0 != *p) && (NULL != VOS_Strstr(sep, *p)))
			{
				p++;
			}
		}
	}
	else
	{
		/*
		* flag == 1:  reverse the empty string
		* arithmetic:
		* 1. loop for each char of src-->p
		*    2. copy until src char is not in sep chars
		*    3. break when src end
		*    4. set terminal char(0) for split string
		*    5. count++;
		*    6. skip until src char is not in sep chars,
		and reverse the empty string, count++
		2. end the loop
		*/
		while (*p != 0)
		{
			while ((0 != *p) && (NULL == VOS_Strstr(sep, *p)))
				*ps++ = *p++;
			if (0 != *p)
			{
				p++;
				*ps++ = 0;
				count++;
			}
			while ((0 != *p) && (NULL != VOS_Strstr(sep, *p)))
			{
				p++;
				*ps++ = 0;
				count++;
			}
			if (0 == *p)
				count++;
		}
	}
	*ps = 0;

	return count;
}

/* ===  FUNCTION  ==============================================================
 *         Name:  cpss_getline
 *  Description:   得到一行
 *  Input      :   
 *  OutPut     :    
 *  Return     :  
 * ==========================================================================*/
#define COMMENT_CHAR    '#'
VOS_STRING cpss_getline(VOS_STRING pbuf, VOS_STRING pline, VOS_INT32 skip)
{
	TCHAR        *p = pbuf;

	pline[0] = 0;
	if (p != NULL)
	{
		if (VOS_TRUE == skip)
		{
		   /*
			* get one line and skip it when line is empty string
			*/
			while (0 == *p)
			{
				p = cpss_getline(p, pline, VOS_FALSE);
				cpss_trim(pline, TRIME_KEY);
				if (0 == *pline)
				{
					continue;
				}
				if (COMMENT_CHAR == *pline)
				{
					continue;
				}
				return p;
			}
			*pline = 0;
		}
		else
		{
			/*
			* get one line
			*/
			while (('\n' != *p) && ('\r' != *p) && ('\0' != *p))
				*pline++ = *p++;
			if ('\n' == *p || '\r' == *p)
				*pline++ = *p++;
			if ('\n' == *p || '\r' == *p)
				*pline++ = *p++;
			*pline = 0;
		}
	}

	return p;
}
