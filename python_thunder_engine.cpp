#include <boost/python.hpp>
#include <stdio.h>
#include "afxwin.h"
#include <tchar.h>
#include "XLDownload.h"
#include "XLError.h"

using namespace boost::python;
#define BUF_SIZE 300
char str_buf[BUF_SIZE];


//初始化下载引擎
bool engine_init()
{
	return ( TRUE == XLInitDownloadEngine() );
}

// 卸载引擎
bool engine_exit()
{
	// TODO:
	// 无论是否下载成功，都必须调用XLStopTask
	//XLStopTask(lTaskId);

	// 释放资源
	XLUninitDownloadEngine();
	return 1;
}

extern "C" {

// 开始一个下载任务，返回下载任务号（整数）或错误码（负数）
int engine_new_download_task(const char * target_url, const char * save_path)
{
	LONG   lTaskId = 0;	// 保存任务号
	CString sp=save_path;
	CString tu=target_url;
	// DWORD  dwRet = 0;
	DWORD  dwRet = XLURLDownloadToFile(
		sp, 	//保存路径
		tu, 	//目标地址，仅支持http
		_T(""),		// 参考网址
		lTaskId
	);

	
	if ( XL_SUCCESS != dwRet )
	{
		XLUninitDownloadEngine();
		return int(dwRet);	// 失败，返回出错码
	}
	return int(lTaskId);	// 成功，返回任务号
}

}

// 查询任务进度，根据创建任务时生成的任务号查询任务状态
// 返回值格式 "状态[|文件总字节数|已下载百分比]"
const char * engine_query_task_progress(int task_id) {
	
	LONG		lTaskId = (LONG)task_id;
	ULONGLONG 	ullFileSize = 0;
	ULONGLONG 	ullRecvSize = 0;
	LONG 		lStatus = -1;

	DWORD  dwRet = XLQueryTaskInfo(lTaskId, &lStatus, &ullFileSize, &ullRecvSize);
	if ( XL_SUCCESS==dwRet )
	{
		if ( enumTaskStatus_Success==lStatus )
		{
			// 下载任务已完成
			XLStopTask(lTaskId);
			return "finished";
		}
		
		if ( 0 != ullFileSize )
		{
			double  douProgress = (double)ullRecvSize/(double)ullFileSize*100.0;
			memset(str_buf, 0, sizeof(char)*BUF_SIZE);
			// 正在下载中，输出进度信息
			sprintf(str_buf, "downloading|%le|%le|%f%%", (double)ullFileSize, (double)ullRecvSize, douProgress);
			return str_buf;
		}
		else
		{
			// 正在准备配置资源开始下载
			return "trying";
		}
		
		if ( enumTaskStatus_Fail==lStatus )
		{
			// 下载任务已失败
			XLStopTask(lTaskId);
			return "failed";
		}
	}
	// 查询失败,可能任务已完成或者失败
	return "error";
}


BOOST_PYTHON_MODULE(python_thunder_engine)
{
	def("init", engine_init);
	def("exit", engine_exit);
	def("new", engine_new_download_task);
	def("query", engine_query_task_progress);
}
