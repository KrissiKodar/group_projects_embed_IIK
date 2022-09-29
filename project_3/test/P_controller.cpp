#include "P_controller.h"

P_Controller::P_Controller(double Kp)
{
    this->Kp = Kp;

}

double P_Controller::update(double ref, double actual)
{
    error = ref - actual;
    output = Kp * error;
    return output;
}