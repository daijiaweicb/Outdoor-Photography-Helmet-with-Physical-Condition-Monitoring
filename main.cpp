#include <iostream>
#include "MPU6050.h"
#include "motor.h"
#include "iic.h"
#include <vector>

using namespace std;

int main() {
    // 初始化硬件
    IIC iic(1);
    iic.iic_open();

    // 创建线程管理对象（传入IIC引用和采样周期）
    ThreadMPU threadm(iic, 0.01f); 

    // 校准传感器（通过线程对象操作）
    threadm.calibrate();
    std::cout << "Calibration done." << std::endl;

    // 注册回调并启动
    MotorControl motorc;
    threadm.RegisterCallback(&motorc);
    threadm.start();

    // 主线程等待
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // 停止
    threadm.stop();
    return 0;
}