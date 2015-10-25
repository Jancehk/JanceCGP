#ifndef _CJANCE_DB_DLL_H
#define _CJANCE_DB_DLL_H
#include "cpss_default.h"
/*
#define JDB_INT32	VOS_INT32
#define JDB_UINT32	VOS_UINT32
#define JDB_INT16	VOS_UINT16
#define JDB_UINT16	VOS_UINT16
#define JDB_INT8	VOS_INT8
#define JDB_UINT8	VOS_UINT8
#define JDB_VOID	VOS_VOID
#define JDB_CHAR	VOS_CHAR
*/
#ifndef Jance_API_EXP
#define Jance_API_EXP __declspec(dllexport)
#endif
#ifndef Jance_API_IMP 
#define Jance_API_IMP __declspec(dllimport)
#endif

//extern "C"Jance_API_EXP int nWwww;
extern "C"Jance_API_EXP VOS_UINT32 CreateAccMDB(VOS_CHAR * m_strPath);
extern "C"Jance_API_EXP VOS_UINT32 OpenAccMDB(VOS_CHAR * m_strPath, VOS_UINT32 *adoConnect);
extern "C"Jance_API_EXP VOS_UINT32 CloseAccMDB(VOS_UINT32 adoConnect);
extern "C"Jance_API_EXP VOS_UINT32 ExecCommand(VOS_UINT32 adoConnect,VOS_CHAR * pstrCmd);
extern "C"Jance_API_EXP VOS_UINT32 ExecRecd(VOS_UINT32 adoConnect,VOS_UINT32 *adoRecord, VOS_CHAR * pstrCmd);
extern "C"Jance_API_EXP VOS_UINT32 FreeRecd(VOS_UINT32 adoRecord);
extern "C"Jance_API_EXP VOS_UINT32 GetRecordCount(VOS_UINT32 adoRecord);

#endif