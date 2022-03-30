#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace algo {

namespace {
    inline constexpr uint64_t BITS_PRECISION = 32;
    inline constexpr uint64_t WHOLE_INTERVAL = uint64_t { 1 } << BITS_PRECISION;
    inline constexpr uint64_t HALF_INTERVAL = WHOLE_INTERVAL / 2;
    inline constexpr uint64_t QUARTER_INTERVAL = WHOLE_INTERVAL / 4;
}

class adaptive_model {
    static constexpr uint64_t MAX_OCCURENCE = QUARTER_INTERVAL - 1;
    static constexpr uint64_t SYMBOL_AMOUNT = 256;

    std::vector<uint64_t> occurence_;
    std::vector<uint64_t> scale_from_;
    std::vector<uint64_t> scale_to_;

    // suma po occurence[i]
    uint64_t occurences_sum_;

    void calculate_scale()
    {
        uint64_t sum = 0;
        for (size_t symbol = 0; symbol < occurence_.size(); symbol++) {
            scale_from_[symbol] = sum;
            sum += occurence_[symbol];
            scale_to_[symbol] = sum;
        }
    }

public:
    adaptive_model()
        : occurence_(SYMBOL_AMOUNT)
        , scale_from_(SYMBOL_AMOUNT)
        , scale_to_(SYMBOL_AMOUNT)
    {
        static_assert(SYMBOL_AMOUNT < MAX_OCCURENCE, "za duza liczba symboli - nie mieszcza sie w maksymalnej dokladnosci");

        // zakladam ze wszystkie pojawily sie z raz aby bylo porowno i nie zero
        for (std::size_t i = 0; i < occurence_.size(); i++)
            occurence_[i] = 1;

        occurences_sum_ = SYMBOL_AMOUNT;

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

    auto scale_from(std::size_t symbol)
    {
        return scale_from_[symbol];
    }

    auto scale_to(std::size_t symbol)
    {
        return scale_to_[symbol];
    }

    void update(std::size_t symbol)
    {
        occurence_[symbol]++;
        occurences_sum_++;

        calculate_scale();
    }
};

inline auto entropy(const std::vector<unsigned char>& set) -> double
{
    auto const size = set.size();

    uint64_t amount[0x100] = {};
    for (size_t i = 0; i < size; ++i) {
        ++amount[set[i]];
    }

    auto const log_size = std::log2(size);
    double entropy = 0;
    for (uint32_t x = 0; x < 0x100; ++x) {

        if (amount[x] != 0) {
            auto diff = amount[x] * (log_size - std::log2(amount[x]));
            entropy += diff;
        }
    }
    entropy /= size;

    return entropy;
}

inline auto average_coding_length(std::size_t coded_size, std::size_t orginal_size) -> double
{
    return 8. * coded_size / orginal_size;
}

inline auto compression_ratio(std::size_t coded_size, std::size_t orginal_size) -> double
{
    return 1. * orginal_size / coded_size;
}

namespace {
    inline auto vector_cast(const std::vector<unsigned char>& char_vector) -> std::vector<bool>
    {
        std::vector<bool> bool_vector {};
        bool_vector.reserve(8 * char_vector.size());

        for (std::size_t byte_id {}; byte_id < char_vector.size(); ++byte_id) {
            for (uint32_t bit_id = 8; bit_id > 0; --bit_id) {
                bool val = char_vector[byte_id] & (1 << (bit_id - 1));
                bool_vector.push_back(val);
            }
        }

        return bool_vector;
    }

    inline auto vector_cast(const std::vector<bool>& bool_vector) -> std::vector<unsigned char>
    {
        std::vector<unsigned char> char_vector(bool_vector.size() / 8 + (bool_vector.size() % 8 ? 1 : 0));

        for (std::size_t i {}; i < bool_vector.size(); ++i) {
            std::size_t byte_id = i / 8;
            uint32_t bit_id = 8 - i % 8;

            unsigned char val = bool_vector[i] ? (1 << (bit_id - 1)) : 0;
            char_vector[byte_id] |= val;
        }

        return char_vector;
    }
}

inline auto encode(const std::vector<unsigned char>& input_vector) -> std::vector<unsigned char>
{
    std::vector<unsigned char> output_vector {};

    adaptive_model model {};

    uint64_t a = 0;
    uint64_t b = WHOLE_INTERVAL;
    uint64_t licznik = 0; // jak na wykladzie -- nie moge wymyslic lepszej nazwy

    std::vector<bool> bool_vector {};
    for (auto byte : input_vector) {

        std::size_t symbol = byte;
        uint64_t b_a = b - a;
        b = a + (b_a * model.scale_to(symbol) / model.occurences_sum());
        a = a + (b_a * model.scale_from(symbol) / model.occurences_sum());

        // skalowanie
        while (true) {
            if (b < HALF_INTERVAL) {
                // oba po lewej stronie - rozszerzenie lewej
                bool_vector.push_back(0);
                for (auto i = decltype(licznik) {}; i < licznik; ++i) {
                    bool_vector.push_back(1);
                }
                licznik = 0;

            } else if (HALF_INTERVAL < a) {
                // oba po prawej stronie - rozszerzenie prawej
                bool_vector.push_back(1);
                for (auto i = decltype(licznik) {}; i < licznik; ++i) {
                    bool_vector.push_back(0);
                }
                licznik = 0;

                a -= HALF_INTERVAL;
                b -= HALF_INTERVAL;
            } else if (QUARTER_INTERVAL <= a && b < 3 * QUARTER_INTERVAL) {
                // a i b sa po srodku
                a -= QUARTER_INTERVAL;
                b -= QUARTER_INTERVAL;
                ++licznik;
            } else {
                // a i b juz sa dluzsze niz polowa -- koniec skalowania
                break;
            }
            a *= 2;
            b *= 2;
        }

        model.update(symbol);
    }

    // skonczyly sie znaki ale przedzial musi osiagnac odpowiednia dlugosc
    licznik += 1;
    if (a < QUARTER_INTERVAL) {
        bool_vector.push_back(0);
        for (auto i = decltype(licznik) {}; i < licznik; ++i) {
            bool_vector.push_back(1);
        }
    } else {
        bool_vector.push_back(1);
        for (auto i = decltype(licznik) {}; i < licznik; ++i) {
            bool_vector.push_back(0);
        }
    }

    output_vector = vector_cast(bool_vector);

    return output_vector;
}

inline auto decode(const std::vector<unsigned char>& input_vector, uint64_t amount_to_decode) -> std::vector<unsigned char>
{
    std::vector<unsigned char> output_vector {};
    output_vector.reserve(amount_to_decode);

    if (amount_to_decode == 0) {
        return output_vector;
    }

    adaptive_model model {};

    std::vector<bool> bool_vector = vector_cast(input_vector);

    uint64_t a = 0;
    uint64_t b = WHOLE_INTERVAL;
    uint64_t z = 0;

    auto iter = bool_vector.begin();
    for (std::size_t i = 1; i <= BITS_PRECISION && iter < bool_vector.end(); ++i) {
        if (!(iter < bool_vector.end())) {
            break;
        }

        if (*iter) {
            z += uint64_t { 1 } << (BITS_PRECISION - i); // z sklada bity od najwiekszego do najmniejszego
        }
        ++iter;
    }

    while (true) {
        std::size_t sym_left = 0;
        std::size_t sym_right = model.size() - 1;
        while (true) {
            std::size_t symbol = sym_left + (sym_right - sym_left) / 2;

            // jakie sa przedzialy skali dla danego symbola
            uint64_t b_a = b - a;
            uint64_t sym_from = a + b_a * model.scale_from(symbol) / model.occurences_sum();
            uint64_t sym_to = a + b_a * model.scale_to(symbol) / model.occurences_sum();

            if (z < sym_from) {
                sym_right = symbol - 1;
            } else if (z >= sym_to) {
                sym_left = symbol + 1;
            } else {
                // z jest w srodku przedzialu tego symbolu -- sukces odczytano symbol

                unsigned char decoded_symbol = static_cast<unsigned char>(symbol);
                output_vector.push_back(decoded_symbol);

                a = sym_from;
                b = sym_to;

                model.update(decoded_symbol);

                if (output_vector.size() == amount_to_decode) {
                    return output_vector;
                } else {
                    break;
                }
            }
        }

        // nie da sie wyznaczyc algorytmicznie momentu wyjscia
        // poniewaz w momencie kiedy skoncza sie bity
        // z ma taka wartosc aby do konca pliku nic nie trzeba bylo do niego dodawac
        // ale nie ma jakiejs dokladnej wartosci po zakonczeniu wczytywania pliku
        // tylko wystarczajaca aby odczytac do ostatniego bajta
        // kodowanie zostawia takie wiszace z w momencie "musi byc jakos wieksze od polowy"
        // a nie da sie dokladnie zrobic calosci bo nie kazda liczba w systemie 2kowym ma dokladna reprezentacje
        // -> zatem trzeba znac ilosc bajtow do odczytania, albo miec specjalny znak zakonczenia
        // -> jest to program do odczytywania binarnych ciagow wiec nie mozna miec specjalnego znaku bo wszystkie musza byc dostepne
        // -> zatem nalezy zapisac liczbe bajtow do odczytania w pliku wynikowym w odpowiednim miejscu

        // skalowanie
        while (true) {
            if (b < HALF_INTERVAL) { // rozszerzenie lewej

            } else if (HALF_INTERVAL < a) { // rozszerzenie prawej

                a -= HALF_INTERVAL;
                b -= HALF_INTERVAL;
                z -= HALF_INTERVAL;

            } else if (QUARTER_INTERVAL <= a && b < 3 * QUARTER_INTERVAL) { // rozszerzenie srodka

                a -= QUARTER_INTERVAL;
                b -= QUARTER_INTERVAL;
                z -= QUARTER_INTERVAL;
            } else {

                break;
            }
            a *= 2;
            b *= 2;
            z *= 2;

            if (iter < bool_vector.end()) {
                if (*iter) {
                    z += 1;
                }
                ++iter;
            }
        }
    }
}
}