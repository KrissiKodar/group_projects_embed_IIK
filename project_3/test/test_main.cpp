#include <avr/delay.h>
#include <unity.h>
#include "PI_controller.h"

void test_simple_integration(void)
{
    // 1 Setup
    PI_Controller pi(1.0, 1.0, 1.0,0.99);
    double ref = 10.0;
    double actual = 0;
    double control = 0;
    // 2 - 3 Execute and validate
    for (int i = 0; i < 3; i++)
    {
        control = pi.update(ref, actual);
        _delay_ms(100);
        actual = 0.01*control;
    }
    TEST_ASSERT_EQUAL_FLOAT(0.39208, actual);
    // Cleanup
}

void test_anti_windup(void)

{
    // 1 Setup
    PI_Controller pi(1.0, 1.0, 1.0, 0.99);
    double ref = 0.25;
    double actual = 0;
    double control = 0;
    // 2 - 3 Execute and validate
    for (int i = 0; i < 3; i++)
    {
        control = pi.update(ref, actual);
        _delay_ms(100);
        actual = 2.0*control;
    }
    TEST_ASSERT_EQUAL_FLOAT(3.0, actual);
    // Cleanup
}

int main()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    _delay_ms(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!

    RUN_TEST(test_simple_integration);
    RUN_TEST(test_anti_windup);
    // Add more unit tests here

    UNITY_END(); // stop unit testing
}