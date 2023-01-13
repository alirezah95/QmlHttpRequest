cmake_minimum_required(VERSION 3.24)

if(QT_VERSION_MAJOR EQUAL 6)

else()
    project(qmlHttpRequest VERSION 0.1 LANGUAGES CXX)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Qml)

    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    add_library(${PROJECT_NAME} SHARED
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE QMLHTTPREQUEST_LIBRARY)

    target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Qml
    )
endif()
