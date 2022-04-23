#include <coding/natural.hpp>

#include <stdexcept>
#include <string>

using algo = coding::natural::elias_gamma;

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
    auto decoded = *algo::decode(iter, encoded.end());

    assert_equal(value, decoded);
}

int main()
{
    test(11);
    test(114);
    test(11445);
    test(2137);
    test(21371337);
    test(0x1);
    test(0xFFFFFFFFFFFFFFFF);
    test(0x1afcbe0011489425);

    auto a1 = algo::encode(11);
    auto a2 = algo::encode(114);
    auto a3 = algo::encode(11445);

    auto all = a1;
    all.insert(all.end(), a2.begin(), a2.end());
    all.insert(all.end(), a3.begin(), a3.end());

    auto iter = all.cbegin();

    auto v1 = *algo::decode(iter, all.end());
    auto v2 = *algo::decode(iter, all.end());
    auto v3 = *algo::decode(iter, all.end());

    assert_equal(decltype(v1) { 11 }, v1);
    assert_equal(decltype(v1) { 114 }, v2);
    assert_equal(decltype(v1) { 11445 }, v3);
}