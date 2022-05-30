#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace hamming {
// prawdopodobnie przetworz na structa z statsami i argumentami konfiguracji algo

namespace stats {
    struct encode_stats {
    };

    struct decode_stats {
        uint64_t non_recoverable_errors {};
        uint64_t recovered_errors {};
    };
}

struct c_hamming_8_4 {
    // (x^3 + x + 1)(x^4 + x^2 + x + 1) plus bit calosci

    stats::encode_stats encode_stats_ {};
    stats::decode_stats decode_stats_ {};

    static auto encode_block(const uint8_t& msg) -> uint8_t
    {
        // 1 1 0 1 0 0 0
        // 0 1 1 0 1 0 0
        // 0 0 1 1 0 1 0
        // 0 0 0 1 1 0 1

        uint8_t b0 = (msg >> 0) & 0x1;
        uint8_t b1 = (msg >> 1) & 0x1;
        uint8_t b2 = (msg >> 2) & 0x1;
        uint8_t b3 = (msg >> 3) & 0x1;

        uint8_t const& x0 = b0;
        uint8_t const& x1 = b0 ^ b1;
        uint8_t const& x2 = b1 ^ b2;
        uint8_t const& x3 = b0 ^ b2 ^ b3;
        uint8_t const& x4 = b1 ^ b3;
        uint8_t const& x5 = b2;
        uint8_t const& x6 = b3;

        uint8_t const& parity = x0 ^ x1 ^ x2 ^ x3 ^ x4 ^ x5 ^ x6;

        // clang-format off
        uint8_t result = 
            (x0 << 0) |
            (x1 << 1) |
            (x2 << 2) |
            (x3 << 3) |
            (x4 << 4) |
            (x5 << 5) |
            (x6 << 6) |
            (parity << 7);
        // clang-format on

        // uint8_t const& parity = ((x0 ^ x1) ^ (x2 ^ x3)) ^ (x4 ^ x5 ^ x6);

        // // clang-format off
        // uint8_t result =
        //     (((x0 << 0) | (x1 << 1)) | ((x2 << 2) | (x3     << 3))) |
        //     (((x4 << 4) | (x5 << 5)) | ((x6 << 6) | (parity << 7)));
        // // clang-format on

        return result;
    }

    static auto hamming_syndrome(const uint8_t& msg) -> uint8_t
    {
        // 0 0 1 0 1 1 1
        // 0 1 0 1 1 1 0
        // 1 0 1 1 1 0 0

        uint8_t b0 = (msg >> 0) & 0x1;
        uint8_t b1 = (msg >> 1) & 0x1;
        uint8_t b2 = (msg >> 2) & 0x1;
        uint8_t b3 = (msg >> 3) & 0x1;
        uint8_t b4 = (msg >> 4) & 0x1;
        uint8_t b5 = (msg >> 5) & 0x1;
        uint8_t b6 = (msg >> 6) & 0x1;

        uint8_t const& x0 = (b2 ^ b4 ^ b5 ^ b6);
        uint8_t const& x1 = (b1 ^ b3 ^ b4 ^ b5);
        uint8_t const& x2 = (b0 ^ b2 ^ b3 ^ b4);

        // clang-format off
        uint8_t result = 
            (x0 << 0) |
            (x1 << 1) |
            (x2 << 2);
        // clang-format on

        return result;
    }

    static auto parity_syndrome(const uint8_t& msg) -> bool
    {
        uint8_t b0 = (msg >> 0) & 0x1;
        uint8_t b1 = (msg >> 1) & 0x1;
        uint8_t b2 = (msg >> 2) & 0x1;
        uint8_t b3 = (msg >> 3) & 0x1;
        uint8_t b4 = (msg >> 4) & 0x1;
        uint8_t b5 = (msg >> 5) & 0x1;
        uint8_t b6 = (msg >> 6) & 0x1;
        uint8_t b7 = (msg >> 7) & 0x1;

        uint8_t result = b0 ^ b1 ^ b2 ^ b3 ^ b4 ^ b5 ^ b6 ^ b7;

        return result != 0;
    }

    enum class error_type {
        no_error,
        recovered,
        non_recoverable
    };

    static auto repair_block(const uint8_t& msg) -> std::pair<uint8_t, error_type>
    {
        uint16_t hs = hamming_syndrome(msg);
        bool ps = parity_syndrome(msg);

        if (hs != 0 && ps == 0) {
            // nienaprawialny (podwojny)

            return { msg, error_type::non_recoverable };
        } else if (hs == 0 && ps != 0) {
            // blad parity bitu

            return { msg ^ 0x80, error_type::recovered };
        } else if (hs != 0 && ps != 0) {
            // pojedynczy bit z hamming_syndrome

            // uint8_t mask = (1 << (hs - 1));

            constexpr std::array<uint8_t, 8> lookup {
                0x00, 0x40, 0x02, 0x20, 0x01, 0x04, 0x08, 0x10
            };

            // constexpr std::array<uint8_t, 8> lookup {
            //     0x00, 0x01, 0x02, 0x08, 0x04, 0x40, 0x10, 0x20
            // };

            assert(hs < 8);
            uint8_t mask = lookup[hs];
            uint8_t res = msg ^ mask;
            return { res, error_type::recovered };

        } else {
            // brak bledu

            return { msg, error_type::no_error };
        }
    }

    static auto decode_block(const uint8_t& msg) -> uint8_t
    {
        // 1 1 0 1 0 0 0
        // 0 1 1 0 1 0 0
        // 0 0 1 1 0 1 0
        // 0 0 0 1 1 0 1

        uint8_t b0 = (msg >> 0) & 0x01;
        uint8_t b1 = (msg >> 1) & 0x01;
        // uint8_t b2 = (msg >> 2) & 0x01;
        // uint8_t b3 = (msg >> 3) & 0x01;
        // uint8_t b4 = (msg >> 4) & 0x01;
        uint8_t b5 = (msg >> 5) & 0x01;
        uint8_t b6 = (msg >> 6) & 0x01;

        uint8_t const& x0 = b0;
        uint8_t const& x1 = b0 ^ b1;
        uint8_t const& x2 = b5;
        uint8_t const& x3 = b6;

        // clang-format off
        uint8_t result = 
            (x0 << 0) |
            (x1 << 1) |
            (x2 << 2) |
            (x3 << 3);
        // clang-format on

        return result;
    }

    auto decode(std::vector<uint8_t> const& encoded) -> std::vector<uint8_t>
    {
        std::vector<uint8_t> message {};
        stats::decode_stats s {};

        message.resize(encoded.size() / 2);
        size_t size = message.size();

        for (size_t i {}; i < size; ++i) {
            uint8_t block {};
            {
                auto [encoded_block, error] = repair_block(encoded[2 * i]);
                if (error == error_type::non_recoverable) {
                    ++s.non_recoverable_errors;
                } else if (error == error_type::recovered) {
                    ++s.recovered_errors;
                }

                block |= decode_block(encoded_block);
            }
            {
                auto [encoded_block, error] = repair_block(encoded[2 * i + 1]);
                if (error == error_type::non_recoverable) {
                    ++s.non_recoverable_errors;
                } else if (error == error_type::recovered) {
                    ++s.recovered_errors;
                }

                block |= decode_block(encoded_block) << 4;
            }

            message[i] = block;
        }

        decode_stats_ = s;
        return message;
    }

    auto encode(std::vector<uint8_t> const& message) -> std::vector<uint8_t>
    {
        std::vector<uint8_t> result {};

        size_t size = message.size();
        result.resize(2 * size);

        for (size_t i {}; i < size; ++i) {
            result[2 * i + 0] = encode_block(message[i] & 0x0f);
            result[2 * i + 1] = encode_block((message[i] >> 4) & 0x0f);
        }

        return result;
    }
};

}