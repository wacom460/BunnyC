@echo off
setlocal EnableDelayedExpansion
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo Error: vswhere.exe not found at "%VSWHERE%".
    echo Please ensure Visual Studio is installed.
    exit /b 1
)
for /f "usebackq tokens=*" %%a in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do (
    set "VSPATH=%%a"
)
if "%VSPATH%"=="" (
    echo Error: No Visual Studio installation with MSBuild found.
    exit /b 1
)
set "MSBUILD=%VSPATH%\MSBuild\Current\Bin\MSBuild.exe"
if not exist "%MSBUILD%" (
    echo Error: MSBuild.exe not found at "%MSBUILD%".
    exit /b 1
)

"%MSBUILD%" /p:Configuration=Debug
"C:\Program Files\BeefLang\bin\BeefIDE.exe" -launch=x64\Debug\Build.exe -launchDir=..\ -- main.ib