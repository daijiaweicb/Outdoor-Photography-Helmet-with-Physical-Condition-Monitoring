# Outdoor Photography Helmet with Physical Condition Monitoring


# 1. Project Introduction 
<p align="left">
  <img src="https://github.com/user-attachments/assets/05bc4ca6-dca0-45ce-85ae-d13045f83054" alt="LOGO" width="100"/>
</p>

The project is a helmet photography platform that has real-time camera stabilization and fatigue detection. It is based on raspberry pi using c++ under Linux.  
The button of the Qt window could change the mode of the photography platform.
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

3. Take selfie video.

# 2. Project detail 
Hardware
1. Raspberry Pi 5 × 1
2. MPU 6050       × 1
3. MG90S motor    × 1
4. 2BYJ-48 motor  × 1
5. Raspberry Pi official camera V3 × 1
6. 7-inch screen  × 1

### Frame
![image](https://github.com/user-attachments/assets/0d391542-6c58-4638-bdd6-689b8dd0681f)
### Features
- Stabilized outdoor photography with helmet-mounted camera
- Real-time fatigue detection through eye movement
- Smart environment sensing


# 3. Installation
The required library 
1. libopencv-dev
2. libgpio
3. dlib
4. liblapack-dev libblas-dev
5. libcamera-dev
6. libqwt-qt5-dev



```bash
cmake .
make
sudo make install
```






# 4. Social media

| Follow me 🔔 |  
|:--------:|  
| [<img src="https://github.com/user-attachments/assets/aea9b545-b5d0-4563-9a56-6cd11e22de6e" width="30"/> TikTok](https://www.tiktok.com/@outdoor.photograp8?_t=ZN-8vV1aefj4Dv&_r=1) |





