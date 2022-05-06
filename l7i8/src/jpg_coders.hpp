#pragma once

#include "moje_tga.hpp"

#include <cstdint>
#include <tuple>


inline auto predictor_1(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
{
    uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    // uint8_t n = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    // uint8_t nw = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;

    return w;
}

inline auto predictor_2(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
{
    // uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t n = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    // uint8_t nw = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;

    return n;
}

inline auto predictor_3(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
{
    // uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    // uint8_t n = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t nw = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;

    return nw;
}

inline auto predictor_4(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
{
    uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t n = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t nw = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;

    return n + w - nw;
}

inline auto predictor_5(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
{
    uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t n = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t nw = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;

    return n + (w - nw) / 2;
}

inline auto predictor_6(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
{
    uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t n = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t nw = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;

    return w + (n - nw) / 2;
}

inline auto predictor_7(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
{
    uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t n = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    // uint8_t nw = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;

    return (n + w) / 2;
}

inline auto predictor_new(tga::accessor_MONO const& acc, uint32_t x, uint32_t y) -> uint8_t
{
    uint8_t w = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t n = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;
    uint8_t nw = acc.check_range(x - 1, y) ? acc[acc.nth(x - 1, y)] : 0;

    if (nw >= std::max(w, n)) {
        return std::max(w, n);
    } else if (nw <= std::min(w, n)) {
        return std::min(w, n);
    }

    return w + n - nw;
}