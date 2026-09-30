cmake_minimum_required(VERSION 3.16)

project(Qt_QML_StackView VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appQt_QML_StackView
    main.cpp
)

qt_add_qml_module(appQt_QML_StackView
    URI Qt_QML_StackView
    QML_FILES
        Main.qml
        Page_1.qml
        Page_2.qml
        Page_3.qml
)

set_target_properties(appQt_QML_StackView PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appQt_QML_StackView
    PRIVATE Qt6::Quick
)

install(TARGETS appQt_QML_StackView
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)