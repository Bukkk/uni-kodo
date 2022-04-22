#include <cstdint>
#include <optional>
#include <vector>

namespace coding::natural {
namespace {

    /**
     * @brief minimalna ilosc bitow potrzeba do zapisu liczby
     *
     * @param num liczba
     * @return uint32_t len minimalna taka num >> len == 0
     */
    inline auto bit_length(uint64_t num) -> uint32_t
    {
        // prawdopodobnie i tak kompilator zamieni w jedna instrukcje
        uint32_t len {};
        while (num != 0) {
            num >>= 1;
            ++len;
        }

        return len;
    }
}

namespace elias_gamma {
    inline auto encode(uint64_t num) -> std::vector<bool>
    {
        std::vector<bool> encoded;

        auto len = bit_length(num);
        for (auto i = decltype(len) {}; i < len - 1; ++i) {
            encoded.push_back(false);
        }

        for (auto i = decltype(len) {}; i < len; ++i) {
            encoded.push_back(((num >> (len - i - 1)) & 0x1) != 0);
        }

        return encoded;
    }

    inline auto decode(
        std::vector<bool>::const_iterator bits,
        const std::vector<bool>::const_iterator end)

        -> std::optional<uint64_t>
    {
        uint64_t decoded {};

        uint32_t zeros {};
        while (true) {

            if (bits == end) {
                return {};
            }

            if (*bits) {
                break;
            }

            ++zeros;
            ++bits;
        }

        for (auto i = decltype(zeros) {}; i < zeros; ++i) {
            if (bits == end) {
                // tutaj moglby nawet zwracac blad bo jest wyraznie niedokonczony
                return {};
            }

            decoded |= *bits ? 0x1 : 0x0;
            decoded <<= 1;

            ++bits;
        }

        return { decoded };
    }
}

namespace elias_delta {
    inline auto encode(uint64_t num) -> std::vector<bool>
    {
    }

    inline auto decode(
        std::vector<bool>::const_iterator bits,
        const std::vector<bool>::const_iterator end)

        -> std::optional<uint64_t>
    {
    }
}

namespace elias_omega {
    inline auto encode(uint64_t num) -> std::vector<bool>
    {
    }

    inline auto decode(
        std::vector<bool>::const_iterator bits,
        const std::vector<bool>::const_iterator end)

        -> std::optional<uint64_t>
    {
    }
}

namespace fibonacci {
    inline auto encode(uint64_t num) -> std::vector<bool>
    {
    }

    inline auto decode(
        std::vector<bool>::const_iterator bits,
        const std::vector<bool>::const_iterator end)

        -> std::optional<uint64_t>
    {
    }
}
}