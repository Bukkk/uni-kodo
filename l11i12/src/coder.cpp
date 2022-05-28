#include "coding/graphics_statistics.hpp"
#include "coding/jpg_coders.hpp"
#include "coding/quantization.hpp"
#include "coding/statistics.hpp"
// #include "jpg_coders.hpp"
#include "coding/quants.hpp"
#include "moje_tga.hpp"
#include "utils/args_helper.hpp"
#include "utils/time_it.hpp"
#include "utils/vector_streams.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <vector>

struct options {
    std::string program_mode {};
    std::string tga_file_path {};
    std::string tga_save_file_path {};
    std::string quant_mode {};

    std::string r_quant {};
    std::string g_quant {};
    std::string b_quant {};

    std::string bits {};
};

auto create_chooser(const options& opts) -> quant_chooser
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

    if (opts.quant_mode == "mse") {
        return quant_choosers::mse { bits };
    } else if (opts.quant_mode == "snr") {
        return quant_choosers::snr { bits };
    } else if (opts.quant_mode == "manual_rgb") {
        return [q]([[maybe_unused]] std::vector<uint8_t> const& rgb) -> quants {
            return q;
        };
    } else if (opts.quant_mode == "manual") {
        return [bits]([[maybe_unused]] std::vector<uint8_t> const& rgb) -> quants {
            return { bits, bits, bits };
        };
    }

    throw std::runtime_error { "nie tryb dzialania=" + opts.quant_mode };
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

    auto quant_chooser = create_chooser(opts);
    quants q = quant_chooser(image._data);
    {
        std::cout << "wybrany kwantyzator=" << q << std::endl;
    }

    auto [r_vals, g_vals, b_vals] = tga::split_channels(image._data);

    std::vector<uint8_t> r_vals_quantized = uniform_quantization(r_vals, q.r_quant);
    std::vector<uint8_t> g_vals_quantized = uniform_quantization(g_vals, q.g_quant);
    std::vector<uint8_t> b_vals_quantized = uniform_quantization(b_vals, q.b_quant);

    std::vector<uint8_t> rgb_vals_quantized = tga::join_channels(r_vals_quantized, g_vals_quantized, b_vals_quantized);

    {
        tga::accessor_RGB accessor { image._data, image._width, image._height };
        tga::accessor_MONO accessor_r { r_vals, image._width, image._height };
        tga::accessor_MONO accessor_g { g_vals, image._width, image._height };
        tga::accessor_MONO accessor_b { b_vals, image._width, image._height };

        tga::accessor_RGB accessor_quantized { rgb_vals_quantized, image._width, image._height };
        tga::accessor_MONO accessor_r_quantized { r_vals_quantized, image._width, image._height };
        tga::accessor_MONO accessor_g_quantized { g_vals_quantized, image._width, image._height };
        tga::accessor_MONO accessor_b_quantized { b_vals_quantized, image._width, image._height };

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

auto encode(std::vector<uint8_t> const& input_data, quants q) -> std::vector<uint8_t>
{
    using namespace coding;

    tga::image image {};
    image.from_binary(input_data);
    std::cout << image._header << std::endl;

    if (image._image_format != tga::image_format::RGB) {
        throw std::runtime_error { "obrazek nie jest w formacie rgb" };
    }

    auto [r_vals, g_vals, b_vals] = tga::split_channels(image._data);

    std::vector<uint8_t> r_vals_quantized = uniform_quantization(r_vals, q.r_quant);
    std::vector<uint8_t> g_vals_quantized = uniform_quantization(g_vals, q.g_quant);
    std::vector<uint8_t> b_vals_quantized = uniform_quantization(b_vals, q.b_quant);

    tga::accessor_MONO quantized_accessor_r { r_vals_quantized, image._width, image._height };
    tga::accessor_MONO quantized_accessor_g { g_vals_quantized, image._width, image._height };
    tga::accessor_MONO quantized_accessor_b { b_vals_quantized, image._width, image._height };

    std::vector<uint8_t> r_vals_diff = differential_coding::encode<jpg_predictors::predictor_new>(quantized_accessor_r);
    std::vector<uint8_t> g_vals_diff = differential_coding::encode<jpg_predictors::predictor_new>(quantized_accessor_g);
    std::vector<uint8_t> b_vals_diff = differential_coding::encode<jpg_predictors::predictor_new>(quantized_accessor_b);

    std::vector<uint8_t> rgb_vals_diff = tga::join_channels(r_vals_diff, g_vals_diff, b_vals_diff);

    tga::image save_image = image;
    save_image._data = rgb_vals_diff; // FIXME niepotrzebna kopia

    return save_image.to_binary();
}

auto decode(std::vector<uint8_t> const& input_data, [[maybe_unused]] quants q) -> std::vector<uint8_t>
{
    using namespace coding;

    tga::image image {};
    image.from_binary(input_data);
    std::cout << image._header << std::endl;

    if (image._image_format != tga::image_format::RGB) {
        throw std::runtime_error { "obrazek nie jest w formacie rgb" };
    }

    auto [r_vals_diff, g_vals_diff, b_vals_diff] = tga::split_channels(image._data);

    tga::accessor_MONO quantized_accessor_r { r_vals_diff, image._width, image._height };
    tga::accessor_MONO quantized_accessor_g { g_vals_diff, image._width, image._height };
    tga::accessor_MONO quantized_accessor_b { b_vals_diff, image._width, image._height };

    std::vector<uint8_t> r_vals_quantized = differential_coding::decode<jpg_predictors::predictor_new>(quantized_accessor_r);
    std::vector<uint8_t> g_vals_quantized = differential_coding::decode<jpg_predictors::predictor_new>(quantized_accessor_g);
    std::vector<uint8_t> b_vals_quantized = differential_coding::decode<jpg_predictors::predictor_new>(quantized_accessor_b);

    std::vector<uint8_t> rgb_vals_quantized = tga::join_channels(r_vals_quantized, g_vals_quantized, b_vals_quantized);

    tga::image save_image = image;
    save_image._data = rgb_vals_quantized; // FIXME niepotrzebna kopia

    return save_image.to_binary();
}

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    utils::args_helper parser {
        "kodowanie rownomierne\n"
        "\n"
        "uzycie: program <encode/decode> <wejsciowy plik tga> <wyjsciowy pliku tga> <mse/snr/manual/manual_rgb> [jezeli manual_rgb to -r val -g val -b val] [-h help]\n"
    };
    options opts {};

    parser.set_positional({ .write_to = opts.program_mode });
    parser.set_positional({ .write_to = opts.tga_file_path });
    parser.set_positional({ .write_to = opts.tga_save_file_path });
    parser.set_positional({ .write_to = opts.quant_mode });

    parser.set_optional({ .write_to = opts.r_quant, .symbol = "-r" });
    parser.set_optional({ .write_to = opts.g_quant, .symbol = "-g" });
    parser.set_optional({ .write_to = opts.b_quant, .symbol = "-b" });

    parser.set_optional({ .write_to = opts.bits, .symbol = "--bits" });

    bool help {};
    parser.set_boolean({ .write_to = help, .symbol = "-h" });
    parser.set_boolean({ .write_to = help, .symbol = "--help" });

    if (!parser.parse(argc, argv) || help) {
        std::cout << parser.help_page();
        return 0;
    }

    using namespace coding;
    using utils::vector_streams::binary::operator<<;
    using utils::vector_streams::binary::operator>>;

    std::vector<unsigned char> input_data {};
    std::ifstream input_file { opts.tga_file_path };
    if (!input_file) {
        throw std::runtime_error { "nie mozna otworzyc pliku=" + opts.tga_file_path };
    }

    input_file >> input_data;
    std::vector<uint8_t> output_data {};

    utils::time_it<std::chrono::milliseconds> timer {};
    timer.set();

    quants q {};
    {
        tga::image image {};
        image.from_binary(input_data);

        auto quant_chooser = create_chooser(opts);
        q = quant_chooser(image._data);
        std::cout << "wybrany kwantyzator=" << q << std::endl;
    }

    if (opts.program_mode == "encode") {
        output_data = encode(input_data, q);
    } else if (opts.program_mode == "decode") {
        output_data = decode(input_data, q);
    } else {
        throw std::runtime_error { "nie znany sposob dzialania programu!" + opts.program_mode + " podaj 'encode' lub 'decode'" };
    }
    uint64_t time = timer.measure();

    std::ofstream output_file { opts.tga_save_file_path };
    if (!output_file) {
        throw std::runtime_error { "nie mozna otworzyc pliku=" + opts.tga_save_file_path };
    }

    output_file << output_data;

    std::cout << "czas dzialania=" << time << "ms\n";
}