/*
 * =====================================================================================
 *
 *       Filename:  test_pid_init.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011��06��04�� 18ʱ42��18��
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance���繤����
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
//ͷ��
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
//������Ϣ��ʽ
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

//��Ӧ��Ϣ��ʽ
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
	{203,"Non-Authoritative information"},//��Ȩ����Ϣ
	{204,"No Content"},					  //������
	{205,"Reset Content"},				  // ��������
	{206,"Partial Content"},			  // ��������
	{300,"Multiple Choices"},			  //���ѡ��
	{301,"Moved Permanently"},			  // �����ƶ�
	{302,"Found"},						  // ����
	{303,"See Other"},					  // ������
	{304,"Not Modified"},				  // û�иı�
	{305,"Use Proxy"},					  // ʹ�ô���
	{306,"unused"},						  // û��ʹ�õ�
	{307,"Temporary Redirect"},			  // ��ʱ�ط�
	{400,"Bad Request"},				  // ������
	{401,"Unauthorized"},				  // δ��Ȩ��
	{402,"Payment Required"},			  // ��Ҫ��֧��
	{403,"Forbidden"},					  // ��ֹ
	{404,"Not Found"},					  // û���ҵ�
	{405,"Method Not Allowed"},			  // ������������
	{406,"Not Acceptable"},				  // ���ɽ��ܵ�
	{407,"Proxy Authentication Required"},// ��Ҫ������֤
	{408,"Request Timeout"},			  // ����ʱ
	{409,"Conflict"},					  // ��ͻ
	{410,"gone"},						  // ������
	{411,"Length Required"},			  // ���ȱ�Ҫ
	{412,"Precondition Failed"},		  // �Ⱦ�����ʧ��
	{413,"Request Authentication big"},	  // ����ʵ��̫��
	{414,"Request-URI too Long"},		  // ����URI̫��
	{415,"Unsupported Media Type"},		  // ����֧�ֵ�ý������
	{416,"Requested Range Not Satisfiable"},// ����Χ������
	{417,"Expectation Failed"},			  // ����ʧ��
	{500,"Internal Server Error"},		  // ���������ݴ���
	{501,"Not Implemented"},			  // ����ʵϰ
	{502,"Bad Gateway"},				  // ������
	{503,"Service Unavailable"},		  // �����ܻ��
	{504,"Gateway Timeout"},			  // ���س�ʱ
	{505,"HTTP version Not Supported"},	  // Http �汾��֧��
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
