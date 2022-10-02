#ifndef CONSTANTS_H
#define CONSTANTS_H

// define your own namespace to hold constants
namespace constants
{
    constexpr float interval {20.0};
    constexpr float time_constant {100.0}; // ms
    constexpr float max_speed {140.0}; // of motor
    constexpr float control_rate {6000.0};//(max 16384 microsec)// (10.0 / (time_constant / 1000.0)) * 2.0;
    constexpr float integration_T {0.006};
    constexpr float max_output {0.99};
    // ... other related constants
}
#endif