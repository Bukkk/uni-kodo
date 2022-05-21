#include "coding/graphics_statistics.hpp"
#include "coding/quantization.hpp"
#include "coding/statistics.hpp"
// #include "jpg_coders.hpp"
#include "moje_tga.hpp"
#include "utils/args_helper.hpp"
#include "utils/time_it.hpp"
#include "utils/vector_streams.hpp"

// #include "extern/tga.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>

struct options {
    std::string tga_file_path {};
    std::string tga_save_file_path {};
    std::string opt_mode {};

    std::string r_quant {};
    std::string g_quant {};
    std::string b_quant {};
};

struct quants {
    uint32_t r_quant {};
    uint32_t g_quant {};
    uint32_t b_quant {};
};

auto run_on_file(const options& opts)
{
    using namespace coding;
    using utils::vector_streams::binary::operator<<;
    using utils::vector_streams::binary::operator>>;

    std::vector<unsigned char> data {};
    std::ifstream file { opts.tga_file_path };
    if (!file) {
        throw std::runtime_error { "nie mozna otworzyc pliku=" + opts.tga_file_path };
    }

    file >> data;

    tga::image image { data };
    std::cout << image._header << std::endl;

    if (image._image_format != tga::image_format::RGB) {
        throw std::runtime_error { "obrazek nie jest w formacie rgb" };
    }

    tga::accessor_RGB accessor { image._data, image._width, image._height };
    auto [r_vals, g_vals, b_vals] = tga::split_channels(accessor);

    std::vector<uint8_t> r_vals_quantized = uniform_quantization(r_vals, 3);
    std::vector<uint8_t> g_vals_quantized = uniform_quantization(g_vals, 3);
    std::vector<uint8_t> b_vals_quantized = uniform_quantization(b_vals, 2);

    tga::accessor_MONO accessor_r { r_vals, image._width, image._height };
    tga::accessor_MONO accessor_g { g_vals, image._width, image._height };
    tga::accessor_MONO accessor_b { b_vals, image._width, image._height };

    tga::accessor_MONO accessor_r_quantized { r_vals_quantized, image._width, image._height };
    tga::accessor_MONO accessor_g_quantized { g_vals_quantized, image._width, image._height };
    tga::accessor_MONO accessor_b_quantized { b_vals_quantized, image._width, image._height };

    std::vector<uint8_t> rgb_vals_quantized = tga::join_channels(accessor_r_quantized, accessor_g_quantized, accessor_b_quantized);

    tga::accessor_RGB accessor_quantized { rgb_vals_quantized, image._width, image._height };

    {
        auto mse = statistics::mse(accessor._image, accessor_quantized._image);
        auto mse_r = statistics::mse(accessor_r._image, accessor_r_quantized._image);
        auto mse_g = statistics::mse(accessor_g._image, accessor_g_quantized._image);
        auto mse_b = statistics::mse(accessor_b._image, accessor_b_quantized._image);

        std::cout << "entropia pliku    =" << statistics::entropy(accessor._image) << std::endl;
        std::cout << "entropia pliku (R)=" << statistics::entropy(accessor_r._image) << std::endl;
        std::cout << "entropia pliku (G)=" << statistics::entropy(accessor_g._image) << std::endl;
        std::cout << "entropia pliku (B)=" << statistics::entropy(accessor_b._image) << std::endl;

        std::cout << "mse    =" << mse << std::endl;
        std::cout << "mse (R)=" << mse_r << std::endl;
        std::cout << "mse (G)=" << mse_g << std::endl;
        std::cout << "mse (B)=" << mse_b << std::endl;

        auto snr = statistics::snr(accessor_quantized._image, mse);
        auto snr_r = statistics::snr(accessor_r_quantized._image, mse_r);
        auto snr_g = statistics::snr(accessor_g_quantized._image, mse_g);
        auto snr_b = statistics::snr(accessor_b_quantized._image, mse_b);

        std::cout << "snr    =" << snr << " (" << statistics::to_decibels(snr) << "dB)" << std::endl;
        std::cout << "snr (R)=" << snr_r << " (" << statistics::to_decibels(snr_r) << "dB)" << std::endl;
        std::cout << "snr (G)=" << snr_g << " (" << statistics::to_decibels(snr_g) << "dB)" << std::endl;
        std::cout << "snr (B)=" << snr_b << " (" << statistics::to_decibels(snr_b) << "dB)" << std::endl;

        std::cout << "entropia pliku wyjsciowego    =" << statistics::entropy(accessor_quantized._image) << std::endl;
        std::cout << "entropia pliku wyjsciowego (B)=" << statistics::entropy(accessor_b_quantized._image) << std::endl;
        std::cout << "entropia pliku wyjsciowego (R)=" << statistics::entropy(accessor_r_quantized._image) << std::endl;
        std::cout << "entropia pliku wyjsciowego (G)=" << statistics::entropy(accessor_g_quantized._image) << std::endl;
    }
}

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    utils::args_helper parser {
        "kodowanie rownomierne\n"
        "\n"
        "uzycie: program <plik tga> <zapis pliku tga> <mse/snr/manual> [jezeli manual to -r val -g val -b val] [-h help]\n"
    };
    options opts {};

    parser.set_positional({ .write_to = opts.tga_file_path });
    parser.set_positional({ .write_to = opts.tga_save_file_path });
    parser.set_positional({ .write_to = opts.opt_mode });

    parser.set_optional({ .write_to = opts.r_quant, .symbol = "-r" });
    parser.set_optional({ .write_to = opts.g_quant, .symbol = "-g" });
    parser.set_optional({ .write_to = opts.b_quant, .symbol = "-b" });

    if (!parser.parse(argc, argv)) {
        std::cout << parser.help_page();
        return 0;
    }

    utils::time_it<std::chrono::milliseconds> timer {};

    timer.set();
    run_on_file(opts);
    uint64_t time = timer.measure();

    std::cout << "czas dzialania=" << time << "ms\n";
}