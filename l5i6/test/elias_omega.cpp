#include <coding/natural.hpp>
#include <utils/vector_streams.hpp>

#include <stdexcept>
#include <string>
// #include <iostream>

namespace algo = coding::natural::elias_omega;

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