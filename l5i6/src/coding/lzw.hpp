#pragma once

/*
KODOWANIE:
0. jezeli jest cos do zakodowania

1. wypelnij slownik
    poczatkowo slownik ma miec wszystkie bajty 0-255

2. rosnac od 1
    szukam najdluzszego poczatkowego z do zakodowania
    ktory istnieje w slowniku i pierwszego ktory nie istnieje.
    jezeli najpiew sie skonczy tekst zanim znajde pierwszy nieistniejacy
    to koduje to co ma jak w 3 ale wychodze

3. koduje liczba (eliasem lub fibonaccim) najdluzszy bedacy w slowniku
    i zapisuje nowy slownika - ten pierwszy ktory nie istnieje.

5. jezeli jeszcze zostalo cos do zakodowania to wroc do 2.

0a 1b 2o 3w 4-
zapisuje, nowy wpis do slowniku, tekst do zakodowania
                                 wabba-wabba-wabba-woo-woo-woo
3                                wabba-wabba-wabba-woo-woo-woo
najdluzszy
poczatkowy
ktory byl
w slowniku
          5 wa
          pierwszy poczatkowy
          ktory nie byl w slowniku
                                 abba-wabba-wabba-woo-woo-woo



3   5wa
0   6ab
1   7bb
1   8ba
0   9a-
4   10-w
5   wab


DEKODOWANIE:
w dekodowaniu jest problem taki ze 
o ile w kodowaniu znasz znak -> zapisujesz kod -> dodajesz do slownika
tak tu 
dodajesz do slownika -> wczytujesz kod czego dodajesz do slownika -> znasz znak
dopiero przy dekodowaniu nastepnego znaku 
wiesz jaki byl poprzedni w slowniku, 
slownik musi to uwzgledniac.

0a 1b 2o 3w 4-

3 w 5w?
0 wa 5wa, 6a?
1 wab 6ab 7b?
1 wabb 7bb 8b?
0 wabba 8ba 9a?

trudny przypadek::
ababababababab....
koduje
1 a 3ab
2 ab 4ba
3 abab 5aba
...

ale przy dekodowaniu:
1 a 3a?
2 ab 3ab 4b?
3 abab 4ba 5ab?
5 ab[ab(a]b?) jak mam wypisac niedokonczony wpis to znaczy ze jego pierwszy znak jest taki sam jak nieznany
bo ta 5 kazala nam odczytac pierwszy znak jako nieznany w kodzie


KODOWANIE ELIASA gamma
000010011
n-1 zer potem n bitow liczby

DEKODOWANIE ELIASA GAMMA
zliczas zera czekajac na jedynke
jak masz jedynke to ile jest zer i odczytujesz kolejnych kilka bitow jako liczbe

KODOWANIE ELIASA OMEGA
zapisz 0
17dec = 10001bin dopisz do poczatku
|10001| = 5 / -14
4dec = 100bin dopisz do poczatku
|100| = 3 / -1 = 2
2dec = 10bin dopisz do poczatku
|10| = 2 / -1 = 1
1 zakoncz
kod 10100100010

DEKODOWANIE ELIASA OMEGA
n = 1
1|0100100010 ->
    n+1 bitow 10|100100010 -> n = 2
1|00100010 ->
    n+1 bitow 100|100010 -> n = 4
1|00010  ->
    n+1 bitow 10001|0 -> n = 17
0| -> return n = 17

KODOWANIE FIBONACCIEGO
znajdujemy najwieksza fib_i <= od k
odejmujemy roznice jako nowe k
stawiamy odpowiadajacy i bit na 1
jezeli roznica to 0 to dostaw jedynke

1 11 bo fib_1 '1
2 011 bo 0fib_1+1fib_2'1
3 0011 bo ...
4 1011
5 00011
6 10011
7 01011
8 000011
9 100011
10 ..
*/

#include "misc.hpp"
#include "natural.hpp"

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <vector>

namespace coding::lzw {

namespace {

    template <typename LIterBegin, typename LIterEnd, typename PIterBegin, typename PIterEnd>
    auto equal_ranges(LIterBegin lbegin, LIterEnd lend, PIterBegin pbegin, PIterEnd pend) -> bool
    {
        while (lbegin < lend && pbegin < pend) {
            if (*lbegin != *lend) {
                return false;
            }

            ++lbegin;
            ++lend;
        }

        return lbegin == lend && pbegin == pend;
    }

    struct dictionary {
        std::vector<std::vector<unsigned char>> set_;

        dictionary()
        {
            for (uint16_t i {}; i < 256; ++i) {
                set_.push_back(std::vector<unsigned char> { static_cast<unsigned char>(i) });
            }
        }

        auto find(std::vector<unsigned char>::const_iterator begin, std::vector<unsigned char>::const_iterator end) -> std::optional<uint64_t>
        {
            for (uint64_t i {}; i < set_.size(); ++i) {
                const auto& bytes = set_[i];
                if (equal_ranges(begin, end, bytes.begin(), bytes.end())) {
                    return { i };
                }
            }

            return {};
        }

        auto add(const std::vector<unsigned char>& to_add)
        {
            set_.push_back(to_add);
        }
    };

}

template <typename Encode>
auto encode(const std::vector<unsigned char>& data) -> std::vector<unsigned char>
{
    if (data.size() == 0) {
        return data;
    }

    std::vector<bool> encoded {};
    std::vector<unsigned char> c {};

    dictionary dict {};

    auto iter { data.begin() };
    c.push_back(*iter++);

    while (iter < data.end()) {
        c.push_back(*iter++);

        if (!dict.find(c.begin(), c.end())) {

            auto index_opt { dict.find(c.begin(), c.end() - 1) };
            if (!index_opt) {
                // nie powinno. zwalony algorytm. ale lepiej sprawdzic. najwyzej usun
                throw std::logic_error("nie powinno sie wydazyc :O");
            }
            auto index = *index_opt;

            auto&& encoded_number = Encode(index);
            encoded.insert(encoded.end(), encoded_number.begin(), encoded_number.end());
            
            dict.add(c);
            
            c.erase(c.begin(), c.end() - 1);
        }
    }

    return coding::misc::vector_cast(encoded);
}

template <typename Decode>
auto decode(const std::vector<unsigned char>& coded) -> std::vector<unsigned char>
{
    std::vector<unsigned char> decoded {};

    dictionary dict {};

    auto encoded_bits = coding::misc::vector_cast(coded);
    auto iter = encoded_bits.begin();

    auto pk_opt = Decode(iter, encoded_bits.end());
    if (!pk_opt) {
        throw std::runtime_error {"niepoprawnie zapisany pierwszy kod"};
    }
    auto pk = *pk_opt;

    decoded.insert(decoded.end(), dict.set_[*pk_opt]);

    while (true) {

        auto k_opt = Decode(iter, encoded_bits.end());
        if (!k_opt) {
            break;
        }
        auto k = *pk_opt;


        std::vector<unsigned char>& pc = dict.set_[pk];

        if (k < dict.set_.size()) {
            const std::vector<unsigned char>& slownik_k = dict.set_[k];

            pc.insert(pc.end(), slownik_k[0]);
            dict.add(pc);

            decoded.insert(decoded.end(), slownik_k.begin(), slownik_k.end());
        } else {
            // tu jest ten przypadek z pierwsza == ostatnia
            pc.insert(pc.end(), pc[0]);
            dict.add(pc);

            decoded.insert(decoded.end(), pc.begin(), pc.end());
        }

        pk = k;
    }

    return decoded;
}

}