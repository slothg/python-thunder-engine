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
