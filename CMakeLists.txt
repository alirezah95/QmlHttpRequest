cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_qml_module(QmlHttpRequest
        URI "QmlHttpRequest"
        OUTPUT_DIRECTORY
            ${CMAKE_BINARY_DIR}/QmlHttpRequest
        VERSION 1.3
        SOURCES
            src/qmlhttprequest_global.hpp
            src/request.hpp src/request.cpp
            src/qmlhttprequest.hpp src/qmlhttprequest.cpp
            src/response.hpp src/response.cpp
        )

    # This is required on Windows and MSVC
    target_link_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/QmlHttpRequest
    )

    # Next line is required for Qml Preview on Linux to be able to load the module
    target_link_libraries(${PROJECT_NAME} PRIVATE
        QmlHttpRequest
    )
else()
    project(QmlHttpRequest VERSION 1.3 LANGUAGES CXX)

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Qml)

    add_library(${PROJECT_NAME} SHARED
        src/qmlhttprequest_global.hpp
        src/request.hpp src/request.cpp
        src/qmlhttprequest.hpp src/qmlhttprequest.cpp
        src/response.hpp src/response.cpp
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE QMLHTTPREQUEST_LIBRARY)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Qml
    )
endif()

# Add tests subdirectory
add_subdirectory("tests/")
