@echo off
setlocal
cd /d "%~dp0.."

set "IMAGE_NAME=dlt-ubuntu:local"
set "CONTAINER_NAME=dlt-ubuntu"

docker build --target ubuntu-runtime --tag "%IMAGE_NAME%" .
if errorlevel 1 exit /b %ERRORLEVEL%

docker rm --force "%CONTAINER_NAME%" >nul 2>&1
docker run --detach --name "%CONTAINER_NAME%" --init --publish 3490:3490 "%IMAGE_NAME%"
if errorlevel 1 exit /b %ERRORLEVEL%

docker exec --detach "%CONTAINER_NAME%" dlt_sample_app
if errorlevel 1 exit /b %ERRORLEVEL%

echo DLT daemon and sample app are running in %CONTAINER_NAME%.
echo View logs with: docker logs -f %CONTAINER_NAME%
echo Stop with: docker rm -f %CONTAINER_NAME%
