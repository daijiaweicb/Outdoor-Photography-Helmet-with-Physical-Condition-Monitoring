cmake_minimum_required(VERSION 3.16)
project(Helmet LANGUAGES CXX)

add_compile_options(-Wno-psabi)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

set(LIBCAMERA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libcamera)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libcamera
    ${GPIOD_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS} 
)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM SOURCES "${PROJECT_SOURCE_DIR}/src/fatigue_detector.cpp")

file(GLOB TEST_FILES "${PROJECT_SOURCE_DIR}/test/*.cpp")

add_executable(motor_mpu_test
    ${SOURCES}
    ${PROJECT_SOURCE_DIR}/test/motor_mpu_test.cpp
)
target_link_libraries(motor_mpu_test PRIVATE i2c ${GPIOD_LIBRARIES})
target_compile_options(motor_mpu_test PRIVATE ${GPIOD_CFLAGS_OTHER})
install(TARGETS motor_mpu_test RUNTIME DESTINATION bin)

add_executable(stepmotor_test
    ${SOURCES}
    ${PROJECT_SOURCE_DIR}/test/stepmotor_test.cpp
)
target_link_libraries(stepmotor_test PRIVATE ${GPIOD_LIBRARIES})
target_compile_options(stepmotor_test PRIVATE ${GPIOD_CFLAGS_OTHER})
install(TARGETS stepmotor_test RUNTIME DESTINATION bin)

enable_testing()

add_executable(motor_mpu_unit_test
    ${SOURCES}
    ${PROJECT_SOURCE_DIR}/test/motor_mpu_unit_test.cpp
)
target_link_libraries(motor_mpu_unit_test
    PRIVATE
    gtest_main
    ${GPIOD_LIBRARIES}
)

target_compile_options(motor_mpu_unit_test PRIVATE ${GPIOD_CFLAGS_OTHER})
add_test(NAME MotorMPUTest COMMAND motor_mpu_unit_test)

add_subdirectory(QTViewer)
