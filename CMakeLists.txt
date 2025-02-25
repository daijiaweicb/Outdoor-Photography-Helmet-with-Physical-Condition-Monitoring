cmake_minimum_required(VERSION 3.10)
project(Helmet)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include ${GPIOD_INCLUDE_DIRS})

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")

add_executable(motor_mpu_test ${SOURCES} ${TEST_SOURCES})

target_link_libraries(motor_mpu_test PRIVATE i2c ${GPIOD_LIBRARIES})

target_compile_options(motor_mpu_test PRIVATE ${GPIOD_CFLAGS_OTHER})
