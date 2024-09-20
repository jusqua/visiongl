/** \file timer.cpp

    Implements functions of libtimer.so

    Libtimer is a library for measuring the performance of functions in runtime. The time count is in microseconds.

  */

#define _CRT_SECURE_NO_WARNINGS
#ifndef _DEMO_TIMER_HPP
#define _DEMO_TIMER_HPP

// fps, clock
#include <sys/time.h>

// malloc
#include <stdlib.h>
#include <stdio.h>

/** \brief Time elapsed.

    If called with no parameters, or a null parameter, returns the time elapsed, in microseconds, from the last call to TimerStart().

    If called with a non-zero parameter, works exaclty as TimerStart().

  */
inline long TimerElapsed(int start = 0) {
    static struct timeval *Tps = NULL;
    static struct timeval *Tpf = NULL;
    long retval;
    if (start || Tps == NULL) {
        Tps = (struct timeval *)malloc(sizeof(struct timeval));
        gettimeofday(Tps, 0);
        Tpf = (struct timeval *)malloc(sizeof(struct timeval));
        gettimeofday(Tpf, 0);
    } else {
        gettimeofday(Tpf, 0);
    }
    retval = (Tpf->tv_sec - Tps->tv_sec) * 1000000 + Tpf->tv_usec - Tps->tv_usec;
    return retval;
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

#endif  // _DEMO_TIMER_HPP
