cmake_minimum_required(VERSION 3.16)
project(Helmet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

include_directories(${PROJECT_SOURCE_DIR}/include ${GPIOD_INCLUDE_DIRS})

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB TEST_FILES "${PROJECT_SOURCE_DIR}/test/*.cpp")

add_executable(motor_mpu_test ${SOURCES} ${PROJECT_SOURCE_DIR}/test/motor_mpu_test.cpp)
target_link_libraries(motor_mpu_test PRIVATE i2c ${GPIOD_LIBRARIES})
target_compile_options(motor_mpu_test PRIVATE ${GPIOD_CFLAGS_OTHER})

install(TARGETS motor_mpu_test 
        RUNTIME DESTINATION bin)

add_subdirectory(QTViewer)
