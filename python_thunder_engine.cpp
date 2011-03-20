#include <boost/python.hpp>
#include <stdio.h>
#include "afxwin.h"
#include <tchar.h>
#include "XLDownload.h"
#include "XLError.h"

using namespace boost::python;
#define BUF_SIZE 300
char str_buf[BUF_SIZE];


//��ʼ����������
bool engine_init()
{
	return ( TRUE == XLInitDownloadEngine() );
}

// ж������
bool engine_exit()
{
	// TODO:
	// �����Ƿ����سɹ������������XLStopTask
	//XLStopTask(lTaskId);

	// �ͷ���Դ
	XLUninitDownloadEngine();
	return 1;
}

extern "C" {

// ��ʼһ���������񣬷�����������ţ�������������루������
int engine_new_download_task(const char * target_url, const char * save_path)
{
	LONG   lTaskId = 0;	// ���������
	CString sp=save_path;
	CString tu=target_url;
	// DWORD  dwRet = 0;
	DWORD  dwRet = XLURLDownloadToFile(
		sp, 	//����·��
		tu, 	//Ŀ���ַ����֧��http
		_T(""),		// �ο���ַ
		lTaskId
	);

	
	if ( XL_SUCCESS != dwRet )
	{
		XLUninitDownloadEngine();
		return int(dwRet);	// ʧ�ܣ����س�����
	}
	return int(lTaskId);	// �ɹ������������
}

}

// ��ѯ������ȣ����ݴ�������ʱ���ɵ�����Ų�ѯ����״̬
// ����ֵ��ʽ "״̬[|�ļ����ֽ���|�����ذٷֱ�]"
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
			// �������������
			XLStopTask(lTaskId);
			return "finished";
		}
		
		if ( 0 != ullFileSize )
		{
			double  douProgress = (double)ullRecvSize/(double)ullFileSize*100.0;
			memset(str_buf, 0, sizeof(char)*BUF_SIZE);
			// ���������У����������Ϣ
			sprintf(str_buf, "downloading|%le|%le|%f%%", (double)ullFileSize, (double)ullRecvSize, douProgress);
			return str_buf;
		}
		else
		{
			// ����׼��������Դ��ʼ����
			return "trying";
		}
		
		if ( enumTaskStatus_Fail==lStatus )
		{
			// ����������ʧ��
			XLStopTask(lTaskId);
			return "failed";
		}
	}
	// ��ѯʧ��,������������ɻ���ʧ��
	return "error";
}


BOOST_PYTHON_MODULE(python_thunder_engine)
{
	def("init", engine_init);
	def("exit", engine_exit);
	def("new", engine_new_download_task);
	def("query", engine_query_task_progress);
}
