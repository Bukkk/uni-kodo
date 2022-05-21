#include "../src/moje_tga.hpp"
#include <cassert>

int main()
{
    tga::header h = {0, 0, 2, 0 ,0, 0, 0, 0, 133, 200, 24, 0};
    std::vector<uint8_t> vec = tga::header::repr(h);
    auto it = vec.cbegin();
    tga::header h2 = tga::header::parse(it, vec.cend());

    assert(h == h2);
}