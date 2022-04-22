#include <coding/natural.hpp>
#include <stdexcept>
#include <string>

auto test(uint64_t value)
{
    auto encoded = coding::natural::elias_gamma::encode(value);
    auto iter = encoded.cbegin();
    auto decoded = *coding::natural::elias_gamma::decode(iter, encoded.end());

    if (decoded != value) {
        throw std::runtime_error{"expected=" + std::to_string(value) + " got=" + std::to_string(decoded)};
    }
}

int main() {
    test(11);
    test(114);
    test(11445);
    test(2137);    
    test(21371337);    
    test(0x1);    
    test(0xFFFFFFFFFFFFFFFF);
    test(0x1afcbe0011489425);

    auto a1 = coding::natural::elias_delta::encode(11);
    auto a2 = coding::natural::elias_delta::encode(114);
    auto a3 = coding::natural::elias_delta::encode(11445);

    auto all = a1;
    all.insert(all.end(), a2.begin(), a2.end());
    all.insert(all.end(), a3.begin(), a3.end());

    auto iter = all.cbegin();

    auto v1 = *coding::natural::elias_delta::decode(iter, all.end());
    auto v2 = *coding::natural::elias_delta::decode(iter, all.end());
    auto v3 = *coding::natural::elias_delta::decode(iter, all.end());

    if (v1 != 11) {
        throw std::runtime_error{"dlugie expected=" "11" " got=" + std::to_string(v1)};
    }

    if (v2 != 114) {
        throw std::runtime_error{"dlugie expected=" "114" " got=" + std::to_string(v2)};
    }

    if (v3 != 11445) {
        throw std::runtime_error{"dlugie expected=" "11445" " got=" + std::to_string(v3)};
    }
}