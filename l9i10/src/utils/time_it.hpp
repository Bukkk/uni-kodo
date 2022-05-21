#pragma once

#include "chrono"
#include <cstdint>

namespace utils {

/**
 * @brief pomiar czasu w dowolnej podanej jednostce timeunit
 *
 * @tparam time_unit std::chrono::... milliseconds microseconds days...
 */
template <typename time_unit>
struct time_it {
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

    time_point _start {};

    /**
     * @brief zacznij odmierzac czas
     *
     * @return auto
     */
    auto set()
    {
        _start = timestamp();
    }

    /**
     * @brief czas ktory minal od rozpoczecia pomiatu
     *
     * @return time_unit::rep
     */
    auto measure() const
        -> typename time_unit::rep
    {
        auto duration = timestamp() - _start;
        auto&& casted = std::chrono::duration_cast<time_unit>(duration);
        return casted.count();
    }

    /**
     * @brief aktualny czas
     *
     * @return auto
     */
    auto timestamp() const
    {
        return std::chrono::high_resolution_clock::now();
    }
};
}