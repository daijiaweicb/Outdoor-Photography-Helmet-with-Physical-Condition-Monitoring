# Outdoor Photography Helmet with Physical Condition Monitoring

Missing the breathtaking view ahead because you're looking down at the road? This Outdoor Photography Helmet is desgined for you !!!
<p align="middle">
  <img src="https://github.com/user-attachments/assets/05bc4ca6-dca0-45ce-85ae-d13045f83054" alt="LOGO" width="100"/>
</p>


🏞️
**Key Features**

**- Stabilized outdoor photography with helmet-mounted camera**

  Allows you to focus on the safety while the camera still steadily records the exciting images in front of you.
  
**- Real-time fatigue detection through eye movement**

  Guard your every outdoor adventure.


# Contents
- [1. Project Introduction](#1-project-introduction)
- [2. Hardware Part](#2-hardware-part)
- [3. Software Part](#3-software-part)
- [4. Social Media](#4-social-media)
- [5. Authors](#5-authors)
- [6. Acknowledgements](#6-acknowledgements)

# 1. Project Introduction 
[![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi-red)](https://www.raspberrypi.com)
[![Language](https://img.shields.io/badge/Language-C++17-blue)](https://isocpp.org)
[![Build System](https://img.shields.io/badge/CMake-Build-blueviolet)](https://cmake.org)
[![Qt](https://img.shields.io/badge/Qt-Enabled-brightgreen)](https://www.qt.io/)
[![OpenCV](https://img.shields.io/badge/OpenCV-Enabled-lightgrey)](https://opencv.org)
[![libcamera](https://img.shields.io/badge/libcamera-Supported-yellowgreen)](https://www.raspberrypi.com/documentation/computers/camera.html)
[![dlib](https://img.shields.io/badge/dlib-Face%20Detection-orange)](http://dlib.net/)

This is a real-time embedded project based on **Raspberry Pi** using **C++**.
The project constructed a helmet photography platform that has real-time camera stabilization and fatigue detection. 

### Mode 1 Front view capture
<p align="left">
  <img src="https://github.com/user-attachments/assets/9de4fa92-fb1c-4261-8e1e-c0fe95a3aa8d" alt="LOGO" width="500"/>
</p>

1. Keep the camera at a certain elevation angle with horizontal level to record the view in front even when climbers are focusing on the road.  

2. Take videos of the view in front.

3. Provide data like the current temperature, angle and time.

### Mode 2 Selfie and condition monitoring
<p align="left">
  <img src="https://github.com/user-attachments/assets/bdd2dea1-08a3-4b30-9a46-625c4f827f6e" alt="LOGO" width="500"/>
</p>

1. Identify the Fatigue level of users by the camera (Eye blinking).

2. Take selfie videos.

The button of the Qt window could change the mode of the photography platform.

# 2. Hardware part 
## 2.1 Hardware List

| No. | Item                                           | Quantity |
|-----|------------------------------------------------|----------|
| 1   | Raspberry Pi 5                                 | 1        |
| 2   | MPU 6050                                        | 1        |
| 3   | MG90S motor                                     | 1        |
| 4   | 28BYJ-48 motor                                  | 1        |
| 5   | Raspberry Pi official camera V3 (wide version) | 1        |
| 6   | 7-inch screen                                   | 1        |


## 2.2 Hardware Connection

### Frame

<p align="left">
  <img src="https://github.com/user-attachments/assets/0d391542-6c58-4638-bdd6-689b8dd0681f" alt="LOGO" width="500"/>
</p>

**Overall structure**

<p align="left">
  <img src="https://github.com/user-attachments/assets/6b055433-48cf-44f1-93dd-9238f6513d05" alt="LOGO" width="500"/>
</p>

**Camera and motor unit**

<p align="left">
  <img src="https://github.com/user-attachments/assets/01c394d5-c90f-46a5-900a-441c4ec4d4fc" alt="LOGO" width="500"/>
</p>

**MPU6050 and Raspberry pi**
<p align="left">
  <img src="https://github.com/user-attachments/assets/838197bf-a61f-4a84-9bb8-b4b9b139b390" alt="LOGO" width="500"/>
</p>

# 3. Software part

## 3.1 Software implementation and its structure

### The motor part 

### The Fatigue detection
To do the Fatigue detection, the input image is first be fed into the Opencv DNN model to identify the face image. Then the shape_predictor 68 face landmarks finds the characteristic point of the select image of the DNN model. Using the characteristic point data, the EAR (Aspect ratio of the eye) is calculated. If the EAR ratio is continuously lower than the expected value, the user will be marked as fatigue and increase the fatigue ratio on the output (From 0 to 100%). If there is not fatigue detected, the fatigue ratio will automatically decrease.

Project file strcuture
```bash
.
├── CMakeLists.txt
├── LICENSE
├── QTViewer
│   ├── CMakeLists.txt
│   ├── images
│   ├── main.cpp
│   ├── mainwindow.cpp
│   ├── mainwindow.h
│   ├── mainwindow.ui
│   ├── mock_for_test
│   ├── motor_control_QT.cpp
│   ├── motor_control_QT.h
│   ├── motor_sensor_service.cpp
│   ├── motor_sensor_service.h
│   ├── motor_thread.cpp
│   ├── motor_thread.h
│   └── qt_tests
├── README.md
├── include
│   ├── IServoControl.h
│   ├── MG90S_setting.h
│   ├── MPU6050.h
│   ├── MPU_kalman.h
│   ├── Mode.h
│   ├── Stepmotor_setting.h
│   ├── fatigue_detector.h
│   ├── iic.h
│   ├── motor_control.h
│   └── pwm.h
├── libcamera
│   ├── LICENSE
│   ├── libcam2opencv.cpp
│   └── libcam2opencv.h
├── models
│   ├── deploy.prototxt
│   ├── res10_300x300_ssd_iter_140000.caffemodel
│   └── shape_predictor_68_face_landmarks.dat
├── src
│   ├── MPU6050.cpp
│   ├── MPU_kalman.cpp
│   ├── Mode.cpp
│   ├── Stepmotor_setting.cpp
│   ├── fatigue_detector.cpp
│   ├── iic.cpp
│   ├── motor_control.cpp
│   └── pwm.cpp
└── test
    ├── motor_mpu_test.cpp
    ├── motor_mpu_unit_test.cpp
    └── stepmotor_test.cpp
```
## 3.2 Unit Testing
We have implemented unit tests using GoogleTest and QtTest for key modules:

| Module              | Test File                   | Coverage Description                          | Type         |
|---------------------|-----------------------------|-----------------------------------------------|--------------|
| **Mode Control**    | `test_motorthreadqt.cpp`    | Mode transitions, signal emission             | Unit Test (QTest) |
| **Servo Control**   | `motor_mpu_unit_test.cpp`   | Roll-to-angle mapping, fatigue logic switching| Unit Test (GTest) |
| **Stepper Control** | `test_motorcontrolqt.cpp`   | Logic correctness of MPUCallback on servo     | Unit Test (GTest) |

How to Run Unit Tests
```bash
cd build
ctest --output-on-failure
```

## 3.3 Running Manual Test Programs
In addition to automated unit tests, we provide manual test programs to verify hardware behavior on a real Raspberry Pi. These are integration-level tools that directly interact with GPIO and I²C hardware. Make sure all devices are properly connected and interfaces are enabled.

1. stepmotor_test.cpp – Stepper Motor CLI Tool\
This test lets you control the 28BYJ-48 stepper motor directly from the command line.
```bash
cd build
./stepmotor_test <steps> <direction>
```
steps: Number of steps to move the motor (e.g., 200)\
direction: Movement direction — forward or backward

2. motor_mpu_test.cpp – MPU6050 + MG90S Servo Test
This tool initializes the MPU6050 sensor and the MG90S servo, and runs a loop that:\
Reads real-time roll angle from MPU6050\
Adjusts servo angle based on the reading
```bash
cd build
./motor_mpu_test
```

## 3.4 Required library and How to install

Make sure you you have **install the required library** and **enable the iic function** on Raspberry Pi before cmake the program.
The required library 

1. libopencv-dev
2. libgpio
3. dlib
4. libcamera-dev
5. libqwt-qt5-dev

How to install all required packages
```bash
sudo apt update && sudo apt install -y \
  libopencv-dev \
  libgpiod-dev \
  libdlib-dev \
  libcamera-dev \
  libqwt-qt5-dev
```

How to use
```bash
cd Outdoor-Photography-Helmet-with-Physical-Condition-Monitoring
mkdir build
cd build
cmake ..
make
cd QTViewer
./QT_HELMET
```

In the project, the (shape_predictor_68_face_landmarks.dat, deploy.prototxt, res10_300x300_ssd_iter_140000.caffemodel) models are needed for the fatigue detection part. They are in the model folder of the project. 


## 3.5 The QT of the project

The helmet could change between the modes using the **Change the mode** button. The record button is on the right side and used to capture the image in both modes (The fatigue dectection on the image will not be recorded).

**Normal mode**

<p align="left">
  <img src="https://github.com/user-attachments/assets/999c964b-fed3-49a7-be2f-27c0e4834d02" alt="LOGO" width="500"/>
</p>

**Selfie and condition monitoring**

<p align="left">
  <img src="https://github.com/user-attachments/assets/efe7d86b-fcfd-42ca-9bc0-c9bc520bc328" alt="LOGO" width="500"/>
</p>




# 4. Social media

| Follow me 🔔 |  
|:--------:|  
| <img src="https://github.com/user-attachments/assets/aea9b545-b5d0-4563-9a56-6cd11e22de6e" width="30"/> |
|[TikTok Link](https://www.tiktok.com/@outdoor.photograp8?_t=ZN-8vV1aefj4Dv&_r=1)|

# 5. Authors
| Member       | Contribution                               |
|--------------|--------------------------------------------|
Yixuan Zha     |  Camera stabilization, Seneor and QT part
Boang Tian     |  Sensor and Camera Opencv part
Chang Sun      |  Motor and Soldering part
Qiming Cao     |  Sensor realization and Soldering part
Yihang Hou     |  Camera and Social media part


# 6. Acknowledgements
Special thanks to the ENG5220 course instructors for the guidance and support throughout the project : Bernd Porr, Chongfeng Wei, Tom O'Hara and Giulia Lafratta. Your support helped us improve our project and turn a simple idea into a working reality.


