#ifndef MODE_H
#define MODE_H

enum class SystemMode {
    Normal,
    FatigueDetection,
};

extern SystemMode g_systemMode;

#endif