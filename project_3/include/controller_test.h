#ifndef CONTROLLER_TEST_H
#define CONTROLLER_TEST_H

class Controller_test
{
public:
    virtual float update(float ref, float actual) = 0;
    virtual void brake() = 0;
};

#endif