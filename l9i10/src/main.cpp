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
#include <functional>
#include <ios>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>

struct options {
    std::string tga_file_path {};
    std::string tga_save_file_path {};
    std::string opt_mode {};

    std::string r_quant {};
    std::string g_quant {};
    std::string b_quant {};

    std::string bits {};
};

struct quants {
    uint32_t r_quant {};
    uint32_t g_quant {};
    uint32_t b_quant {};
};

auto operator<<(std::ostream& o, quants const& q) -> std::ostream&
{
    // clang-format off
    o << "{"
        << "r_bits=" << q.r_quant << ","
        << "g_bits=" << q.g_quant << ","
        << "b_bits=" << q.b_quant
        << "}";
    // clang-format on

    return o;
}

using chooser = std::function<quants(std::vector<uint8_t> const&)>;

auto choose_chooser(const options& opts) -> chooser
{
    quants q {};
    {
        std::stringstream ss { opts.r_quant };
        ss >> q.r_quant;
    }
    {
        std::stringstream ss { opts.g_quant };
        ss >> q.g_quant;
    }
    {
        std::stringstream ss { opts.b_quant };
        ss >> q.b_quant;
    }

    uint32_t bits { 24 };
    {
        std::stringstream ss { opts.bits };
        ss >> bits;
    }

    if (opts.opt_mode == "mse") {
        return [bits](std::vector<uint8_t> const& rgb_vals) -> quants {

            auto [r_vals, g_vals, b_vals] = tga::split_channels(rgb_vals);
            
            quants best = {0, 0, 0};
            auto mse_best = std::optional<double>{};
            for (uint32_t q_r {}; q_r <= bits; ++q_r) {
                for (uint32_t q_g {}; q_g <= bits - q_r; ++q_g) {
                    uint32_t q_b = bits - q_r - q_g;

                    std::vector<uint8_t> r_vals_quantized = coding::uniform_quantization(r_vals, q_r);
                    std::vector<uint8_t> g_vals_quantized = coding::uniform_quantization(g_vals, q_g);
                    std::vector<uint8_t> b_vals_quantized = coding::uniform_quantization(b_vals, q_b);

                    std::vector<uint8_t> rgb_vals_quantized = tga::join_channels(r_vals_quantized, g_vals_quantized, b_vals_quantized);
                    double mse = coding::statistics::mse(rgb_vals, rgb_vals_quantized);

                    std::cout << quants{q_r, q_g, q_b} << "-->" << mse << std::endl;

                    if (!mse_best || (mse_best && *mse_best > mse)) {
                        mse_best = mse;
                        best = {q_r, q_g, q_b};
                    }
                }
            }

            return best;
        };
    } else if (opts.opt_mode == "snr") {
        return [bits](std::vector<uint8_t> const& rgb_vals) -> quants {
            auto [r_vals, g_vals, b_vals] = tga::split_channels(rgb_vals);
            
            quants best = {0, 0, 0};
            auto snr_best = std::optional<double>{};
            for (uint32_t q_r {}; q_r <= bits; ++q_r) {
                for (uint32_t q_g {}; q_g <= bits - q_r; ++q_g) {
                    uint32_t q_b = bits - q_r - q_g;

                    std::vector<uint8_t> r_vals_quantized = coding::uniform_quantization(r_vals, q_r);
                    std::vector<uint8_t> g_vals_quantized = coding::uniform_quantization(g_vals, q_g);
                    std::vector<uint8_t> b_vals_quantized = coding::uniform_quantization(b_vals, q_b);

                    std::vector<uint8_t> rgb_vals_quantized = tga::join_channels(r_vals_quantized, g_vals_quantized, b_vals_quantized);
                    double mse = coding::statistics::mse(rgb_vals, rgb_vals_quantized);
                    double snr = coding::statistics::snr(rgb_vals, mse);

                    std::cout << quants{q_r, q_g, q_b} << "-->" << snr << std::endl;

                    if (!snr_best || (snr_best && *snr_best < snr)) {
                        snr_best = snr;
                        best = {q_r, q_g, q_b};
                    }
                }
            }

            return best;
        };
    } else if (opts.opt_mode == "manual") {
        return [q]([[maybe_unused]] std::vector<uint8_t> const& rgb) -> quants {
            return q;
        };
    }

    throw std::runtime_error { "nie tryb dzialania=" + opts.opt_mode };
}

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

    tga::image image {};
    image.from_binary(data);
    std::cout << image._header << std::endl;

    if (image._image_format != tga::image_format::RGB) {
        throw std::runtime_error { "obrazek nie jest w formacie rgb" };
    }

    auto best_quants = choose_chooser(opts);
    quants q = best_quants(image._data);

    auto [r_vals, g_vals, b_vals] = tga::split_channels(image._data);

    std::vector<uint8_t> r_vals_quantized = uniform_quantization(r_vals, q.r_quant);
    std::vector<uint8_t> g_vals_quantized = uniform_quantization(g_vals, q.g_quant);
    std::vector<uint8_t> b_vals_quantized = uniform_quantization(b_vals, q.b_quant);

    std::vector<uint8_t> rgb_vals_quantized = tga::join_channels(r_vals_quantized, g_vals_quantized, b_vals_quantized);

    tga::accessor_RGB accessor { image._data, image._width, image._height };
    tga::accessor_MONO accessor_r { r_vals, image._width, image._height };
    tga::accessor_MONO accessor_g { g_vals, image._width, image._height };
    tga::accessor_MONO accessor_b { b_vals, image._width, image._height };

    tga::accessor_RGB accessor_quantized { rgb_vals_quantized, image._width, image._height };
    tga::accessor_MONO accessor_r_quantized { r_vals_quantized, image._width, image._height };
    tga::accessor_MONO accessor_g_quantized { g_vals_quantized, image._width, image._height };
    tga::accessor_MONO accessor_b_quantized { b_vals_quantized, image._width, image._height };

    {
        auto mse = statistics::mse(accessor._image, accessor_quantized._image);
        auto mse_r = statistics::mse(accessor_r._image, accessor_r_quantized._image);
        auto mse_g = statistics::mse(accessor_g._image, accessor_g_quantized._image);
        auto mse_b = statistics::mse(accessor_b._image, accessor_b_quantized._image);

        auto snr = statistics::snr(accessor_quantized._image, mse);
        auto snr_r = statistics::snr(accessor_r_quantized._image, mse_r);
        auto snr_g = statistics::snr(accessor_g_quantized._image, mse_g);
        auto snr_b = statistics::snr(accessor_b_quantized._image, mse_b);

        std::cout << "entropia pliku wejsciowego    =" << statistics::entropy(accessor._image) << std::endl;
        std::cout << "entropia pliku wejsciowego (R)=" << statistics::entropy(accessor_r._image) << std::endl;
        std::cout << "entropia pliku wejsciowego (G)=" << statistics::entropy(accessor_g._image) << std::endl;
        std::cout << "entropia pliku wejsciowego (B)=" << statistics::entropy(accessor_b._image) << std::endl;

        std::cout << "mse    =" << mse << std::endl;
        std::cout << "mse (R)=" << mse_r << std::endl;
        std::cout << "mse (G)=" << mse_g << std::endl;
        std::cout << "mse (B)=" << mse_b << std::endl;

        std::cout << "snr    =" << snr << " (" << statistics::to_decibels(snr) << "dB)" << std::endl;
        std::cout << "snr (R)=" << snr_r << " (" << statistics::to_decibels(snr_r) << "dB)" << std::endl;
        std::cout << "snr (G)=" << snr_g << " (" << statistics::to_decibels(snr_g) << "dB)" << std::endl;
        std::cout << "snr (B)=" << snr_b << " (" << statistics::to_decibels(snr_b) << "dB)" << std::endl;

        std::cout << "entropia pliku wyjsciowego    =" << statistics::entropy(accessor_quantized._image) << std::endl;
        std::cout << "entropia pliku wyjsciowego (B)=" << statistics::entropy(accessor_b_quantized._image) << std::endl;
        std::cout << "entropia pliku wyjsciowego (R)=" << statistics::entropy(accessor_r_quantized._image) << std::endl;
        std::cout << "entropia pliku wyjsciowego (G)=" << statistics::entropy(accessor_g_quantized._image) << std::endl;

        std::cout << "wybrany kwantyzator=" << q << std::endl;
    }

    std::ofstream save_file { opts.tga_save_file_path };
    if (!save_file) {
        throw std::runtime_error { "nie mozna otworzyc pliku=" + opts.tga_save_file_path };
    }

    tga::image save_image = image;
    save_image._data = rgb_vals_quantized;

    std::vector<uint8_t> save_data = save_image.to_binary();

    save_file << save_data;
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

    parser.set_optional({ .write_to = opts.bits, .symbol = "--bits" });

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