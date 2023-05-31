cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJECT_NAME "QmlHttpRequest")
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION_TWEAK 0)
if (PROJECT_VERSION_TWEAK EQUAL 0)
    set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
else()
    set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK})
endif()
set(PROJECT_VERSION_TYPE "alpha")
set(PROJECT_LICENSE_TYPE "MIT")
set(PROJECT_CREATOR "Alireza")
set(PROJECT_LANGUAGES "CXX")
set(PROJECT_DESCRIPTION "A XmlHttpRequest like module for QML providing more HTTP functionality like timeout, upload and download files, etc")
set(PROJECT_HOMEPAGE_URL "https://github.com/alirezah95/QmlHttpRequest")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/src/config.hpp
)

set(QHR_ENABLE_TESTING
    ON
    CACHE BOOL "Enable unit testing. Note that Google test library must be
    installed on properly configrued on your sysytem"
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

find_package(QT NAMES Qt6 Qt5)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_qml_module(${PROJECT_NAME}
        URI "QmlHttpRequest"
        SHARED
        VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        SOURCES
            src/qmlhttprequest_global.hpp
            src/request.hpp src/request.cpp
            src/qmlhttprequest.hpp src/qmlhttprequest.cpp
            src/response.hpp src/response.cpp
        )

    target_compile_definitions(QmlHttpRequest PRIVATE QMLHTTPREQUEST_LIBRARY)
else()
    project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES ${PROJECT_LANGUAGES})

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Qml)

    add_library(${PROJECT_NAME} SHARED
        src/qmlhttprequest_global.hpp
        src/request.hpp src/request.cpp
        src/qmlhttprequest.hpp src/qmlhttprequest.cpp
        src/response.hpp src/response.cpp
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE QMLHTTPREQUEST_LIBRARY)

    target_link_libraries(${PROJECT_NAME} PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Qml
    )
endif()

if (QHR_ENABLE_TESTING)
    # Add tests subdirectory
    add_subdirectory("tests/")
endif()
