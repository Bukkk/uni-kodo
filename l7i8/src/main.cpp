#include "moje_tga.hpp"
#include "utils/args_helper.hpp"
#include "utils/time_it.hpp"
#include "utils/vector_streams.hpp"

// #include "extern/tga.hpp"

#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <vector>

auto run_on_file(std::string const& path)
{
    using utils::vector_streams::binary::operator<<;
    using utils::vector_streams::binary::operator>>;

    std::vector<unsigned char> data {};
    std::ifstream file { path };
    if (!file) {
        throw std::runtime_error { "nie mozna otworzyc pliku=" + path };
    }

    file >> data;
    // std::cout << data << std::endl;

    // tga::TGA image {};
    // image.Load(path);
    // auto format = image.GetFormat();
    // if (format != tga::ImageFormat::RGB) {
    //     throw std::runtime_error{ "format obrazu nie jest rgb"};
    // }

    tga::image image {data};
    std::cout << image._header << std::endl;

    if (image._image_format != tga::image_format::RGB) {
        throw std::runtime_error {"obrazek nie jest w formacie rgb"};
    }

    tga::accessor_RGB accessor { image._data, image._width, image._height };
    auto [r_vals, g_vals, b_vals]  = tga::split_channels(accessor);

    tga::accessor_MONO accessor_r {r_vals, image._width, image._height};
    tga::accessor_MONO accessor_g {g_vals, image._width, image._height};
    tga::accessor_MONO accessor_b {b_vals, image._width, image._height};


    // std::cout << "00 " << accessor.get(0, 0) << std::endl;
    // std::cout << "01 " << accessor.get(0, 1) << std::endl;
    // std::cout << "10 " << accessor.get(1, 0) << std::endl;
    // std::cout << "11 " << accessor.get(1, 1) << std::endl;
    // std::cout << "02 " << accessor.get(0, 2) << std::endl;
    // std::cout << "kk " << accessor.get(13, 0) << std::endl;


    // std::cout << "pd" << accessor.get(image._width - 1, 0) << std::endl;
    // std::cout << "lg" << accessor.get(0, image._height - 1) << std::endl;
    // std::cout << "pg" << accessor.get(image._width - 1, image._height - 1) << std::endl;
    // std::cout << "pg-r" << (uint16_t) accessor_r.get(image._width - 1, image._height - 1) << std::endl;
    // std::cout << "pg-g" << (uint16_t) accessor_g.get(image._width - 1, image._height - 1) << std::endl;
    // std::cout << "pg-b" << (uint16_t) accessor_b.get(image._width - 1, image._height - 1) << std::endl;

    



}

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    utils::args_helper parser {
        "program testujacy kodowaie w tga\n"
        "\n"
        "uzycie: program <plik tga> [-h help]\n"
    };

    std::string tga_file_path {};
    parser.set_positional({ .write_to = tga_file_path });

    if (!parser.parse(argc, argv)) {
        std::cout << parser.help_page();
        return 0;
    }

    utils::time_it<std::chrono::milliseconds> timer {};

    timer.set();
    run_on_file(tga_file_path);
    uint64_t time = timer.measure();

    std::cout << "czas dzialania=" << time << "ms\n";
}