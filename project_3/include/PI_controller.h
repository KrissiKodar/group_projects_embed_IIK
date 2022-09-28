#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

class PI_Controller
{
public:
    PI_Controller();
    void init(double proportional_gain);
    double update(double ref, double actual);
private:
    double error;
    double Kp;
    double Ti;
    double integration_T;
    double sum_error;
};


#endif