#ifndef EVENT_CALLBACK_H
#define EVENT_CALLBACK_H

class CallbackInterface
{
public:
    virtual void MPUCallback(float value) = 0;
    virtual ~CallbackInterface() = default;
};

#endif