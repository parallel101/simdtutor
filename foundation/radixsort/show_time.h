#pragma once


#include <chrono>
#include <fmt/format.h>


struct show_time
{
    const char *name;
    std::chrono::steady_clock::time_point t0;

    show_time(const char *name)
        : name(name), t0(std::chrono::steady_clock::now())
    {
    }

    ~show_time()
    {
        auto t1 = std::chrono::steady_clock::now();
        double sec = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count();
        fmt::println("{}\t: {}", name, sec);
    }
};
