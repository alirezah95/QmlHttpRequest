find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Qml Core Network Test)

#if (WIN32)
#    if (NOT EXISTS ${CMAKE_SOURCE_DIR}/tests/googletest/.git)
#        find_package(Git)
#        if (GIT_FOUND)
#            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init
#                --recursive
#            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests/googletest/
#            RESULT_VARIABLE GOOGLE_TEST_OK)
#        if(NOT GOOGLE_TEST_OK EQUAL "0")
#            message(FATAL_ERROR "Could not update submodule googletest, aborting")
#        endif()
#        endif()
#    endif()

#    # Building googletest as object library
#    set(gtest_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest)
#    set(gmock_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googlemock)
#    add_library(googletest OBJECT
#        ${gtest_SOURCE_DIR}/src/gtest-all.cc
#        ${gmock_SOURCE_DIR}/src/gmock-all.cc
#    )
#    target_include_directories(googletest PRIVATE
#        ${gtest_SOURCE_DIR}
#        ${gtest_SOURCE_DIR}/include
#        ${gmock_SOURCE_DIR}
#        ${gmock_SOURCE_DIR}/include
#    )
#endif()

enable_testing()

function(add_new_gtest testname)
    if (ARGC GREATER 1)
        add_executable(${testname}
            ${ARGN}
        )
        add_test(NAME "test_${testname}" COMMAND ${testname})

        target_link_libraries(${testname} PRIVATE
            Qt${QT_VERSION_MAJOR}::Qml
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Test
            Qt${QT_VERSION_MAJOR}::Network
            gtest
            gmock
            qmlHttpRequest
        )
        target_include_directories(${testname} PRIVATE ../src)
    else()
        message(FATAL_ERROR "add_test_executable needs a test name and at "
            "least one source")
    endif()
endfunction()

# Set a varibable holding mocks headers, to easily add them in different tests
set(MOCKS_HEADERS
)

set(TEST_FILES
    tst_request.cpp
    tst_qmlhttprequest.cpp
)

foreach(TEST_FILE IN LISTS TEST_FILES)
    string(REPLACE ".cpp" "" TEST_NAME ${TEST_FILE})
    string(REPLACE "tst" "test" TEST_NAME ${TEST_NAME})
    add_new_gtest(${TEST_NAME}
        ${TEST_FILE}
    )
endforeach()


