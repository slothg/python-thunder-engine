#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "XLDownload.h"
#include "XLError.h"

extern "C" {

int main()
{
     // 1、初始化下载引擎
     if ( FALSE == XLInitDownloadEngine() )
     {
          printf("Initialize download engine failed.\n");
          return 1;
     }

     // 2、启动新任务
     // PS：如果链接无法通过，请把工程属性“将wchar_t设置为内置类型        (/Zc:wchar_t)”设置为Yes
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

     // 3、查询任务状态
     do 
     {
          ::Sleep(1000);

          ULONGLONG   ullFileSize = 0;
          ULONGLONG   ullRecvSize = 0;
          LONG        lStatus     = -1;

          dwRet = XLQueryTaskInfo(lTaskId, &lStatus, &ullFileSize, &ullRecvSize);
          if ( XL_SUCCESS==dwRet )
          {
               // 输出进度信息
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

     // 4、无论是否下载成功，都必须调用XLStopTask
     XLStopTask(lTaskId);

     // 5、释放资源
     XLUninitDownloadEngine();
     return 0;
}

}