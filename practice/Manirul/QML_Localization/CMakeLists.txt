cmake_minimum_required(VERSION 3.16)

project(QML_Sig_Slot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.10 )

qt_add_executable(appQML_Sig_Slot main.cpp)

qt_add_qml_module(appQML_Sig_Slot
    URI QML_Sig_Slot
    VERSION 1.0
    QML_FILES
        Main.qml
)

set_source_files_properties(
    translations/qml_bn_BD.qm
    PROPERTIES QT_RESOURCE_ALIAS qml_bn_BD.qm
)

set_source_files_properties(
    translations/qml_hi_IN.qm
    PROPERTIES QT_RESOURCE_ALIAS qml_hi_IN.qm
)

qt_add_resources(appQML_Sig_Slot "translations"
    PREFIX "/qt/qml/QML_Sig_Slot/i18n"
    FILES
        translations/qml_bn_BD.qm
        translations/qml_hi_IN.qm
)

target_link_libraries(appQML_Sig_Slot
    PRIVATE Qt6::Quick
)

install(TARGETS appQML_Sig_Slot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)