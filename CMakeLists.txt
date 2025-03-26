cmake_minimum_required(VERSION 3.10)
project(Helmet)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PROJECT_SOURCE_DIR}/include ${GPIOD_INCLUDE_DIRS})

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

file(GLOB TEST_FILES "${PROJECT_SOURCE_DIR}/test/*.cpp")

add_executable(motor_mpu_test ${SOURCES} ${PROJECT_SOURCE_DIR}/test/motor_mpu_test.cpp)
target_link_libraries(motor_mpu_test PRIVATE i2c ${GPIOD_LIBRARIES})
target_compile_options(motor_mpu_test PRIVATE ${GPIOD_CFLAGS_OTHER})

add_executable(bme_test ${SOURCES} ${PROJECT_SOURCE_DIR}/test/bme_test.cpp)
target_link_libraries(bme_test PRIVATE i2c ${GPIOD_LIBRARIES})
target_compile_options(bme_test PRIVATE ${GPIOD_CFLAGS_OTHER})

add_executable(mpu_bme_test ${SOURCES} ${PROJECT_SOURCE_DIR}/test/mpu_bme_test.cpp)
target_link_libraries(mpu_bme_test PRIVATE i2c ${GPIOD_LIBRARIES})
target_compile_options(mpu_bme_test PRIVATE ${GPIOD_CFLAGS_OTHER})

install(TARGETS motor_mpu_test bme_test
        RUNTIME DESTINATION bin)
