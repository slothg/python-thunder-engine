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
	path=r"E:\python-wrapper-center\thunder-open-engine\mywrapper\XMPSetup_3.8.1.485-www.exe"
	demo(url, path)