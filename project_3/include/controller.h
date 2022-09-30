#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller
{
public:
    virtual float update(float ref, float actual) = 0;
    virtual void brake() = 0;
};

#endif