#include <coding/natural.hpp>
#include <cstdint>
#include <utils/vector_streams.hpp>

#include <stdexcept>
#include <string>
#include <vector>
// #include <iostream>

using algo = coding::natural::elias_omega;

template <typename T>
void assert_equal(T expected, T actual)
{
    if (expected != actual) {
        throw std::runtime_error { "expected=" + std::to_string(expected) + " actual=" + std::to_string(actual) };
    }
}

void test(uint64_t value)
{
    auto encoded = algo::encode(value);
    auto iter = encoded.cbegin();
    auto decoded = algo::decode(iter, encoded.end());

    if (!decoded) {
        throw std::runtime_error { "nima wartosci dla " + std::to_string(value) };
    }

    assert_equal(value, *decoded);
}

void test2() 
{
    auto a1 = algo::encode(11);
    auto a2 = algo::encode(114);
    auto a3 = algo::encode(11445);

    auto all = a1;
    all.insert(all.end(), a2.begin(), a2.end());
    all.insert(all.end(), a3.begin(), a3.end());

    // std::cout << all << std::endl;

    auto iter = all.cbegin();

    auto v1 = *algo::decode(iter, all.end());
    auto v2 = *algo::decode(iter, all.end());
    auto v3 = *algo::decode(iter, all.end());

    assert_equal(decltype(v1) { 11 }, v1);
    assert_equal(decltype(v1) { 114 }, v2);
    assert_equal(decltype(v1) { 11445 }, v3);
}

void test3() 
{
    std::vector<bool> all {};
    for (uint64_t character = 0; character < 256; ++character) {
        for (uint64_t i = 0; i < 30; ++i) {
            auto sfg = algo::encode(character + 1);
            all.insert(all.end(), sfg.begin(), sfg.end());
        }
    }

    auto iter = all.cbegin();
    for (uint64_t character = 0; character < 256; ++character) {
        for (uint64_t i = 0; i < 30; ++i) {
            auto opt = algo::decode(iter, all.end());
            if (!opt) {
                throw std::runtime_error {"nie moze odczytac zapisanej wartosci " + std::to_string(character)};
            }
            auto val = *opt - 1;

            if (character != val) {
                throw std::runtime_error {"zla wartosc char=" + std::to_string(character) + " val=" + std::to_string(val)};
            }
        }
    }
}

int main()
{
    // using utils::vector_streams::serialization::operator<<;
    // std::cout << algo::encode(11) << std::endl;
    // std::cout << algo::encode(114) << std::endl;
    // std::cout << algo::encode(11445) << std::endl;

    test(0x1);
    test(17);
    test(2137);
    test(21371337);
    test(0xFFFFFFFFFFFFFFFF);
    test(0x1afcbe0011489425);

    test2();

    test3();
}