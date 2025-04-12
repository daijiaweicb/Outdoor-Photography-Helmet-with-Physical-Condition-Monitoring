project(QT_HELMET VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    motor_thread.cpp
    motor_thread.h
    ${CMAKE_SOURCE_DIR}/src/Stepmotor_setting.cpp
    ${CMAKE_SOURCE_DIR}/src/Mode.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QT_HELMET
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        motor_thread.h
        motor_thread.cpp
    )
else()
    add_executable(QT_HELMET
        ${PROJECT_SOURCES}
    )
endif()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GPIOD REQUIRED libgpiod)

target_include_directories(QT_HELMET PRIVATE
    ${GPIOD_INCLUDE_DIRS}
)

target_link_libraries(QT_HELMET
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS}
    ${GPIOD_LIBRARIES}
)

target_include_directories(QT_HELMET
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS QT_HELMET
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QT_HELMET)
endif()
