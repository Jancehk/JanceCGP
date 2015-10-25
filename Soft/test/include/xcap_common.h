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

#define XCAP_METHOD_LENGTH			16
#define XCAP_KEY_LENGTH				32
#define XCAP_VALUE_LENGTH			128
#define XCAP_REQUESURI_LENGTH		512
#define XCAP_REQUESVER_LENGTH		16
#define XCAP_FIELDS_NUM				20
	
#define XCAP_REQ_OTHER				1
#define XCAP_REQ_GET				2
#define XCAP_REQ_PUT				3
#define XCAP_REQ_POST				4
#define XCAP_REQ_HEAD				5
#define XCAP_REQ_DELETE				6
#define XCAP_REQ_TRACE				7
#define XCAP_REQ_CONNECT			8
//头域
typedef struct _XCAP_FIELDS_T
{
	VOS_CHAR Key[XCAP_KEY_LENGTH];
	VOS_CHAR Value[XCAP_VALUE_LENGTH];
}XCAP_FIELDS,*pXCAP_FIELDS;
typedef struct _XCAP_STATUS_CODE_T
{
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
	VOS_UINT16		  fields_num;
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
	XCAP_RESPONSE_HEAD Res_head;
	VOS_UINT16		   fields_num;
	XCAP_FIELDS		   Res_head_fields[XCAP_FIELDS_NUM];
}XCAP_RESPONSE,*pXCAP_RESPONSE;
/*
static XCAP_FIELDS g_fields[] = {
	{1,"Accept",""},
	{2,"Accept-Charset",""},
	{3,"Accept-Encoding",""},
	{4,"Accept-Language",""},
	{5,"Authorization",""},
	{6,"Expect",""},
	{7,"From",""},
	{8,"Host",""},
	{9,"if-Match",""},
	{10,"if-Modified-Since",""},
	{11,"if-None-Match",""},
	{12,"if-Range",""},
	{13,"if-Unmodified-Since",""},
	{14,"Max-Forwards",""},
	{15,"Proxy-Authorization",""},
	{16,"Range",""},
	{17,"Referer",""},
	{18,"TE",""},
};
*/
static STATUS_CODE g_StatusCode[]={
	{100,"Continue"},
	{101,"Switching Protocols"},
	{200,"OK"},
	{201,"Created"},
	{202,"Accepted"},
	{203,"Non-Authoritative information"},//非权威信息
	{204,"No Content"},					  //无内容
	{205,"Reset Content"},				  // 重置内容
	{206,"Partial Content"},			  // 部分内容
	{300,"Multiple Choices"},			  //多个选择
	{301,"Moved Permanently"},			  // 永久移动
	{302,"Found"},						  // 发现
	{303,"See Other"},					  // 见其他
	{304,"Not Modified"},				  // 没有改变
	{305,"Use Proxy"},					  // 使用代理
	{306,"unused"},						  // 没有使用的
	{307,"Temporary Redirect"},			  // 临时重发
	{400,"Bad Request"},				  // 坏请求
	{401,"Unauthorized"},				  // 未授权的
	{402,"Payment Required"},			  // 必要的支付
	{403,"Forbidden"},					  // 禁止
	{404,"Not Found"},					  // 没有找到
	{405,"Method Not Allowed"},			  // 方法不被允许
	{406,"Not Acceptable"},				  // 不可接受的
	{407,"Proxy Authentication Required"},// 需要代理验证
	{408,"Request Timeout"},			  // 请求超时
	{409,"Conflict"},					  // 冲突
	{410,"gone"},						  // 不存在
	{411,"Length Required"},			  // 长度必要
	{412,"Precondition Failed"},		  // 先决条件失败
	{413,"Request Authentication big"},	  // 请求实体太大
	{414,"Request-URI too Long"},		  // 请求URI太长
	{415,"Unsupported Media Type"},		  // 不被支持的媒体类型
	{416,"Requested Range Not Satisfiable"},// 请求范围不满足
	{417,"Expectation Failed"},			  // 期望失败
	{500,"Internal Server Error"},		  // 服务器内容错误
	{501,"Not Implemented"},			  // 不能实习
	{502,"Bad Gateway"},				  // 坏网关
	{503,"Service Unavailable"},		  // 服务不能获得
	{504,"Gateway Timeout"},			  // 网关超时
	{505,"HTTP version Not Supported"},	  // Http 版本不支持
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

#ifdef _cplusplus
}
#endif
#endif
