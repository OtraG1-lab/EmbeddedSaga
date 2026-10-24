cmake_minimum_required(VERSION 3.16)

project(Qt_QML_Car_HMI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.10)

qt_add_executable(appQt_QML_Car_HMI
    main.cpp
)

qt_add_qml_module(appQt_QML_Car_HMI
    URI Qt_QML_Car_HMI
    VERSION 1.0
    RESOURCE_PREFIX "/"

    QML_FILES
        Main.qml
        Car_Home_View.qml
        Car_Main_View.qml
        Car_Menu_View.qml
        Car_Music_View.qml
        Car_Navigation_View.qml

    RESOURCES
        Images/Resources/Car_Default.png
        Images/Resources/Car_Selected.png
        Images/Resources/Navigation_Default.png
        Images/Resources/Navigation_Selected.png
        Images/Resources/Home_Default.png
        Images/Resources/Home_Selected.png
        Images/Resources/Music_Default.png
        Images/Resources/Music_Selected.png
        Images/Resources/Menu_Default.png
        Images/Resources/Menu_Selected.png

        Images/Resources/0_Android_Auto.png
        Images/Resources/0_Bluetooth.png
        Images/Resources/0_Calls.png
        Images/Resources/0_clock.png
        Images/Resources/0_Contacts.png
        Images/Resources/0_DRVM.png
        Images/Resources/0_Netflix.png
        Images/Resources/0_Radio.png
        Images/Resources/0_Settings.png
        Images/Resources/0_Spotify.png
        Images/Resources/0_Users.png
        Images/Resources/0_Volume.png
        Images/Resources/0_WiFi.png
        Images/Resources/0_YouTube.png
        Images/Resources/0_YT_Music.png

        # New Car_Home_View dashboard icons
        Images/Resources/ac.png
        Images/Resources/auto.png
        Images/Resources/fan.png
        Images/Resources/Ford_S-max.jpg
        Images/Resources/front_defrost.png
        Images/Resources/rear_defrost.png
        Images/Resources/recirculation.png
        Images/Resources/sunny.png
        Images/Resources/temperature.png

        Images/Resources/Srivalli_pushpa.mp4
)

set_target_properties(appQt_QML_Car_HMI PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appQt_QML_Car_HMI
    PRIVATE
        Qt6::Quick
        Qt6::Multimedia
)

include(GNUInstallDirs)

install(TARGETS appQt_QML_Car_HMI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
