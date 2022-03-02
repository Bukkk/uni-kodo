#include <cstddef>
#include <fstream>
#include <vector>

namespace files {

template <typename T>
inline auto read_binary(char* name)
{
    std::vector<T> vec {};

    std::ifstream file(name, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        auto size = file.tellg();
        file.seekg(0, std::ios::beg);

        vec.reserve(size / sizeof(T));
        for (size_t i = 0; i < size; i += sizeof(T)) {
            char symbol[sizeof(T)];
            file.read(symbol, sizeof(T));

            vec.push_back(*reinterpret_cast<T*>(symbol));
        }

        file.close();
    }

    return vec;
}
}