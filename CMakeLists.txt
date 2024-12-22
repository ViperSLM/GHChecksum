cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME GHChecksum)
project(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(GHCHECKSUM_COMPILE_AS_SHAREDLIB "Compile ${PROJECT_NAME}Lib as a shared library, rather than a static library\n[ Useful for creating bindings for different programming languages (i.e Python) ]")

if(GHCHECKSUM_COMPILE_AS_SHAREDLIB)
    set(GHCHECKSUM_LIB SHARED)
    add_compile_definitions(GH_EXPORT_DLL)
else()
    set(GHCHECKSUM_LIB STATIC)
    add_compile_definitions(GH_STATIC_LIB)
endif()
add_library(${PROJECT_NAME}Lib ${GHCHECKSUM_LIB}
    src/lib/GHChecksum_API.h
    src/lib/CustomString.h src/lib/CustomString.cpp
    src/lib/ghchecksum.h src/lib/ghchecksum.cpp
)

add_executable(${PROJECT_NAME}
    src/app.h
    src/main.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}Lib)