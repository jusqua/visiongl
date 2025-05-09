/** \file timer.cpp

    Implements functions of libtimer.so

    Libtimer is a library for measuring the performance of functions in runtime. The time count is in microseconds.

  */

#ifndef VISIONGL_DEMO_TIMER_HPP
#define VISIONGL_DEMO_TIMER_HPP

// Modern C++ timing utilities
#include <chrono>

// malloc
#include <stdlib.h>
#include <stdio.h>

/** \brief Time elapsed.

    If called with no parameters, or a null parameter, returns the time elapsed, in microseconds, from the last call to TimerStart().

    If called with a non-zero parameter, works exaclty as TimerStart().

  */
inline long TimerElapsed(int start = 0) {
    using namespace std::chrono;
    static time_point<high_resolution_clock> Tps;
    static time_point<high_resolution_clock> Tpf;
    static bool initialized = false;

    if (start || !initialized) {
        Tps = high_resolution_clock::now();
        Tpf = high_resolution_clock::now();
        initialized = true;
    } else {
        Tpf = high_resolution_clock::now();
    }

    return duration_cast<microseconds>(Tpf - Tps).count();
}

/** \brief Timer start.

    Starts counting time in microseconds.
  */
inline void TimerStart() {
    TimerElapsed(1);
}

inline char *getTimeElapshed() {
    char *ret = (char *)malloc(sizeof(char) * 255);
    sprintf(ret, "%ld us", TimerElapsed());
    return ret;
}

inline char *getTimeElapsedInSeconds() {
    char *ret = (char *)malloc(sizeof(char) * 255);
    sprintf(ret, "%.6f s", TimerElapsed() / 1000000.0f);
    return ret;
}

#endif  // VISIONGL_DEMO_TIMER_HPP
