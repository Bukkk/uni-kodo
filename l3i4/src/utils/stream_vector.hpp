#pragma once

#include <cstddef>
#include <istream>
#include <vector>

namespace utils::vector_stream_operators {

namespace binary {
    namespace detail {
        inline auto length(std::istream& istream) -> std::size_t
        {
            auto&& start = istream.tellg();
            istream.seekg(0, std::ios::end);
            auto&& size = istream.tellg();
            istream.seekg(start, std::ios::beg);

            return size;
        }

        template <typename T>
        auto read_byte(std::istream& istream)
        {
            char data;
            istream.read(&data, 1);

            return static_cast<T>(data);
        }

        template <typename T>
        void write_byte(std::ostream& ostream, T byte)
        {
            char data = static_cast<char>(byte);
            ostream.write(&data, 1);
        }
    }

    inline auto operator>>(std::istream& istream, std::vector<unsigned char>& vec) -> std::istream&
    {
        auto length = detail::length(istream);

        for (auto i = decltype(length) {}; i < length; ++i) {
            auto byte = detail::read_byte<unsigned char>(istream);
            vec.push_back(byte);
        }

        return istream;
    }

    inline auto operator<<(std::ostream& ostream, std::vector<unsigned char>& vec) -> std::ostream&
    {
        for (std::size_t i {}; i < vec.size(); ++i) {
            detail::write_byte<unsigned char>(ostream, vec[i]);
        }

        return ostream;
    }
}

namespace serialization {
    template <typename T>
    auto operator<<(std::ostream& ost, const std::vector<T>& vec) -> std::ostream&
    {
        ost << "[";

        bool first = true;
        for (auto& it : vec) {
            if (first) {
                first = false;

            } else {
                ost << ", ";
            }

            ost << it;
        }

        ost << "]";

        return ost;
    }
}

}
