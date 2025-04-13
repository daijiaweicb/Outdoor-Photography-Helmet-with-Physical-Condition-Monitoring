#ifndef MODE_H
#define MODE_H

enum class SystemMode {
    Normal,
    FatigueDetection,
    Temp,
};

extern SystemMode g_systemMode;

#endif