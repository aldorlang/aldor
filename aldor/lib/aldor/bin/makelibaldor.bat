@echo off
rem
rem Creates libaldor.a given libaldor.al (release version) #
rem The Aldor Development Group (2001-2002)                #

set LIBEXT=lib
set ALDORLIBROOT=%1
set LIBDIR=%ALDORLIBROOT%\lib
set LIBAL=%LIBDIR%\%2.al
set LIBA=%LIBDIR%\%2.%LIBEXT%

cd src\util
cl /c /Ox /nologo sal_util.c

if exist %LIBA% (
	lib /nologo %LIBA% sal_util.obj
) else (
	lib /nologo /out:%LIBA% sal_util.obj
)

del /q sal_util.obj
cd ..\..

for /f %%i in ('uniar t %LIBAL%') do uniar x %LIBAL% %%i
for /f %%i in ('uniar t %LIBAL%') do aldor -fo -q5 -qinline-all -csmax=0 %%i
for /f %%i in ('uniar t %LIBAL%') do del /q %%i
for /f %%i in ('uniar t %LIBAL%') do lib /nologo %LIBA% %%~ni.obj
for /f %%i in ('uniar t %LIBAL%') do del /q %%~ni.obj


