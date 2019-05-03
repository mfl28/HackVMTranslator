cmake_minimum_required(VERSION 3.12)

project(HackVMTranslator VERSION 1.0.0 LANGUAGES CXX)

# Adds an option to build the unit-tests (including the downloading and building of gtest)
option(BUILD_TESTS "Build the tests" OFF)

# Files
set(SOURCES src/CodeWriter.cpp
            src/HackVMTranslator.cpp
            src/CodeWriter.cpp 
            src/Parser.cpp 
)

set(HEADERS include/CodeWriter.h
            include/HackVMTranslator.h 
            include/Parser.h 
)

# Build
set(PROJECT_LIB_NAME ${PROJECT_NAME}Lib)

add_library(${PROJECT_LIB_NAME} OBJECT ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_LIB_NAME} PUBLIC include)

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIB_NAME})

# If gcc is used, it is necessary to link stdc++fs for std::filename support
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()

set_target_properties(${PROJECT_LIB_NAME} ${PROJECT_NAME} PROPERTIES 
    CXX_STANDARD 17 
    CXX_STANDARD_REQUIRED YES 
    CXX_EXTENSIONS NO
)

# Testing
if (BUILD_TESTS)
    ## Source: https://github.com/google/googletest/blob/master/googletest/README.md#incorporating-into-an-existing-cmake-project
    # Download and unpack googletest at configure time
    set(INSTALL_GTEST OFF CACHE BOOL "Do not install gtest" FORCE)
    set(INSTALL_GMOCK OFF CACHE BOOL "Do not install gmock" FORCE)

    # Gmock is not used in this project
    set(BUILD_GMOCK OFF CACHE BOOL "Do not build gmock" FORCE)      

    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)

    # Prevent GoogleTest from overriding our compiler/linker options
    # when building with Visual Studio
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This adds
    # the following targets: gtest, gtest_main, gmock
    # and gmock_main
    add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
            ${CMAKE_BINARY_DIR}/googletest-build)

    enable_testing()

    add_subdirectory(test)
endif ()