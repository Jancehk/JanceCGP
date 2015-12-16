/*==============================================================================
 *       Filename:  cpss_com_app.c
 *    Description:  CGP命令处理及平台初始化
 *        Version:  1.0
 *        Created:  2011年05月21日 21时59分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  Jance (), shenghs2004@163.com
 *        Company:  Jance网络工作室
 * ============================================================================*/

#include	"cpss_common.h"
#include 	"cpss_com_app.h"
#include	"cpss_com_pid.h"
#include 	"cpss_vk_file.h"
#include	"cpss_vos_framwork.h"

#define N	NONE
#define R	RED
#define B   BLUE
extern VOS_UINT32 cpss_pid_init ();
static VOS_VOID shell_print_hello()
{
	#if (OS_TYPE == OS_TYPE_WINDOW)
	VOS_CHAR *szHello[14] = {"====================================================",
	"==---N-NNNNNN-----****JJJJJJJJJJ****-----SSS------==",
	"==---NNN------**-------**-JJ-**------**SS----SS---==",
	"==---NN-----**NN----------**-----------**S--------==",	
	"==---NN------**N----------JJ----------**-SS-------==",
	"==---NN-------N**---------JJ--------**----SS------==",
	"==---NN-------NN--**------JJ-----**---------SS----==",
	"==---NN-------NN------**--JJ--**-----SS----SS-----==",
	"==---NN-------NN--------JJ**-----------SSS--------==",
	"====================================================",
	"=================By Jance@NJS.com===================",
	"========================CGP=========================",
	"===================================================="};
	int i =0;
	for (i=0;i<13;i++)
	{
		VOS_PrintInfo("",CPSS_PRINTF_BUFFER,szHello[i]);
	}
	#elif (OS_TYPE == OS_TYPE_LINUX)
	printf(R"====================================================\n");
	printf("="N"----"B"N"N"-"B"NNNNNN"N"-----"R"****"B"JJJJJJJJJJ"R"****"N"\
	-----"B"SSS"N"-------"R"=\n");
	printf("="N"----"B"NNN"N"------"B"N"R"**"N"------"R"**"N"-"B"JJ"N"-"R"\
	**"N"------"R"**"B"SS"N"----"B"SS"N"----"R"=\n");
	printf("="N"----"B"NN"N"------"R"**"B"N"N"----------"R"**"N"-----------\
	"R"**"B"S"N"---------"R"=\n");	
	printf("="N"----"B"NN"N"-------"R"**"N"----------"B"JJ"N"----------\
	"R"**"N"-"B"SS"N"--------"R"=\n");
	printf("="N"----"B"NN"N"-------"B"N"R"**"N"---------"B"JJ"N"--------\
	"R"**"N"----"B"SS"N"-------"R"=\n");
	printf("="N"----"B"NN"N"-------"B"NN"N"--"R"**"N"------"B"JJ"N"-----\
	"R"**"N"---------"B"SS"N"-----"R"=\n");
	printf("="N"----"B"NN"N"-------"B"NN"N"------"R"**"N"--"B"JJ"N"--"R"**"N"\
	-----"B"SS"N"----"B"SS"N"------"R"=\n");
	printf("="N"----"B"NN"N"-------"B"NN"N"--------"B"JJ"R"**"N"-----------\
	"B"SSS"N"---------"R"=\n");
	printf("==================================================="R"=\n");
	printf("=               "B"By: NTS And NJS"N"                    "R"=\n");
	printf("=                    "R"CGP"N"                           "R"=\n");
	printf("====================================================\n"N);
	#endif
}
/* ===  FUNCTION  ============================================================
 *         Name:  cpss_init
 *  Description:	CGP   
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * ===========================================================================*/
static VOS_UINT32 cpss_init ()
{
	VOS_UINT32 ulRet = VOS_ERR;
	
	// 内存管理初始化
	if (VOS_OK != cpss_init_mem())
	{
		//VOS_PrintErr(__FILE__, __LINE__, "CGP Init Memory Module is Error");
		printf("CGP Init Memory Module is Error");
		return ulRet;
	}
	//打印平台初始化
	if(VOS_OK != cpss_print_init())
	{
		printf("CGP Init Print Module is Error");
		return ulRet;
	}
	shell_print_hello();
	if (VOS_OK != cpss_init_mul_timer(1000))
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Init Timer is Error");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__,__LINE__,"Init MEM OK");

	//pid 管理初始化
	if(VOS_OK != cpss_pid_init())
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Init PID Module is Error");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__,__LINE__,"Init PID OK");

	// 消息队列初始化
	if (VOS_OK != cpss_init_msg_sem())
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Init MSG Module is Error");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__,__LINE__,"Init MSG OK");

	// socket初始化
	if (VOS_OK != cpss_socket_init())
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Init SKT Module is Error");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__,__LINE__,"Init SKT OK");

	return VOS_OK;
}		/* -----  end of static function swp_init  ----- */


/* ===  FUNCTION  ============================================================
 *         Name:  cpss_system_init
 *  Description:	初始化平台
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * ===========================================================================*/
static VOS_UINT32 cpss_system_init ()
{
	VOS_UINT32 ulRet = VOS_ERR;
	// Frame侧初始化
	if (VOS_OK != cpss_framwork_init())
	{
		VOS_PrintErr(__FILE__, __LINE__, "Init Framwork Server is Error");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__,__LINE__,"Init Framwork Server OK");
	// telnet 服务器初始化
	if (VOS_OK != cpss_telnet_init())
	{
		VOS_PrintErr(__FILE__, __LINE__, "Init Telnet Server is Error");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__,__LINE__,"Init Telnet Server OK");
	return VOS_OK;
}

/*===  FUNCTION  ===============================================================
 *         Name:  swp_int
 *  Description:	CGP 初始化  
 *  Input      :	
 *  OutPut     :	
 *  Return     :	
 * =============================================================================*/
VOS_UINT32 swp_init (int argc, char ** argv )
{
	VOS_UINT32 ulRet = VOS_ERR;

	//VOS_PrintErr(__FILE__,__LINE__,"%s","This My CGP");
	cpss_set_current_path(*argv);
	if(VOS_OK != cpss_init(argc,argv))
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP CPSS_INIT Error!");
		return  ulRet;
	}
	/******************************/
	/*          PID INIT          */
	/******************************/
	if(VOS_OK != VOS_Registmsgporcess())
	{
		VOS_PrintErr(__FILE__, __LINE__, "Regist Process Error!");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__, __LINE__, "CGP SYSTEM LOAD OK");

	if(VOS_OK != cpss_system_init())
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP CPSS_SYSTEM_INIT Error!");
		return  ulRet;
	}
	VOS_PrintInfo(__FILE__, __LINE__, "CGP SYSTEM INIT OK");
	/******************************/
	/*         IOCP INIT          */
	/******************************/
	if(VOS_OK != cpss_iocp_init())
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP IOCP_INIT Error");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__, __LINE__, "CGP IOCP_INIT OK");
	
	ulRet = cpss_subsystem_init(cps_set_msg_type(CPSS_OBJ_PID_FW, CPSS_SYSTEM, CPSS_TYPE_SYS, CPSS_MSG_INIT), CPSS_CMD_SYSTEM_INIT);
	if (VOS_OK != ulRet)
	{
		VOS_PrintErr(__FILE__, __LINE__, "CGP Sub System Init Error");
		return ulRet;
	}
	VOS_PrintInfo(__FILE__, __LINE__, "CGP Sub System Init OK");

	if (argc>1)
	{
		VOS_PrintInfo("",CPSS_PRINTF_BUFFER,"%d,%s\n",argc, *(argv+1));
	}
	else
	{
	}

	return VOS_OK;
}		/* -----  end of function cpss_int  ----- */

/*===  FUNCTION  ===============================================================
 *         Name:  cpss_shell_cmd_init
 *  Description:	命令函数初始化  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 cpss_shell_cmd_init (  )
{
	return VOS_OK;
}		/* -----  end of function cpss_shell_cmd_init  ----- */

/*===  FUNCTION  ===============================================================
 *         Name:  initmsgporcess
 *  Description:	函数消息初始化  
 *  Input      :	
 *  OutPut     :	
 *  Return     :    
 * =============================================================================*/
VOS_UINT32 VOS_Registmsgporcess()
{
	return VOS_ProcessInit();
}		/* -----  end of function initmsgporcess  ----- */
