cmake_minimum_required(VERSION 3.21)
project(ServerGui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Graphs)
find_package(Threads REQUIRED)
qt_standard_project_setup(REQUIRES 6.5)

set(COMMON ${CMAKE_CURRENT_SOURCE_DIR}/../../common)

qt_add_executable(server_gui
    main.cpp Backend.hpp ServerWorker.hpp
    ${COMMON}/TCPSocket.cpp ${COMMON}/UDPSocket.cpp)

qt_add_qml_module(server_gui
    URI ServerGui
    VERSION 1.0
    QML_FILES Main.qml Gauge.qml
    RESOURCES
        images/Background.jpg
	images/facebook-app-symbol.png
	images/instagram.png
	images/linkedin.png)

target_include_directories(server_gui PRIVATE ${COMMON})
target_link_libraries(server_gui PRIVATE
    Qt6::Quick Qt6::QuickControls2 Qt6::Graphs Threads::Threads)