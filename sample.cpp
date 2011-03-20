#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "XLDownload.h"
#include "XLError.h"

extern "C" {

int main()
{
     // 1����ʼ����������
     if ( FALSE == XLInitDownloadEngine() )
     {
          printf("Initialize download engine failed.\n");
          return 1;
     }

     // 2������������
     // PS����������޷�ͨ������ѹ������ԡ���wchar_t����Ϊ��������        (/Zc:wchar_t)������ΪYes
     LONG   lTaskId = 0;
     DWORD  dwRet   = XLURLDownloadToFile(
                       _T("d:\\xmp.exe"), 
                       _T("http://xmp.down.sandai.net/kankan/XMPSetup_3.8.1.485-www.exe"), 
                       _T(""), lTaskId);

     if ( XL_SUCCESS != dwRet )
     {
          XLUninitDownloadEngine();
          printf("Create new task failed, error code:%d.\n", dwRet-0x10000000);
          return 1;
     }

     printf("Begin download file.\n");

     // 3����ѯ����״̬
     do 
     {
          ::Sleep(1000);

          ULONGLONG   ullFileSize = 0;
          ULONGLONG   ullRecvSize = 0;
          LONG        lStatus     = -1;

          dwRet = XLQueryTaskInfo(lTaskId, &lStatus, &ullFileSize, &ullRecvSize);
          if ( XL_SUCCESS==dwRet )
          {
               // ���������Ϣ
               if ( 0 != ullFileSize )
               {
                    double  douProgress = (double)ullRecvSize/(double)ullFileSize;
                    douProgress *= 100.0;
                    printf("Download progress:%.2f%%\n", douProgress);
               }
               else
               {
                    printf("File size is zero.\n");
               }

               if ( enumTaskStatus_Success==lStatus )
               {
                    printf("Download successfully.\n");
                    break;
               }

               if ( enumTaskStatus_Fail==lStatus )
               {
                    printf("Download failed.\n");
                    break;
               }
          }
     } while( XL_SUCCESS==dwRet );

     // 4�������Ƿ����سɹ������������XLStopTask
     XLStopTask(lTaskId);

     // 5���ͷ���Դ
     XLUninitDownloadEngine();
     return 0;
}

}