#ifndef CONSTANTS_H
#define CONSTANTS_H

// define your own namespace to hold constants
namespace constants
{
    constexpr double interval {20.0};
    constexpr double time_constant {100.0}; // ms
    constexpr double max_speed {140.0}; // of motor
    constexpr double control_rate {6000.0};//(max 16384 microsec)// (10.0 / (time_constant / 1000.0)) * 2.0;
    // ... other related constants
}
#endif