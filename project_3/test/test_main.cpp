#include <avr/delay.h>
#include <unity.h>

#include "PI_controller.h"

void test_integration(void)
{
    // 1 Setup
    /* Fifo f;
    f.put(1);
    f.put(2);
    f.get();
    f.put(3);

    // 2-3 Execute and validate
    TEST_ASSERT_EQUAL(2, f.get());
    TEST_ASSERT_EQUAL(3, f.get()); */

    // 4 Cleanup
}

void test_anti_windup(void)

{
    // 1 Setup
    /* Fifo f;
    f.put(1);
    f.put(2);
    f.get();
    f.get();

    // 2-3 Execute and validate
    TEST_ASSERT_TRUE(f.is_empty()); */

    // 4 Cleanup
}

int main()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    _delay_ms(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!

    RUN_TEST(test_integration);
    RUN_TEST(test_anti_windup);
    // Add more unit tests here

    UNITY_END(); // stop unit testing
}