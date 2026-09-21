\# Qt Application Cross Compilation for Raspberry Pi



\## Overview



This project demonstrates how to cross-compile a Qt 6 QML application for a Raspberry Pi 4 using Docker on a Windows development machine.



The application is developed on Windows and compiled inside a Docker container configured for the Raspberry Pi target environment.



\## Project Structure



```text

QTApponRpi/

├── Dockerfile

├── README.md

└── project/

&#x20;   ├── CMakeLists.txt

&#x20;   ├── main.cpp

&#x20;   └── Main.qml

```



\## Tools and Technologies



\* Qt 6

\* Qt Quick / QML

\* C++

\* CMake

\* Docker

\* Raspberry Pi 4

\* Raspberry Pi OS

\* Windows



\## Docker Images



The cross-compilation environment is based on the following images:



\* Base image: `ghcr.io/otrag1-lab/raspimage:26.8`

\* Qt cross-build image: `ghcr.io/otrag1-lab/rpiqtcrossbuild:26.8`



\## Cross Compilation Flow



The overall process is:



```text

Windows PC

&#x20;   |

&#x20;   | Docker build

&#x20;   v

Docker Cross-Compilation Environment

&#x20;   |

&#x20;   | CMake + Qt cross compilation

&#x20;   v

Raspberry Pi executable

&#x20;   |

&#x20;   | SCP

&#x20;   v

Raspberry Pi 4

&#x20;   |

&#x20;   | Execute

&#x20;   v

Qt QML Application

```



\## Build the Docker Image



Open a terminal in the project directory and run:



```bash

docker build -f Dockerfile -t final-app .

```



This creates the Docker image `final-app`.



Docker can reuse previously built layers, so subsequent builds can be faster when only the application source changes.



\## Create a Temporary Container



Create a container from the build image:



```bash

docker create --name tmpapp final-app

```



The container is used as a temporary environment from which the generated Raspberry Pi executable can be copied.



\## Copy the Generated Executable



Copy the executable from the container to the host machine:



```bash

docker cp tmpapp:/home/otra/QTonRaspberryPi/project/appRaspberryPiDemo .

```



The generated executable is then available on the Windows development machine.



\## Transfer the Application to Raspberry Pi



Use SCP to transfer the executable to the Raspberry Pi:



```bash

scp appRaspberryPiDemo <username>@<raspberry-pi-ip>:\~/

```



Replace `<username>` with the Raspberry Pi username and `<raspberry-pi-ip>` with the Raspberry Pi IP address.



\## Run on Raspberry Pi



Connect to the Raspberry Pi using SSH:



```bash

ssh <username>@<raspberry-pi-ip>

```



Give execute permission if required:



```bash

chmod +x appRaspberryPiDemo

```



Run the application:



```bash

./appRaspberryPiDemo

```



\## Result



The Qt application is compiled on the Windows host using the Docker-based cross-compilation environment and the resulting executable is transferred to and executed on the Raspberry Pi 4.



\## Reference



The Docker cross-compilation setup was based on the following project:



`https://github.com/PhysicsX/QTonRaspberryPi`



