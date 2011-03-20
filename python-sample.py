#coding:gbk
import python_thunder_engine as p
from time import sleep
from datetime import datetime
import os
total=0
recv=0
start_time=datetime.now()


enum_errors=(
	"δ֪", #��������
	"��δ���г�ʼ��",
	"��֧�ֵ�Э�飬Ŀǰֻ֧��HTTP",
	"��ʼ������ͼ��ʧ��",
	"�������ͼ��ʧ��",
	"ָ��Ϊ��",
	"�ַ����ǿմ�",
	"�����·��û�а����ļ���",
	"����Ŀ¼ʧ��",
	"�ڴ治��",
	"�������Ϸ�",
	"���񲻴���",
	"�ļ������Ϸ�",
	"û��ʵ��",
	"�Ѿ��������������ﵽ������������޷�������������",
	"��������δ֪",
	"�ļ��Ѿ�����",
	"�ļ�������",
	"��ȡcfg�ļ�ʧ��",
	"д��cfg�ļ�ʧ��",
	"�޷��������񣬿����ǲ�֧�ֶϵ�������Ҳ�п����������Ѿ�ʧ��",
	"�޷���ͣ���񣬿����ǲ�֧�ֶϵ�������Ҳ�п����������Ѿ�ʧ��",
	"������̫С",
	"����XLInitDownloadEngine���̣߳��ڵ���XLUninitDownloadEngine֮ǰ�Ѿ�����"
)


#�����ֽ�Ϊ��λ������ת�����Ѻõ�B/KB/MB/GB/TB��ʾ��ʽ
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
	print '��������:', url
	print '����·��:', path
	if p.init():
		print '�����ʼ���ɹ�~~'
		id=p.new(url, path)
		if id>0x10000000:
			error_code=id-0x10000000
			print '������������ʧ�� :P ������:', error_code, enum_errors[error_code]
			return
		print '������������ɹ� :) �����:', id
		start_time=datetime.now()
		s=p.query(id)
		while not s=='error': 
			if s=='finished':
				print '���������^_^'
				break
			elif s=='trying':
				print '����׼������...'
			elif s=='failed':
				print '������ʧ�� 555~~~'
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
				print "����������... %s / %s �����:%s �ۼ�ƽ���ٶ�:%s ʣ��ʱ��:%s" % (recv, total, percent, speed, time_left)
				
				
			s=p.query(id) 
			#print s
			sleep(1)

if __name__=='__main__':
	url="http://xmp.down.sandai.net/kankan/XMPSetup_3.8.1.485-www.exe"
	path=r"E:\python-wrapper-center\thunder-open-engine\mywrapper\XMPSetup_3.8.1.485-www.exe"
	demo(url, path)