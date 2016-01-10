/*
 * =====================================================================================
 *
 *       Filename:  test_pid_init.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年06月04日 18时42分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 *
 * =====================================================================================
 */
#ifndef _XCAP_COMMON_H
#define _XCAP_COMMON_H

#ifdef _cplusplus
extern "C" {
#endif
#include "cpss_common.h"
#include "cpss_msg.h"
#pragma comment(lib,"Wininet.lib")
#include <WININET.H>

#ifndef  CPSS_TCP_XCAP
#define  CPSS_TCP_XCAP		4030
#endif
	
#ifndef  CPSS_UDP_MONEY
#define  CPSS_UDP_MONEY		4031
#endif

#define CPSS_PID_XCAP	VOS_SOCKET_IN | VOS_TCP_PID |CPSS_TCP_XCAP
#define CPSS_PID_MONEY	VOS_SOCKET_IN | VOS_UDP_PID |CPSS_UDP_MONEY

#define XCAP_METHOD_LENGTH			16
#define XCAP_KEY_LENGTH				32
#define XCAP_VALUE_LENGTH			512
#define XCAP_REQUESURI_LENGTH		512
#define XCAP_REQUESVER_LENGTH		16
#define XCAP_FIELDS_NUM				20
#define XCAP_HOST_LENGTH			32

#define XCAP_FIELDS_HOST			7

#define XCAP_HEAD_BODY	"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\r\n"

typedef enum XCAP_REQ_METHOD {
	XCAP_REQ_GET =3,
	XCAP_REQ_PUT,
	XCAP_REQ_OTHER,
	XCAP_REQ_POST,
	XCAP_REQ_HEAD,
	XCAP_REQ_DELETE,
	XCAP_REQ_TRACE,
	XCAP_REQ_CONNECT,
};

typedef enum XCAP_RES_FIELDS {
	XCAP_RES_FIELDE_CONTENT_TYPE,
	XCAP_RES_FIELDE_SERVER,
	XCAP_RES_FIELDE_TRANSFER_ENCODING,
	XCAP_RES_FIELDE_CONTENT_LENGTH,
	XCAP_RES_FIELDE_DATE,
	XCAP_RES_FIELDE_CONNECTIN,
	XCAP_RES_FIELDE_LAST_MODIFIED
};
typedef enum XCAP_REQ_FIELDS {
	XCAP_REQ_FIELDE_ACCEPT,
	XCAP_REQ_FIELDE_ACCEPT_CHARSET,
	XCAP_REQ_FIELDE_ACCEPT_ENCODING,
	XCAP_REQ_FIELDE_ACCEPT_LANGUAGE,
	XCAP_REQ_FIELDE_AUTHORIZATION,
	XCAP_REQ_FIELDE_EXPECT,
	XCAP_REQ_FIELDE_FROM,
	XCAP_REQ_FIELDE_HOST,
	XCAP_REQ_FIELDE_IF_MATCH,
	XCAP_REQ_FIELDE_IF_MODIFIED_SINE,
	XCAP_REQ_FIELDE_IF_NONE_MATCH,
	XCAP_REQ_FIELDE_IF_RANGE,
	XCAP_REQ_FIELDE_IF_UNMODIFIED_SINCE,
	XCAP_REQ_FIELDE_MAX_FORWARDS,
	XCAP_REQ_FIELDE_PROXY_AUTHORIZATION,
	XCAP_REQ_FIELDE_RANGE,
	XCAP_REQ_FIELDE_REFERER,
	XCAP_REQ_FIELDE_TE,
	XCAP_REQ_FIELDE_CONNECTION,
	XCAP_REQ_FIELDE_USER_AGENT,
	XCAP_REQ_FIELDE_CACHE_CONTROL,
 	XCAP_REQ_FIELDE_PRAGMA,
	XCAP_REQ_FIELDE_CONTENT_LENGTH,
};
typedef enum XCAP_RES_CODE{
	XCAP_RES_CODE_100,
	XCAP_RES_CODE_101,
	XCAP_RES_CODE_200,
	XCAP_RES_CODE_201,
	XCAP_RES_CODE_202,
	XCAP_RES_CODE_203,
	XCAP_RES_CODE_204,
	XCAP_RES_CODE_205,
	XCAP_RES_CODE_206,
	XCAP_RES_CODE_300,
	XCAP_RES_CODE_301,
	XCAP_RES_CODE_302,
	XCAP_RES_CODE_303,
	XCAP_RES_CODE_304,
	XCAP_RES_CODE_305,
	XCAP_RES_CODE_306,
	XCAP_RES_CODE_307,
	XCAP_RES_CODE_400,
	XCAP_RES_CODE_401,
	XCAP_RES_CODE_402,
	XCAP_RES_CODE_403,
	XCAP_RES_CODE_404,
	XCAP_RES_CODE_405,
	XCAP_RES_CODE_406,
	XCAP_RES_CODE_407,
	XCAP_RES_CODE_408,
	XCAP_RES_CODE_409,
	XCAP_RES_CODE_410,
	XCAP_RES_CODE_411,
	XCAP_RES_CODE_412,
	XCAP_RES_CODE_413,
	XCAP_RES_CODE_414,
	XCAP_RES_CODE_415,
	XCAP_RES_CODE_416,
	XCAP_RES_CODE_417,
	XCAP_RES_CODE_500,
	XCAP_RES_CODE_501,
	XCAP_RES_CODE_502,
	XCAP_RES_CODE_503,
	XCAP_RES_CODE_504,
	XCAP_RES_CODE_505
};
//头域
typedef struct _XCAP_FIELDS_T
{
	VOS_CHAR Key[XCAP_KEY_LENGTH];
	VOS_CHAR Value[XCAP_VALUE_LENGTH];
}XCAP_FIELDS,*pXCAP_FIELDS;
typedef struct _XCAP_STATUS_CODE_T
{
	VOS_UINT16  uNum;
	VOS_UINT16  uCode;
	VOS_CHAR	phrase[XCAP_KEY_LENGTH];
}STATUS_CODE,*pSTATUS_CODE;
//请求消息格式
typedef struct _XCAP_REQUEST_HEAD_T
{
	VOS_CHAR Method[XCAP_METHOD_LENGTH];
	VOS_CHAR Request_URI[XCAP_REQUESURI_LENGTH];
	VOS_CHAR Request_version[XCAP_REQUESVER_LENGTH];
}XCAP_REQUEST_HEAD,*pXCAP_REQUEST_HEAD;
typedef struct _XCAP_REQUEST_T
{
	VOS_UINT8         Req_mothod;
	XCAP_REQUEST_HEAD Req_head;
	VOS_CHAR		  strHost[XCAP_HOST_LENGTH];
	VOS_UINT32		  uPort;
	VOS_UINT16		  fields_num;
	VOS_UINT32		  ulMsgID;
	XCAP_FIELDS       Req_head_fields[XCAP_FIELDS_NUM];
}XCAP_REQUEST,*pXCAP_REQUEST;

//响应消息格式
typedef struct _XCAP_RESPONSE_HEAD_T
{
	VOS_UINT16 StatueCode;
	VOS_CHAR   Request_version[XCAP_REQUESVER_LENGTH];
}XCAP_RESPONSE_HEAD,*pXCAP_RESPONSE_HEAD;
typedef struct _XCAP_RESPONSE_T
{
	XCAP_RESPONSE_HEAD	Res_head;
	VOS_UINT8			fields_num;
	VOS_UINT32			ulMsgID;
	XCAP_FIELDS			Res_head_fields[XCAP_FIELDS_NUM];
	VOS_CHAR		    pstrFilePath[MAX_PATH];
	VOS_CHAR			*pstuBuffer;
}XCAP_RESPONSE,*pXCAP_RESPONSE;
typedef struct _XCAP_FIELDS_TEMP_T{
	VOS_UINT8 m_num;
	XCAP_FIELDS m_fields;
	VOS_UINT8 m_fieldsLen;
}XCAP_FIELDS_TEMP,*pXCAP_FIELDS_TEMP;
typedef struct _XCAP_METHOD_T{
	VOS_UINT32 nReq_Method;
	VOS_CHAR   strReq_Method[XCAP_FIELDS_NUM];
}XCAP_METHOD,*pXCAP_METHOD;
typedef struct _XCAP_MSG_MANAGE_T{
	XCAP_REQUEST	xCap_Request_Info;
	XCAP_RESPONSE	xCap_Respone_Info;
}XCAP_MSG_MANAGE,*pXCAP_MSG_MANAGE;
static XCAP_METHOD g_xcap_method[] ={
	{XCAP_REQ_GET, "GET"},
	{XCAP_REQ_PUT, "PUT"},
	{XCAP_REQ_POST, "POST"},
	{XCAP_REQ_HEAD, "HEAD"},
	{XCAP_REQ_DELETE, "DELETE"},
	{XCAP_REQ_TRACE, "TRACE"},
	{XCAP_REQ_CONNECT, "CONNECT"},
	{XCAP_REQ_OTHER, "OTHER"},
};

static XCAP_FIELDS_TEMP g_fields_req[] = {
	{XCAP_REQ_FIELDE_ACCEPT,"Accept: ","",8},
	{XCAP_REQ_FIELDE_ACCEPT_CHARSET,"Accept-Charset: ", "", 16},
	{XCAP_REQ_FIELDE_ACCEPT_ENCODING,"Accept-Encoding: ", "", 17},
	{XCAP_REQ_FIELDE_ACCEPT_LANGUAGE,"Accept-Language: ", "", 17},
	{XCAP_REQ_FIELDE_AUTHORIZATION,"Authorization: ", "", 15},
	{XCAP_REQ_FIELDE_EXPECT,"Expect: ", "", 8},
	{XCAP_REQ_FIELDE_FROM,"From: ", "", 6},
	{XCAP_REQ_FIELDE_HOST,"Host: ", "", 6},
	{XCAP_REQ_FIELDE_IF_MATCH,"if-Match: ","", 10},
	{XCAP_REQ_FIELDE_IF_MODIFIED_SINE,"if-Modified-Since: ", "", 19},
	{XCAP_REQ_FIELDE_IF_NONE_MATCH,"if-None-Match: ", "", 15},
	{XCAP_REQ_FIELDE_IF_RANGE, "if-Range: ", "", 10},
	{XCAP_REQ_FIELDE_IF_UNMODIFIED_SINCE,"if-Unmodified-Since: ", "", 21},
	{XCAP_REQ_FIELDE_MAX_FORWARDS,"Max-Forwards: ", "", 14},
	{XCAP_REQ_FIELDE_PROXY_AUTHORIZATION,"Proxy-Authorization: ", "", 21},
	{XCAP_REQ_FIELDE_RANGE,"Range: ", "", 7},
	{XCAP_REQ_FIELDE_REFERER,"Referer: ", "", 9},
	{XCAP_REQ_FIELDE_TE,"TE: ", "", 3},
	{XCAP_REQ_FIELDE_CONNECTION,"Connection: ","",0},
	{XCAP_REQ_FIELDE_USER_AGENT,"User-Agent: ","",0},
	{XCAP_REQ_FIELDE_CACHE_CONTROL,"Cache-Control: ","",0},
	{XCAP_REQ_FIELDE_PRAGMA,"Pragma: ","",0},
	{XCAP_REQ_FIELDE_CONTENT_LENGTH,"Content-Length: ","",0},
	{XCAP_REQ_FIELDE_IF_MODIFIED_SINE,"If-Modified-Since: ", "", 19},
};

static XCAP_FIELDS_TEMP g_fields_res[] = {
	{XCAP_RES_FIELDE_CONTENT_TYPE,"Content-Type: ","",0},
	{XCAP_RES_FIELDE_SERVER,"Server: ","",0},
	{XCAP_RES_FIELDE_TRANSFER_ENCODING,"Transfer-Encoding: ","",0},
	{XCAP_RES_FIELDE_CONTENT_LENGTH,"Content-Length: ","",0},
	{XCAP_RES_FIELDE_DATE,"Date: ","",0},
	{XCAP_RES_FIELDE_CONNECTIN,"Connection: ","",0},
	{XCAP_RES_FIELDE_LAST_MODIFIED,"Last-Modified: ","",0},
};
	

static STATUS_CODE g_StatusCode[]={
	{XCAP_RES_CODE_100,100,"Continue"},
	{XCAP_RES_CODE_101,101,"Switching Protocols"},
	{XCAP_RES_CODE_200,200,"OK"},
	{XCAP_RES_CODE_201,201,"Created"},
	{XCAP_RES_CODE_202,202,"Accepted"},
	{XCAP_RES_CODE_203,203,"Non-Authoritative information"},	//非权威信息
	{XCAP_RES_CODE_204,204,"No Content"},						//无内容
	{XCAP_RES_CODE_205,205,"Reset Content"},					// 重置内容
	{XCAP_RES_CODE_206,206,"Partial Content"},					// 部分内容
	{XCAP_RES_CODE_300,300,"Multiple Choices"},					//多个选择
	{XCAP_RES_CODE_301,301,"Moved Permanently"},				// 永久移动
	{XCAP_RES_CODE_302,302,"Found"},							// 发现
	{XCAP_RES_CODE_303,303,"See Other"},						// 见其他
	{XCAP_RES_CODE_304,304,"Not Modified"},						// 没有改变
	{XCAP_RES_CODE_305,305,"Use Proxy"},						// 使用代理
	{XCAP_RES_CODE_306,306,"unused"},							// 没有使用的
	{XCAP_RES_CODE_307,307,"Temporary Redirect"},				// 临时重发
	{XCAP_RES_CODE_400,400,"Bad Request"},						// 坏请求
	{XCAP_RES_CODE_401,401,"Unauthorized"},						// 未授权的
	{XCAP_RES_CODE_402,402,"Payment Required"},					// 必要的支付
	{XCAP_RES_CODE_403,403,"Forbidden"},						// 禁止
	{XCAP_RES_CODE_404,404,"Not Found"},						// 没有找到
	{XCAP_RES_CODE_405,405,"Method Not Allowed"},				// 方法不被允许
	{XCAP_RES_CODE_406,406,"Not Acceptable"},					// 不可接受的
	{XCAP_RES_CODE_407,407,"Proxy Authentication Required"},	// 需要代理验证
	{XCAP_RES_CODE_408,408,"Request Timeout"},					// 请求超时
	{XCAP_RES_CODE_409,409,"Conflict"},							// 冲突
	{XCAP_RES_CODE_410,410,"gone"},								// 不存在
	{XCAP_RES_CODE_411,411,"Length Required"},					// 长度必要
	{XCAP_RES_CODE_412,412,"Precondition Failed"},				// 先决条件失败
	{XCAP_RES_CODE_413,413,"Request Authentication big"},		// 请求实体太大
	{XCAP_RES_CODE_414,414,"Request-URI too Long"},				// 请求URI太长
	{XCAP_RES_CODE_415,415,"Unsupported Media Type"},			// 不被支持的媒体类型
	{XCAP_RES_CODE_416,416,"Requested Range Not Satisfiable"},	// 请求范围不满足
	{XCAP_RES_CODE_417,417,"Expectation Failed"},				// 期望失败
	{XCAP_RES_CODE_500,500,"Internal Server Error"},			// 服务器内容错误
	{XCAP_RES_CODE_501,501,"Not Implemented"},					// 不能实习
	{XCAP_RES_CODE_502,502,"Bad Gateway"},						// 坏网关
	{XCAP_RES_CODE_503,503,"Service Unavailable"},				// 服务不能获得
	{XCAP_RES_CODE_504,504,"Gateway Timeout"},					// 网关超时
	{XCAP_RES_CODE_505,505,"HTTP version Not Supported"},		// Http 版本不支持
};

/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintInfo
 *  Description:  
 * ========================================================================*/
void XCAP_PrintInfo (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintErr
 *  Description:  
 * ========================================================================*/
void XCAP_PrintErr (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);
/* ===  FUNCTION  =========================================================
 *         Name:  XCAP_PrintInfo
 *  Description:  
 * ========================================================================*/
void XCAP_PrintWarn (
		VOS_STRING szFilename, 
		VOS_UINT32 ulLine,
		const VOS_STRING fmt, ...);

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_send_info
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 xcap_send_info(VOS_VOID *pVoidMsg, VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType);
/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_send_info_msgid
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 xcap_send_info_msgid(VOS_UINT32 uMsgID, VOS_VOID * pstuBuffer, VOS_UINT32 uBufLen, VOS_UINT32 uType);
/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_analyzing_info
 *  Description:  解析xcap 字符串
 *  Input      :  
 *  OutPut     :  
 *  Return     :  
 * ==========================================================================*/
VOS_UINT32 xcap_analyzing_buffer(pXCAP_REQUEST pMsgInfo, VOS_CHAR * pstrInput);

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_fileds_init
 *  Description:  初始化 头域节点
 * ==========================================================================*/
VOS_VOID xcap_fileds_init();
/* ===  FUNCTION  ==============================================================
 *         Name:  send_xcap_URL
 *  Description:  给xcap服务器发送url请求
 * ==========================================================================*/
VOS_UINT32 xcap_request_URL(pCPSS_MSG pMsgInfo);

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_responce_data
 *  Description:  发送telnet的数据
 * ==========================================================================*/
VOS_UINT32 xcap_responce_proc(pCPSS_MSG pMsgInfo);

/* ===  FUNCTION  ==============================================================
 *         Name:  xcap_analyzing_info
 *  Description:  解析xcap 字符串
 * ==========================================================================*/
//VOS_UINT32 xcap_analyzing_root(pXCAP_SER_MGR pSerMgr, pXCAP_REQUEST pMsgInfo);


#ifdef _cplusplus
}
#endif
#endif
