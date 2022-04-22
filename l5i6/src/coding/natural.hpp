#include <array>
#include <cstdint>
#include <optional>
#include <stack>
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
        std::vector<bool>::const_iterator& bits,
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

        for (auto i = decltype(zeros) {}; i < zeros + 1; ++i) {
            if (bits == end) {
                // tutaj moglby nawet zwracac blad bo jest wyraznie niedokonczony
                return {};
            }

            decoded <<= 1;
            decoded |= *bits ? 0x1 : 0x0;

            ++bits;
        }

        return { decoded };
    }
}

namespace elias_delta {
    inline auto encode(uint64_t num) -> std::vector<bool>
    {
        std::vector<bool> encoded;

        auto num_len = bit_length(num);
        auto len_len = bit_length(num_len);

        for (auto i = decltype(len_len) {}; i < len_len - 1; ++i) {
            encoded.push_back(false);
        }

        for (auto i = decltype(len_len) {}; i < len_len; ++i) {
            encoded.push_back(((num_len >> (len_len - i - 1)) & 0x1) != 0);
        }

        for (auto i = decltype(num_len) {}; i < num_len - 1; ++i) {
            encoded.push_back(((num >> (num_len - i - 1)) & 0x1) != 0);
        }

        return encoded;
    }

    inline auto decode(
        std::vector<bool>::const_iterator& bits,
        const std::vector<bool>::const_iterator end)

        -> std::optional<uint64_t>
    {
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

        uint32_t len {};
        for (auto i = decltype(zeros) {}; i < zeros + 1; ++i) {
            if (bits == end) {
                // tutaj moglby nawet zwracac blad bo jest wyraznie niedokonczony
                return {};
            }

            len <<= 1;
            len |= *bits ? 0x1 : 0x0;

            ++bits;
        }

        uint64_t decoded = 1;

        for (auto i = decltype(len) {}; i < len - 1; ++i) {
            if (bits == end) {
                // tutaj moglby nawet zwracac blad bo jest wyraznie niedokonczony
                return {};
            }

            decoded <<= 1;
            decoded |= *bits ? 0x1 : 0x0;

            ++bits;
        }

        return decoded;
    }
}

namespace elias_omega {
    inline auto encode(uint64_t num) -> std::vector<bool>
    { // po prostu rozpisana rekursja aby nie allokowac rekursywnie
        std::vector<bool> encoded;
        std::stack<uint16_t> lengths;

        uint16_t len = bit_length(num);
        uint16_t x = len - 1;
        while (x != 1) {
            lengths.push(x);

            uint16_t next_x = bit_length(x) - 1;
            x = next_x;
        }

        while (!lengths.empty()) {
            auto y = lengths.top();
            auto y_len = bit_length(y);
            for (auto i = decltype(y_len) {}; i < y_len; ++i) {
                encoded.push_back(((y >> (y_len - i - 1)) & 0x1) != 0);
            }
            lengths.pop();
        }

        for (auto i = decltype(len) {}; i < len; ++i) {
            encoded.push_back(((num >> (len - i - 1)) & 0x1) != 0);
        }

        encoded.push_back(false);

        return encoded;
    }

    inline auto decode(
        std::vector<bool>::const_iterator& bits,
        const std::vector<bool>::const_iterator end)

        -> std::optional<uint64_t>
    {
        uint64_t val = 1;
        while (true) {
            if (bits == end) {
                return {};
            }

            if (!*bits) {
                break;
            }

            uint64_t new_val {};
            for (uint64_t i = 0; i < val + 1; ++i) {
                if (bits == end) {
                    // tutaj moglby nawet zwracac blad bo jest wyraznie niedokonczony
                    return {};
                }

                new_val <<= 1;
                new_val |= *bits ? 0x1 : 0x0;

                ++bits;
            }

            val = new_val;
        }

        return { val };
    }
}

namespace fibonacci {

    namespace {
        template <typename T>
        constexpr auto fib_len()
        {
            T fib1 = 1;
            T fib2 = 2;

            auto i = 2;
            while (true) {
                T fib = fib2 + fib1;

                if (fib < fib2) {
                    return i;
                }

                ++i;
                fib1 = fib2;
                fib2 = fib;
            }
        }

        template <typename T, uint16_t N>
        constexpr auto fib_lookup() -> std::array<T, N>
        {
            std::array<T, N> table {};
            table[0] = 1;
            table[1] = 2;

            for (auto i = 2; i < N; ++i) {
                table[i] = table[i - 1] + table[i - 2];
            }

            return table;
        }

        constexpr auto fibonacci_lookup_table
            = fib_lookup<uint64_t, fib_len<uint64_t>()>();
    }

    inline auto encode(uint64_t num) -> std::vector<bool>
    {   

    }

    inline auto decode(
        std::vector<bool>::const_iterator& bits,
        const std::vector<bool>::const_iterator end)

        -> std::optional<uint64_t>
    {
    }
}
}