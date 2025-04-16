project(QT_HELMET VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_NO_KEYWORDS ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)
find_package(dlib REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)
pkg_check_modules(GPIOD REQUIRED IMPORTED_TARGET libgpiod)

set(LIBCAMERA_HEADER_DIR ${CMAKE_SOURCE_DIR}/libcamera)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    motor_thread.cpp
    motor_thread.h
    motor_sensor_service.cpp
    motor_sensor_service.h
    motor_control_QT.cpp
    motor_control_QT.h
    threadpool.cpp
    threadpool.h
    ${CMAKE_SOURCE_DIR}/src/Stepmotor_setting.cpp
    ${CMAKE_SOURCE_DIR}/src/Mode.cpp
    ${CMAKE_SOURCE_DIR}/src/MPU6050.cpp
    ${CMAKE_SOURCE_DIR}/src/pwm.cpp
    ${CMAKE_SOURCE_DIR}/src/iic.cpp
    ${CMAKE_SOURCE_DIR}/src/MPU_kalman.cpp
    ${CMAKE_SOURCE_DIR}/src/fatigue_detector.cpp
    ${LIBCAMERA_HEADER_DIR}/libcam2opencv.cpp
)

add_executable(QT_HELMET ${PROJECT_SOURCES})

target_include_directories(QT_HELMET PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LIBCAMERA_HEADER_DIR}            
    ${LIBCAMERA_INCLUDE_DIRS} 
    ${GPIOD_INCLUDE_DIRS}
)

target_link_libraries(QT_HELMET
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS}
    dlib::dlib
    PkgConfig::LIBCAMERA
    PkgConfig::GPIOD
)

install(TARGETS QT_HELMET
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
