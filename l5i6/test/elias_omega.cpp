#include <coding/natural.hpp>
#include <stdexcept>

auto test(uint64_t value)
{
    auto encoded = coding::natural::elias_omega::encode(value);
    auto iter = encoded.cbegin();
    auto decoded = *coding::natural::elias_omega::decode(iter, encoded.end());

    if (decoded != value) {
        throw std::runtime_error{"expected=" + std::to_string(value) + " got=" + std::to_string(decoded)};
    }
}

int main() {
    test(2137);    
    test(21371337);    
    test(0x1);    
    test(0xFFFFFFFFFFFFFFFF);
    test(0x1afcbe0011489425);
}