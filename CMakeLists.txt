cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME GHChecksum)
project(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(GHCHECKSUM_COMPILE_AS_SHAREDLIB "Compile ${PROJECT_NAME}Lib as a shared library, rather than a static library\n[ Useful for creating bindings for different programming languages (i.e Python) ]")
option(GHCHECKSUM_COMPILE_TESTS "Compile test programs")

# Determine if build as static or shared/dynamic
if(GHCHECKSUM_COMPILE_AS_SHAREDLIB)
    set(GHCHECKSUM_LIB SHARED)
if(WIN32)
    add_compile_definitions(GH_EXPORT_DLL)
endif()
else()
    set(GHCHECKSUM_LIB STATIC)
    add_compile_definitions(GH_STATIC_LIB)
endif()

# Library
add_library(${PROJECT_NAME}Lib ${GHCHECKSUM_LIB}
    src/lib/GHChecksum_API.h
    src/lib/inttypes.h
    src/lib/CustomString.h src/lib/CustomString.cpp
    src/lib/ghchecksum.h src/lib/ghchecksum.cpp
)

# Main program
add_executable(${PROJECT_NAME}
    src/app.h
    src/main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Lib)

# Test programs
if(GHCHECKSUM_COMPILE_TESTS)
set(TEST_QBKEY qbkey)
add_executable(${TEST_QBKEY}
    src/tests/qbkey.cpp
)
target_include_directories(${TEST_QBKEY} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)
target_link_libraries(${TEST_QBKEY} PRIVATE ${PROJECT_NAME}Lib)

set(TEST_ROQ roqtest)
add_executable(${TEST_ROQ}
    src/tests/roqtable.cpp
)
target_include_directories(${TEST_ROQ} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)
target_link_libraries(${TEST_ROQ} PRIVATE ${PROJECT_NAME}Lib)
endif()
