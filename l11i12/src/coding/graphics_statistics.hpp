#pragma once

#include "moje_tga.hpp"
#include <cmath>
#include <cstdint>
#include <limits>

namespace coding::statistics {

// blad sredniokwadratowy
// mean squared error
inline auto mse(const std::vector<uint8_t>& orginal_data, const std::vector<uint8_t>& processed_data) -> double
{
    auto size = orginal_data.size();

    double result {};
    for (size_t i {}; i < size; ++i) {
        int64_t k = orginal_data[i];
        k -= processed_data[i];
        result += k * k;
    }
    result /= size;

    return result;
}

// stosunek sygnalu do szumu
// signal to noice ratio
inline auto snr(const std::vector<uint8_t>& orginal_data, double mse) -> double
{
    if (mse == 0) {
        return std::numeric_limits<double>::infinity();
    }

    auto size = orginal_data.size();
    double result {};
    for (size_t i {}; i < size; ++i) {
        auto k = orginal_data[i];
        result += k * k;
    }
    result /= size;
    
    result /= mse;

    return result;
}

inline auto to_decibels(double val) -> double
{
    return 10 * std::log10(val);
}

}