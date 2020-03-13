# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.5)
set (CMAKE_CXX_STANDARD 11)

project(servo_ctrl)

set(EXECUTABLE_NAME ${PROJECT_NAME})


# find_package(PkgConfig REQUIRED)

#
# --- Library dependencies ---

# find_path(XXX_INCLUDE_DIR
#     xxx.h
# )
# find_library(XXX_LIBRARY 
#     xxx
# )

## add sub projects
# add_subdirectory( src )

## include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

## link_directories
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/)

## files
file( GLOB SRCS src/*.c src/*.cpp src/*.cc)  # a variable called SRCS with all files whose path match "*.c *.cpp..."

## binary files
add_executable( ${PROJECT_NAME} ${SRCS} )


## link binaries
# target_link_libraries(${EXECUTABLE_NAME}
#     pthread
# )

set(INSTALL_TARGETS
    ${EXECUTABLE_NAME})

## install binaries
install(TARGETS ${INSTALL_TARGETS}
    RUNTIME DESTINATION bin
)
