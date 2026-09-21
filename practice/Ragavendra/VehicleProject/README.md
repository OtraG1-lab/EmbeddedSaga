\# Vehicle CommonAPI + Franca + D-Bus



\## 1. Project Overview



This project demonstrates a basic \*\*automotive vehicle service\*\* using:



\* Franca IDL (FIDL)

\* CommonAPI

\* CommonAPI D-Bus

\* D-Bus

\* C++

\* CMake

\* Docker

\* ARM64 cross-compilation

\* Raspberry Pi



The main purpose of this project is to understand how an interface is defined using \*\*Franca IDL\*\*, how CommonAPI code is generated, how the D-Bus binding is generated, and how a client communicates with a server through CommonAPI and D-Bus.



\---



\## 2. Communication Architecture



The communication flow is:



```text

+-------------------+

|   Vehicle Client  |

+-------------------+

&#x20;         |

&#x20;         v

+-------------------+

| CommonAPI Proxy   |

+-------------------+

&#x20;         |

&#x20;         v

+-------------------+

| CommonAPI D-Bus   |

|     Binding      |

+-------------------+

&#x20;         |

&#x20;         v

+-------------------+

|      D-Bus       |

+-------------------+

&#x20;         |

&#x20;         v

+-------------------+

| CommonAPI D-Bus   |

|  Stub Adapter     |

+-------------------+

&#x20;         |

&#x20;         v

+-------------------+

|  CommonAPI Stub   |

+-------------------+

&#x20;         |

&#x20;         v

+-------------------+

|  Vehicle Service  |

+-------------------+

```



In simple words:



```text

Client

&#x20; |

&#x20; | CommonAPI

&#x20; v

D-Bus

&#x20; |

&#x20; | CommonAPI

&#x20; v

Server

```



The client and server are separate applications. CommonAPI provides the programming interface, while D-Bus is used as the Linux IPC communication mechanism.



\---



\# 3. Vehicle Example



A simple vehicle speed service was implemented.



The service contains:



```text

Speed = 80 km/h

```



The client can request the current speed and request a speed change.



For example:



```text

Client -> setSpeed(100)

```



The server updates the speed:



```text

speed = 100

```



The server can then notify clients that the speed has changed.



\---



\# 4. Franca IDL



Franca IDL is used to define the service interface.



The FIDL used in this project is:



```fidl

package com.example.vehicle



interface Vehicle {

&#x20;   version {

&#x20;       major 1

&#x20;       minor 0

&#x20;   }



&#x20;   attribute UInt32 speed



&#x20;   method getSpeed {

&#x20;       out {

&#x20;           UInt32 speed

&#x20;       }

&#x20;   }



&#x20;   method setSpeed {

&#x20;       in {

&#x20;           UInt32 speed

&#x20;       }

&#x20;   }



&#x20;   broadcast speedChanged {

&#x20;       out {

&#x20;           UInt32 speed

&#x20;       }

&#x20;   }

}

```



\---



\# 5. FIDL Elements



\## Package



```fidl

package com.example.vehicle

```



Defines the namespace of the interface.



\## Interface



```fidl

interface Vehicle

```



Defines the vehicle service interface.



\## Version



```fidl

version {

&#x20;   major 1

&#x20;   minor 0

}

```



Defines the interface version.



\## Attribute



```fidl

attribute UInt32 speed

```



Represents the vehicle speed/state.



\## Method



```fidl

method getSpeed

```



Used by the client to request the current speed.



\## Method



```fidl

method setSpeed

```



Used by the client to request a speed change.



\## Broadcast



```fidl

broadcast speedChanged

```



Used by the service to notify clients when the speed changes.



\---



\# 6. Attribute, Method and Broadcast



These three concepts are important in Franca.



```text

Attribute

&#x20;   ↓

State / Data



Method

&#x20;   ↓

Request / Action



Broadcast

&#x20;   ↓

Event / Notification

```



Example:



```text

Attribute:

speed = 80



Method:

setSpeed(100)



Broadcast:

speedChanged(100)

```



\---



\# 7. CommonAPI



CommonAPI provides a middleware abstraction between the application and the communication mechanism.



Instead of directly writing low-level D-Bus communication code, the application uses generated CommonAPI classes.



The basic concept is:



```text

Application

&#x20;    |

&#x20;    v

CommonAPI

&#x20;    |

&#x20;    v

Communication Binding

&#x20;    |

&#x20;    v

D-Bus

```



CommonAPI uses generated \*\*Proxy\*\* classes on the client side and \*\*Stub\*\* classes on the server side.



\---



\# 8. CommonAPI Core Generator



The CommonAPI Core Generator was used to generate the generic CommonAPI source files from the FIDL.



Generator location:



```text

C:\\CommonAPI\\core

```



Generation command:



```cmd

C:\\CommonAPI\\core\\commonapi-core-generator-windows-x86\_64.exe -sk Vehicle.fidl

```



The Core generator produces files such as:



```text

Vehicle.hpp

VehicleProxy.hpp

VehicleProxyBase.hpp

VehicleStub.hpp

VehicleStubDefault.hpp

```



These files provide the generated CommonAPI interface, proxy and stub definitions.



\---



\# 9. CommonAPI D-Bus Generator



The CommonAPI D-Bus Generator was used to generate the D-Bus binding code.



Generator location:



```text

C:\\CommonAPI\\dbus

```



Generation command:



```cmd

C:\\CommonAPI\\dbus\\commonapi-dbus-generator-windows-x86\_64.exe -d src-gen Vehicle.fidl

```



The D-Bus generator produces files such as:



```text

VehicleDBusDeployment.hpp

VehicleDBusDeployment.cpp



VehicleDBusProxy.hpp

VehicleDBusProxy.cpp



VehicleDBusStubAdapter.hpp

VehicleDBusStubAdapter.cpp

```



These files connect the generated CommonAPI interface to D-Bus.



\---



\# 10. Generated Files



The generated files are stored under:



```text

src-gen/

```



\## CommonAPI files



\### Vehicle.hpp



Contains the generated interface definition.



\### VehicleProxy.hpp



Used by the client.



The client communicates with the vehicle service through the generated proxy.



\### VehicleProxyBase.hpp



Contains common proxy functionality and generated attribute/event definitions.



\### VehicleStub.hpp



Used by the server.



The service implementation derives from the generated stub.



\### VehicleStubDefault.hpp



Provides default stub functionality and generated helper/storage implementations.



\---



\# 11. D-Bus Generated Files



\### VehicleDBusDeployment



Contains D-Bus deployment information.



```text

VehicleDBusDeployment.hpp

VehicleDBusDeployment.cpp

```



\### VehicleDBusProxy



Provides the D-Bus side of the client proxy.



```text

VehicleDBusProxy.hpp

VehicleDBusProxy.cpp

```



\### VehicleDBusStubAdapter



Connects the D-Bus side to the CommonAPI server stub.



```text

VehicleDBusStubAdapter.hpp

VehicleDBusStubAdapter.cpp

```



\---



\# 12. Server



The server implementation is located under:



```text

server/

├── main.cpp

├── VehicleService.cpp

└── VehicleService.hpp

```



\## VehicleService



The service class implements the generated CommonAPI interface.



Conceptually:



```text

VehicleStub

&#x20;    ↑

&#x20;    |

VehicleService

```



The service maintains the current vehicle speed.



Initial value:



```text

speed = 80

```



\---



\# 13. Server Operations



\## getSpeed()



The client requests the current speed.



Example:



```text

Client

&#x20;  |

&#x20;  | getSpeed()

&#x20;  v

VehicleService

&#x20;  |

&#x20;  | speed = 80

&#x20;  v

Client

```



\## setSpeed()



The client requests a new speed.



Example:



```text

Client

&#x20;  |

&#x20;  | setSpeed(100)

&#x20;  v

VehicleService

&#x20;  |

&#x20;  | speed = 100

&#x20;  |

&#x20;  +----> Attribute changed notification

&#x20;  |

&#x20;  +----> speedChanged event

```



The service updates its internal speed and sends the appropriate notifications.



\---



\# 14. Server main.cpp



The server creates the CommonAPI runtime:



```cpp

auto runtime = CommonAPI::Runtime::get();

```



Then creates the service:



```cpp

auto service = std::make\_shared<VehicleService>();

```



The service is registered with CommonAPI:



```cpp

runtime->registerService(

&#x20;   "com.example.vehicle",

&#x20;   "VehicleService",

&#x20;   service

);

```



After registration, the server keeps running and waits for client requests.



\---



\# 15. Client



The client is located under:



```text

client/

└── main.cpp

```



The client creates the CommonAPI runtime and builds a proxy for the service.



Conceptually:



```text

Vehicle Client

&#x20;     |

&#x20;     v

VehicleProxy

&#x20;     |

&#x20;     v

CommonAPI

&#x20;     |

&#x20;     v

D-Bus

```



The client waits until the service becomes available.



After the service is available, the client requests the current vehicle speed.



Example result:



```text

Vehicle service is available!

Current speed = 80

```



\---



\# 16. Why D-Bus?



D-Bus is an \*\*Inter-Process Communication (IPC)\*\* mechanism commonly used in Linux systems.



It allows different processes to communicate with each other.



Example:



```text

+-------------------+          +-------------------+

|  Vehicle Client   |          |  Vehicle Server   |

|                   |          |                   |

|     Process A     |          |     Process B     |

+---------+---------+          +---------+---------+

&#x20;         |                              |

&#x20;         +------------ D-Bus -----------+

```



In this project, CommonAPI-D-Bus provides the connection between the CommonAPI application and D-Bus.



D-Bus is intended to run in the Linux environment, so Docker was used to provide a Linux development environment on the Windows development machine.



\---



\# 17. Docker



The development machine was Windows.



Because the CommonAPI/D-Bus runtime and Linux D-Bus environment are required, Docker was used to provide a Linux development environment.



Docker was used for:



\* Linux build environment

\* CommonAPI runtime

\* CommonAPI D-Bus runtime

\* D-Bus development dependencies

\* ARM64 cross compiler

\* ARM64 sysroot

\* CMake build environment



\---



\# 18. Docker Image



The Docker environment was created using a \*\*previous Docker image provided by the project head/sir as the base environment\*\*.



Instead of building the complete environment from the beginning, the provided image was reused and the required CommonAPI/D-Bus development components were added using a Dockerfile.



The concept was:



```text

Provided Docker Image

&#x20;         |

&#x20;         v

&#x20;      Dockerfile

&#x20;         |

&#x20;         +------------------+

&#x20;         |                  |

&#x20;         v                  v

&#x20;    CommonAPI          D-Bus dependencies

&#x20;    Runtime            and development files

&#x20;         |                  |

&#x20;         +--------+---------+

&#x20;                  |

&#x20;                  v

&#x20;         CommonAPI/D-Bus

&#x20;         Development Image

```



The Dockerfile was used to prepare the required development environment.



During the work, the CommonAPI ARM64 development environment was available through:



```text

commonapi-arm64:1.3

```



The Raspberry Pi/Qt cross-build environment was also available through the previously provided image:



```text

ghcr.io/otrag1-lab/rpiqtcrossbuild:26.8

```



\---



\# 19. CommonAPI Runtime



The generated source files require the CommonAPI runtime libraries.



The CommonAPI runtime was built for the ARM64 target environment.



The runtime was installed under:



```text

/opt/commonapi-arm64

```



Important libraries include:



```text

libCommonAPI.so

libCommonAPI-DBus.so

```



CommonAPI headers are available under:



```text

/opt/commonapi-arm64/include/CommonAPI-3.2

```



\---



\# 20. D-Bus Development Files



D-Bus headers and libraries required for the ARM64 target were available through the Raspberry Pi sysroot.



Important paths include:



```text

/build/sysroot/usr/include/dbus-1.0

```



and:



```text

/build/sysroot/usr/lib/aarch64-linux-gnu

```



\---



\# 21. ARM64 Cross Compilation



The target platform for the application is Raspberry Pi ARM64.



The Docker environment contains an ARM64 cross compiler:



```text

/usr/bin/aarch64-linux-gnu-g++-14

```



The general flow is:



```text

Windows PC

&#x20;    |

&#x20;    v

Docker Linux Environment

&#x20;    |

&#x20;    v

ARM64 Cross Compiler

&#x20;    |

&#x20;    v

ARM64 Executable

&#x20;    |

&#x20;    v

Raspberry Pi

```



The Docker container itself may report:



```text

x86\_64

```



because Docker is running on the Windows PC architecture.



The important point is that the compiler used for the application is:



```text

aarch64-linux-gnu-g++

```



which generates ARM64 target binaries.



\---



\# 22. CMake



CMake is used as the project build system.



The CMake configuration contains the required:



\* CommonAPI include paths

\* CommonAPI libraries

\* D-Bus include paths

\* Generated source files

\* Server sources

\* Client sources

\* ARM64 cross compiler configuration



The project uses C++17.



\---



\# 23. Project Structure



The final project structure is:



```text

VehicleProject/

│

├── CMakeLists.txt

│

├── fidl/

│   └── Vehicle.fidl

│

├── server/

│   ├── main.cpp

│   ├── VehicleService.cpp

│   └── VehicleService.hpp

│

├── client/

│   └── main.cpp

│

└── src-gen/

&#x20;   └── v1/

&#x20;       └── com/

&#x20;           └── example/

&#x20;               └── vehicle/

&#x20;                   ├── Vehicle.hpp

&#x20;                   ├── VehicleProxy.hpp

&#x20;                   ├── VehicleProxyBase.hpp

&#x20;                   ├── VehicleStub.hpp

&#x20;                   ├── VehicleStubDefault.hpp

&#x20;                   │

&#x20;                   ├── VehicleDBusDeployment.hpp

&#x20;                   ├── VehicleDBusDeployment.cpp

&#x20;                   ├── VehicleDBusProxy.hpp

&#x20;                   ├── VehicleDBusProxy.cpp

&#x20;                   ├── VehicleDBusStubAdapter.hpp

&#x20;                   └── VehicleDBusStubAdapter.cpp

```



\---



\# 24. Complete Development Flow



The complete workflow followed in this project was:



```text

Step 1

Define the interface using Franca FIDL

&#x20;       |

&#x20;       v

Step 2

Generate CommonAPI Core files

&#x20;       |

&#x20;       v

Step 3

Generate CommonAPI D-Bus files

&#x20;       |

&#x20;       v

Step 4

Implement VehicleService

&#x20;       |

&#x20;       v

Step 5

Implement Vehicle Client

&#x20;       |

&#x20;       v

Step 6

Prepare Linux/D-Bus Docker environment

&#x20;       |

&#x20;       v

Step 7

Build/install CommonAPI runtime

&#x20;       |

&#x20;       v

Step 8

Configure ARM64 cross compiler

&#x20;       |

&#x20;       v

Step 9

Build the project using CMake

&#x20;       |

&#x20;       v

Step 10

Generate ARM64 executable

&#x20;       |

&#x20;       v

Step 11

Transfer application to Raspberry Pi

&#x20;       |

&#x20;       v

Step 12

Run CommonAPI/D-Bus service

&#x20;       |

&#x20;       v

Step 13

Run client

&#x20;       |

&#x20;       v

Client communicates with Vehicle Service

```



\---



\# 25. What Was Learned



This project provided practical understanding of:



\### Franca IDL



Defining service interfaces independently from implementation.



\### CommonAPI



Providing a middleware abstraction for service communication.



\### CommonAPI Core Generator



Generating CommonAPI proxy/stub/interface code.



\### CommonAPI D-Bus Generator



Generating the D-Bus communication binding.



\### Proxy



Used by the client to communicate with the service.



\### Stub



Used by the server to implement the service.



\### Stub Adapter



Connects the server-side CommonAPI implementation with D-Bus.



\### D-Bus



Linux IPC mechanism used for communication between processes.



\### Docker



Providing a controlled Linux development environment on Windows.



\### CMake



Managing the C++ project build.



\### Cross Compilation



Building ARM64 applications on the development PC for Raspberry Pi.



\---



\# 26. Automotive Application Concept



The same architecture can be extended to an automotive HMI.



For example:



```text

Vehicle Signals / CAN

&#x20;         |

&#x20;         v

&#x20;   Vehicle Service

&#x20;         |

&#x20;         v

&#x20;      CommonAPI

&#x20;         |

&#x20;         v

&#x20;        D-Bus

&#x20;         |

&#x20;         v

&#x20;       Qt HMI

```



Possible vehicle/HMI data could include:



```text

Speed

Temperature

A/C status

Volume

Time

Date

```



This project therefore provides a foundation for understanding how an automotive application can communicate between services and an HMI using middleware.



\---



\# 27. Summary



The project demonstrates the complete concept:



```text

Franca FIDL

&#x20;    |

&#x20;    v

CommonAPI Generator

&#x20;    |

&#x20;    +----------------------+

&#x20;    |                      |

&#x20;    v                      v

&#x20; Proxy                   Stub

&#x20;    |                      |

&#x20;    v                      v

D-Bus Proxy          D-Bus Stub Adapter

&#x20;    |                      |

&#x20;    +-------- D-Bus -------+

&#x20;             |

&#x20;             v

&#x20;      Linux IPC

```



The application interface is defined in Franca FIDL, CommonAPI source files are generated automatically, D-Bus binding files are generated separately, and the server/client applications use those generated interfaces.



Docker provides the Linux development environment, while the ARM64 cross compiler prepares the application for the Raspberry Pi target.



