#pragma once

#include <vector>
#include <cstdint>

namespace hamming {
// prawdopodobnie przetworz na structa z statsami i argumentami konfiguracji algo
inline auto decode(std::vector<uint8_t> const& encoded) -> std::vector<uint8_t>
{
    return {};
}

inline auto encode(std::vector<uint8_t> const& message) -> std::vector<uint8_t>
{
    return {};
}

}