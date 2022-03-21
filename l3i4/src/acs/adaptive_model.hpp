#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

static constexpr uint64_t PRECISION = 32;
static constexpr uint64_t WHOLE = uint64_t{1} << PRECISION;
static constexpr uint64_t HALF = WHOLE / 2;
static constexpr uint64_t QUARTER = WHOLE / 4;

class adaptive_model {
    static constexpr int MODEL_MAX_FREQUENCY = QUARTER - 1; // max total frequency for model
    static constexpr int MODEL_SIZE = 256; // 256 + 1 for EOF symbol

    std::vector<size_t> occurence_;
    std::vector<size_t> scale_from_;
    std::vector<size_t> scale_to_;

    // suma po occurence[i]
    size_t occurences_sum_;

    void calculate_scale()
    {
        size_t sum = 0;
        for (size_t sym = 0; sym < occurence_.size(); sym++) {
            scale_from_[sym] = sum;
            sum += occurence_[sym];
            scale_to_[sym] = sum; // end = begin + frequency
        }
    }

public:
    adaptive_model()
        : occurence_(MODEL_SIZE)
        , scale_from_(MODEL_SIZE)
        , scale_to_(MODEL_SIZE)
    {
        static_assert(MODEL_SIZE < MODEL_MAX_FREQUENCY, "za duza liczba symboli - nie mieszcza sie w maksymalnej dokladnosci");

        // zakladam ze wszystkie pojawily sie z raz aby bylo porowno i nie zero
        for (std::size_t i = 0; i < occurence_.size(); i++)
            occurence_[i] = 1;

        occurences_sum_ = MODEL_SIZE;

        calculate_scale();
    }

    auto size()
    {
        return occurence_.size();
    }

    auto occurences_sum()
    {
        return occurences_sum_;
    }

    auto scale_from(size_t symbol)
    {
        return scale_from_[symbol];
    }

    auto scale_to(size_t symbol)
    {
        return scale_to_[symbol];
    }

    void update(size_t symbol)
    {
        occurence_[symbol]++;
        occurences_sum_++;

        calculate_scale();
    }
};