@echo off
setlocal
cd /d "%~dp0.."

set "PACKAGE_NAME=dlt-ubuntu-amd64"
set "OUTPUT_DIR=build\%PACKAGE_NAME%"

if exist "%OUTPUT_DIR%" rmdir /s /q "%OUTPUT_DIR%"
mkdir "%OUTPUT_DIR%" || exit /b 1

docker buildx build --platform linux/amd64 --target package-export --output "type=local,dest=%OUTPUT_DIR%" .
if errorlevel 1 exit /b %ERRORLEVEL%

move /y "%OUTPUT_DIR%\dlt-package.tar.gz" "%PACKAGE_NAME%.tar.gz" >nul
if errorlevel 1 exit /b %ERRORLEVEL%

echo Created %PACKAGE_NAME%.tar.gz
