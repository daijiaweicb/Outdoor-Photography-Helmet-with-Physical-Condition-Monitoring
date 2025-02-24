#include <iostream>
#include "MPU6050.h"
#include "motor.h"
#include "iic.h"
#include <vector>

using namespace std;

int main() {
    IIC iic(1);  // 确保 iic 生命周期覆盖所有线程
    ThreadMPU threadm(iic, 0.01f); 

    // 启动线程前注册回调
    MotorControl motorc;
    threadm.RegisterCallback(&motorc);

    threadm.start();

    // 主线程等待（避免 iic 提前销毁）
    std::cout << "Press ENTER to exit..." << std::endl;
    std::cin.get();

    threadm.stop();
    return 0;
}