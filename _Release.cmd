@echo off

set "VS_BIN_ROOT=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin"
if not exist "%VS_BIN_ROOT%" (
  echo %VS_BIN_ROOT% is missing!
  exit
)

set "CWD=%~dp0"

:: Win32
if exist "_out\Win32\Release" (

xcopy /D /I /Y "%VS_BIN_ROOT%\Hostx86\x86\msvcp140.dll      _out\Win32\Release
xcopy /D /I /Y "%VS_BIN_ROOT%\Hostx86\x86\vcruntime140.dll  _out\Win32\Release

7z u -stl -mx9 LogonHoursControl-1.X.X-Win32.7z             @_Release-Win32.lst
7z u -stl -mx9 LogonHoursControl-1.X.X-Win32-PDB.7z         _out\Win32\Release\*.pdb
)

:: x64
if exist "_out\x64\Release" (
xcopy /D /I /Y "%VS_BIN_ROOT%\Hostx64\x64\msvcp140.dll      _out\x64\Release
xcopy /D /I /Y "%VS_BIN_ROOT%\Hostx64\x64\vcruntime140.dll  _out\x64\Release

7z u -stl -mx9 LogonHoursControl-1.X.X-x64.7z               @_Release-x64.lst
7z u -stl -mx9 LogonHoursControl-1.X.X-x64-PDB.7z 	        _out\x64\Release\*.pdb
)
