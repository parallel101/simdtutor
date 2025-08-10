#pragma once


#if __unix__
#include <sched.h>
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif


inline void set_this_thread_affinity(int i)
{
#if __unix__
    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(i, &cpus);
    sched_setaffinity(gettid(), sizeof(cpus), &cpus);
#elif _WIN32
    DWORD_PTR cpus = DWORD_PTR(1) << i;
    SetThreadAffinityMask(GetCurrentThread(), cpus);
#endif
}
