#include <iostream>
#include "MPU6050.h"
#include "motor.h"
#include "iic.h"
#include <vector>

using namespace std;

#include <chrono> // 需要添加此头文件

int main() {
    // 初始化硬件
    IIC iic(1);
    iic.iic_open();

    // 1. 先初始化电机
    StepperMotor motor;
    if(motor.start(0, 17, 27, 22, 5)) {
        cout << "motor init success" << endl;
    }

    // 2. 创建线程管理对象（但暂不启动）
    ThreadMPU threadm(iic, 0.01f); 

    // 3. 执行校准
    threadm.calibrate();
    std::cout << "Calibration done." << std::endl;

    // 4. 添加硬件稳定延时（关键修改）
    std::cout << "Waiting for sensor stabilization..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 500ms稳定时间

    // 5. 注册回调
    MotorControl motorc;
    threadm.RegisterCallback(&motorc);

    // 6. 最后启动线程（关键修改）
    std::cout << "Starting data acquisition..." << std::endl;
    threadm.start();

    // 主线程等待
    std::cout << "Press ENTER to exit..." << std::endl;
    std::cin.get();

    // 安全停止
    threadm.stop();
    return 0;
}

