#ifndef CONTROLLER_TEST_H
#define CONTROLLER_TEST_H

class Controller_test
{
public:
    Controller_test();
    virtual double update(double ref, double actual) = 0;
};

#endif