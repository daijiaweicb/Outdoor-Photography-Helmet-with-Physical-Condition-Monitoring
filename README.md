# Outdoor Photography Helmet with Physical Condition Monitoring
<p align="middle">
  <img src="https://github.com/user-attachments/assets/05bc4ca6-dca0-45ce-85ae-d13045f83054" alt="LOGO" width="100"/>
</p>

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

Key Features
- Stabilized outdoor photography with helmet-mounted camera
- Real-time fatigue detection through eye movement


### Mode 1 Front view capture
<p align="left">
  <img src="https://github.com/user-attachments/assets/9de4fa92-fb1c-4261-8e1e-c0fe95a3aa8d" alt="LOGO" width="500"/>
</p>

1. Keep the camera at a certain elevation angle with horizontal level to record the view in front even when climbers are focusing on the road.  

2. Take videos of the view.

### Mode 2 Selfie and condition monitoring
<p align="left">
  <img src="https://github.com/user-attachments/assets/bdd2dea1-08a3-4b30-9a46-625c4f827f6e" alt="LOGO" width="500"/>
</p>

1. Identify the Fatigue level of users by the camera (Eye blinking).

2. Combine pervious data with the current temperature, humidity and give advice to user.

3. Take selfie videos.

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

**Camera and motor unit**

<p align="left">
  <img src="https://github.com/user-attachments/assets/01c394d5-c90f-46a5-900a-441c4ec4d4fc" alt="LOGO" width="500"/>
</p>

**MPU6050 and Raspberry pi**
<p align="left">
  <img src="https://github.com/user-attachments/assets/838197bf-a61f-4a84-9bb8-b4b9b139b390" alt="LOGO" width="500"/>
</p>

# 3. Software part
Make sure you you have **install the required library** and **enable the iic function** on Raspberry Pi before cmake the program.
The required library 

1. libopencv-dev
2. libgpio
3. dlib
4. libcamera-dev
5. libqwt-qt5-dev

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
│   ├── motor_control_QT.cpp
│   ├── motor_control_QT.h
│   ├── motor_sensor_service.cpp
│   ├── motor_sensor_service.h
│   ├── motor_thread.cpp
│   └── motor_thread.h
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


