# 概述 #
这个扩展使得python能方便的调用迅雷开放引擎执行下载任务

# 截图 #
![http://python-thunder-engine.googlecode.com/svn-history/r2/trunk/wiki/python-thunder-demo.jpg](http://python-thunder-engine.googlecode.com/svn-history/r2/trunk/wiki/python-thunder-demo.jpg)


# python调用示例 #

```
#coding:gbk
import python_thunder_engine as p
from time import sleep
from datetime import datetime
import os
total=0
recv=0
start_time=datetime.now()


enum_errors=(
	"未知", #此条不用
	"尚未进行初始化",
	"不支持的协议，目前只支持HTTP",
	"初始化托盘图标失败",
	"添加托盘图标失败",
	"指针为空",
	"字符串是空串",
	"传入的路径没有包含文件名",
	"创建目录失败",
	"内存不足",
	"参数不合法",
	"任务不存在",
	"文件名不合法",
	"没有实现",
	"已经创建的任务数达到最大任务数，无法继续创建任务",
	"任务类型未知",
	"文件已经存在",
	"文件不存在",
	"读取cfg文件失败",
	"写入cfg文件失败",
	"无法继续任务，可能是不支持断点续传，也有可能是任务已经失败",
	"无法暂停任务，可能是不支持断点续传，也有可能是任务已经失败",
	"缓冲区太小",
	"调用XLInitDownloadEngine的线程，在调用XLUninitDownloadEngine之前已经结束"
)


#将以字节为单位的数字转化成友好的B/KB/MB/GB/TB表示形式
def friendly_repr(bytes):
	if bytes<1024:
		return '%dB' % bytes
	else:
		bytes /= 1024
		if bytes<1024:
			return '%.1fKB' % bytes
		else:
			bytes /= 1024
			if bytes<1024:
				return '%.1fMB' % bytes
			else:
				bytes /= 1024
				if bytes<1024:
					return '%.1fGB' % bytes
				else:
					bytes /= 1024
					if bytes<1024:
						return '%.1fTB' % bytes
					else:
						bytes /= 1024
						return '%.1fPB' % bytes

def demo(url, path):
	print '下载任务:', url
	print '保存路径:', path
	os.system('rm '+path+'.td')
	os.system('rm '+path+'.td.cfg')
	if p.init():
		print '引擎初始化成功~~'
		id=p.new(url, path)
		if id>0x10000000:
			error_code=id-0x10000000
			print '创建下载任务失败 :P 错误码:', error_code, enum_errors[error_code]
			return
		print '创建下载任务成功 :) 任务号:', id
		start_time=datetime.now()
		s=p.query(id)
		while not s=='error': 
			if s=='finished':
				print '下载已完成^_^'
				break
			elif s=='trying':
				print '正在准备下载...'
			elif s=='failed':
				print '下载已失败 555~~~'
				break
			else:
				(status, total, recv, percent) = list(s.split('|'))
				speed=float(recv)/(datetime.now()-start_time).seconds
				if speed!=0:
					time_left='%.1f' % ((float(total)-float(recv))/speed) +'S'
				else:
					time_left='-/-'
				speed=friendly_repr(speed)+'/S'
				recv=friendly_repr(float(recv))
				total=friendly_repr(float(total))
				percent=('%.1f' % float(percent[:-1]))+'%'
				print "正在下载中... %s / %s 已完成:%s 累计平均速度:%s 剩余时间:%s" % (recv, total, percent, speed, time_left)
				
				
			s=p.query(id) 
			#print s
			sleep(1)

if __name__=='__main__':
	url="http://xmp.down.sandai.net/kankan/XMPSetup_3.8.1.485-www.exe"
	path=r"E:\a\b\c\XMPSetup_3.8.1.485-www.exe"
	demo(url, path)
```


# 模块编译代码 #
```
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

```

# 编译脚本 #
```
@echo off

set cpp=python_thunder_engine.cpp

::使用vc2005编译
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat"

::initial includes and libs
set includes=
set libpaths=/EXPAT_LIBPATH
set libs=

::set environment for boost
set includes=%includes% /I D:\cpp-dev-packages\boost\boost_1_44_0
set libpaths=%libpaths%:"D:\cpp-dev-packages\boost\boost_1_44_0\bin\vc9\lib\"
::set libs=%libs% libboost_python-vc90-mt-s.lib libboost_python-vc90-mt-sgd.lib
set libs=%libs% libboost_python-vc90-mt-s.lib

::set environment for python extension
set includes=%includes% /I C:\Python26\include
set libpaths=%libpaths%"c:\Python26\libs\"
set libs=%libs% python26.lib

::generate cmd line and run
::编译python-thunder-engine扩展 @2011年3月19日17:31:09
set cmdline=cl /LD /DUNICODE /D_UNICODE /Od /D "WIN32" /D "_WINDOWS" /EHsc  /W3  /INCREMENTAL /DNDEBUG /DYNAMICBASE:NO /ERRORREPORT:PROMPT  /DBOOST_PYTHON_STATIC_LIB XLDownload.lib %includes%  %libs% %cpp% /o python_thunder_engine.pyd

%cmdline%

pause
```