#include <vector>
#include <fstream>

namespace files {

inline auto read(char* name)
{
    std::vector<char> vec {};

    std::ifstream file(name, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);

        vec.resize(size);
        file.read(vec.data(), size);
        file.close();
    }
    return vec;
}
}