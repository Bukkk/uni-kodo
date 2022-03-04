#pragma once

#include "chrono"
#include <cstdint>

namespace utils {

    template <typename time_unit>
    struct time_it {
        using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

        time_point _start;

        time_it()
        {
            _start = std::chrono::high_resolution_clock::now();
        }

        auto measure() const
            -> typename time_unit::rep
        {
            auto duration = std::chrono::high_resolution_clock::now() - _start;
            auto&& casted = std::chrono::duration_cast<time_unit>(duration);
            return casted.count();
        }
    };
}