#pragma once

#include "moje_tga.hpp"

#include <cstddef>
#include <cstdint>
#include <tuple>

namespace jpg_predictors {
struct predictor_1 {
    static auto predict(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
    {
        uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
        // uint8_t n = acc.check_range(x, y + 1) ? acc[acc.nth(x, y + 1)] : 0;
        // uint8_t nw = acc.check_range(x - 1, y + 1) ? acc[acc.nth(x - 1, y + 1)] : 0;

        return w;
    }
};

struct predictor_2 {
    static auto predict(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
    {
        // uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
        uint8_t n = acc.check_range(x, y + 1) ? acc[acc.nth(x, y + 1)] : 0;
        // uint8_t nw = acc.check_range(x - 1, y + 1) ? acc[acc.nth(x - 1, y + 1)] : 0;

        return n;
    }
};

struct predictor_3 {
    static auto predict(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
    {
        // uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
        // uint8_t n = acc.check_range(x, y + 1) ? acc[acc.nth(x, y + 1)] : 0;
        uint8_t nw = acc.check_range(x - 1, y + 1) ? acc[acc.nth(x - 1, y + 1)] : 0;

        return nw;
    }
};

struct predictor_4 {
    static auto predict(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
    {
        uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
        uint8_t n = acc.check_range(x, y + 1) ? acc[acc.nth(x, y + 1)] : 0;
        uint8_t nw = acc.check_range(x - 1, y + 1) ? acc[acc.nth(x - 1, y + 1)] : 0;

        return n + w - nw;
    }
};

struct predictor_5 {
    static auto predict(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
    {
        uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
        uint8_t n = acc.check_range(x, y + 1) ? acc[acc.nth(x, y + 1)] : 0;
        uint8_t nw = acc.check_range(x - 1, y + 1) ? acc[acc.nth(x - 1, y + 1)] : 0;

        return n + (w - nw) / 2;
    }
};

struct predictor_6 {
    static auto predict(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
    {
        uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
        uint8_t n = acc.check_range(x, y + 1) ? acc[acc.nth(x, y + 1)] : 0;
        uint8_t nw = acc.check_range(x - 1, y + 1) ? acc[acc.nth(x - 1, y + 1)] : 0;

        return w + (n - nw) / 2;
    }
};

struct predictor_7 {
    static auto predict(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
    {
        uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
        uint8_t n = acc.check_range(x, y + 1) ? acc[acc.nth(x, y + 1)] : 0;
        // uint8_t nw = acc.check_range(x - 1, y + 1) ? acc[acc.nth(x - 1, y + 1)] : 0;

        return (n + w) / 2;
    }
};

struct predictor_new {
    static auto predict(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
    {
        uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
        uint8_t n = acc.check_range(x, y + 1) ? acc[acc.nth(x, y + 1)] : 0;
        uint8_t nw = acc.check_range(x - 1, y + 1) ? acc[acc.nth(x - 1, y + 1)] : 0;

        if (nw >= std::max(w, n)) {
            return std::max(w, n);
        } else if (nw <= std::min(w, n)) {
            return std::min(w, n);
        }

        return w + n - nw;
    }
};

template <typename Predictor>
auto encode(tga::accessor_MONO const& acc) -> std::vector<uint8_t>
{
    std::vector<uint8_t> vals;
    vals.resize(acc.size());

    for (size_t y {}; y < acc._height; ++y) {
        for (size_t x {}; x < acc._width; ++x) {
            auto nth = acc.nth(x, y);
            uint8_t predicted = Predictor::predict(acc, x, y);
            uint8_t real = acc[nth];

            uint8_t diff = real - predicted;
            vals[nth] = diff;
        }
    }

    return vals;
}
}