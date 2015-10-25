#ifndef _DBSVR_COMMON_IMPORT_H
#define _DBSVR_COMMON_IMPORT_H

#ifndef Jance_API_EXP
#define Jance_API_EXP __declspec(dllexport)
#endif
#ifndef Jance_API_IMP 
#define Jance_API_IMP __declspec(dllimport)
#endif

//extern "C"Jance_API_IMP int CreateAccMDB(char * m_strPath);
HINSTANCE g_hDLL = NULL;
VOS_UINT32 g_adoConnect = 0;
VOS_CHAR g_strCommand[2048]={0};
typedef VOS_UINT32 (*CreateAccMDBT)(VOS_CHAR * m_strPath);
typedef VOS_UINT32 (*OpenAccMDBT)(VOS_CHAR * m_strPath, VOS_UINT32 *adoConnect);
typedef VOS_UINT32 (*CloseAccMDBT)(VOS_UINT32 adoConnect);
typedef VOS_UINT32 (*ExecCommandT)(VOS_UINT32 adoConnect,VOS_CHAR * pstrCmd);
typedef VOS_UINT32 (*ExecRecdT)(VOS_UINT32 adoConnect,VOS_UINT32 * adoRecord, VOS_CHAR * pstrCmd);
typedef VOS_UINT32 (*FreeRecdT)(VOS_UINT32 adoRecord);
typedef VOS_UINT32 (*GetRecordCountT)(VOS_UINT32 adoRecord);
OpenAccMDBT  pOpenAccMDB = NULL;
CloseAccMDBT pCloseAccMDB = NULL;
ExecCommandT  pExecCommand = NULL;
ExecRecdT pExecRecd = NULL;
FreeRecdT  pFreeRecd = NULL;
GetRecordCountT pGetRecordCount = NULL;
#endif