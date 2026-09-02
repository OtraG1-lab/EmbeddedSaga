If you run:

```bash
$ docker build -f Dockerfile -t final-app .
```
With the final-app image, you can create a container solely for compilation purposes. Docker caches the previous commands, so when you run this command, it will not start from scratch but only execute the latest command where you wish to compile your application. The compilation process will begin in the image, then, as before, create a temporary container and copy your binary:

```bash
$ docker create --name tmpapp final-app
$ docker cp tmpapp:/projectPath/HelloQt6 ./HelloQt6


refer the repo https://github.com/PhysicsX/QTonRaspberryPi for more details, based on this we have prepared the docker images and pushed to GHCR
crossbuild image : ghcr.io/otrag1-lab/rpiqtcrossbuild:26.8
base image:  ghcr.io/otrag1-lab/raspimage:26.8 

```
```deploy the app on Raspberry Pi

To test the hello world, you need to copy and send the compiled qt binaries in the image.

$ docker cp tmpbuild:/build/qt-pi-binaries.tar.gz ./qt-pi-binaries.tar.gz
$ scp qt-pi-binaries.tar.gz ulas@192.168.16.20:/home/ulas/
$ ssh rasp@192.168.16.25
$ ulas@raspberrypi:~ sudo mkdir /usr/local/qt6
$ ulas@raspberrypi:~ sudo tar -xvf qt-pi-binaries.tar.gz -C /usr/local/qt6
$ ulas@raspberrypi:~ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/qt6/lib/
Extract it under /usr/local or wherever you want and do not forget to add the path to LD_LIBRARY_PATH in case of path is not in the list.

ulas@raspberrypi:~ $ ./HelloQt6
Hello world
