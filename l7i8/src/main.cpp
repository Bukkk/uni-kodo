#include "coding/statistics.hpp"
#include "jpg_coders.hpp"
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

    tga::image image { data };
    std::cout << image._header << std::endl;

    if (image._image_format != tga::image_format::RGB) {
        throw std::runtime_error { "obrazek nie jest w formacie rgb" };
    }

    tga::accessor_RGB accessor { image._data, image._width, image._height };
    auto [r_vals, g_vals, b_vals] = tga::split_channels(accessor);

    tga::accessor_MONO accessor_r { r_vals, image._width, image._height };
    tga::accessor_MONO accessor_g { g_vals, image._width, image._height };
    tga::accessor_MONO accessor_b { b_vals, image._width, image._height };

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

    using namespace coding;
    std::cout << "entropia pliku=" << statistics::entropy(accessor._image) << std::endl;
    std::cout << "entropia pliku (R)=" << statistics::entropy(accessor_r._image) << std::endl;
    std::cout << "entropia pliku (G)=" << statistics::entropy(accessor_g._image) << std::endl;
    std::cout << "entropia pliku (B)=" << statistics::entropy(accessor_b._image) << std::endl;

    auto r_with_pred_1 = jpg_predictors::encode<jpg_predictors::predictor_1>(accessor_r);
    auto r_with_pred_2 = jpg_predictors::encode<jpg_predictors::predictor_2>(accessor_r);
    auto r_with_pred_3 = jpg_predictors::encode<jpg_predictors::predictor_3>(accessor_r);
    auto r_with_pred_4 = jpg_predictors::encode<jpg_predictors::predictor_4>(accessor_r);
    auto r_with_pred_5 = jpg_predictors::encode<jpg_predictors::predictor_5>(accessor_r);
    auto r_with_pred_6 = jpg_predictors::encode<jpg_predictors::predictor_6>(accessor_r);
    auto r_with_pred_7 = jpg_predictors::encode<jpg_predictors::predictor_7>(accessor_r);
    auto r_with_pred_new = jpg_predictors::encode<jpg_predictors::predictor_new>(accessor_r);

    auto g_with_pred_1 = jpg_predictors::encode<jpg_predictors::predictor_1>(accessor_g);
    auto g_with_pred_2 = jpg_predictors::encode<jpg_predictors::predictor_2>(accessor_g);
    auto g_with_pred_3 = jpg_predictors::encode<jpg_predictors::predictor_3>(accessor_g);
    auto g_with_pred_4 = jpg_predictors::encode<jpg_predictors::predictor_4>(accessor_g);
    auto g_with_pred_5 = jpg_predictors::encode<jpg_predictors::predictor_5>(accessor_g);
    auto g_with_pred_6 = jpg_predictors::encode<jpg_predictors::predictor_6>(accessor_g);
    auto g_with_pred_7 = jpg_predictors::encode<jpg_predictors::predictor_7>(accessor_g);
    auto g_with_pred_new = jpg_predictors::encode<jpg_predictors::predictor_new>(accessor_g);

    auto b_with_pred_1 = jpg_predictors::encode<jpg_predictors::predictor_1>(accessor_b);
    auto b_with_pred_2 = jpg_predictors::encode<jpg_predictors::predictor_2>(accessor_b);
    auto b_with_pred_3 = jpg_predictors::encode<jpg_predictors::predictor_3>(accessor_b);
    auto b_with_pred_4 = jpg_predictors::encode<jpg_predictors::predictor_4>(accessor_b);
    auto b_with_pred_5 = jpg_predictors::encode<jpg_predictors::predictor_5>(accessor_b);
    auto b_with_pred_6 = jpg_predictors::encode<jpg_predictors::predictor_6>(accessor_b);
    auto b_with_pred_7 = jpg_predictors::encode<jpg_predictors::predictor_7>(accessor_b);
    auto b_with_pred_new = jpg_predictors::encode<jpg_predictors::predictor_new>(accessor_b);

    std::array<double, 8> r_entropies{
        statistics::entropy(r_with_pred_1),
        statistics::entropy(r_with_pred_2),
        statistics::entropy(r_with_pred_3),
        statistics::entropy(r_with_pred_4),
        statistics::entropy(r_with_pred_5),
        statistics::entropy(r_with_pred_6),
        statistics::entropy(r_with_pred_7),
        statistics::entropy(r_with_pred_new)
    };

    std::cout << std::endl;
    std::cout << "entropia (R) dla predyktora 1=" << r_entropies[0] << "\n";
    std::cout << "entropia (R) dla predyktora 2=" << r_entropies[1] << "\n";
    std::cout << "entropia (R) dla predyktora 3=" << r_entropies[2] << "\n";
    std::cout << "entropia (R) dla predyktora 4=" << r_entropies[3] << "\n";
    std::cout << "entropia (R) dla predyktora 5=" << r_entropies[4] << "\n";
    std::cout << "entropia (R) dla predyktora 6=" << r_entropies[5] << "\n";
    std::cout << "entropia (R) dla predyktora 7=" << r_entropies[6] << "\n";
    std::cout << "entropia (R) dla predyktora new=" << r_entropies[7] << "\n";
    std::cout << "najmniejsza entropia zachodzi dla predyktora " 
        << std::distance(r_entropies.begin(), std::min_element(r_entropies.begin(), r_entropies.end())) + 1 << std::endl;

    std::array<double, 8> g_entropies{
        statistics::entropy(g_with_pred_1),
        statistics::entropy(g_with_pred_2),
        statistics::entropy(g_with_pred_3),
        statistics::entropy(g_with_pred_4),
        statistics::entropy(g_with_pred_5),
        statistics::entropy(g_with_pred_6),
        statistics::entropy(g_with_pred_7),
        statistics::entropy(g_with_pred_new)
    };

    std::cout << std::endl;
    std::cout << "entropia (G) dla predyktora 1=" << g_entropies[0] << "\n";
    std::cout << "entropia (G) dla predyktora 2=" << g_entropies[1] << "\n";
    std::cout << "entropia (G) dla predyktora 3=" << g_entropies[2] << "\n";
    std::cout << "entropia (G) dla predyktora 4=" << g_entropies[3] << "\n";
    std::cout << "entropia (G) dla predyktora 5=" << g_entropies[4] << "\n";
    std::cout << "entropia (G) dla predyktora 6=" << g_entropies[5] << "\n";
    std::cout << "entropia (G) dla predyktora 7=" << g_entropies[6] << "\n";
    std::cout << "entropia (G) dla predyktora new=" << g_entropies[7] << "\n";
    std::cout << "najmniejsza entropia zachodzi dla predyktora " 
        << std::distance(g_entropies.begin(), std::min_element(g_entropies.begin(), g_entropies.end())) + 1 << std::endl;

    std::array<double, 8> b_entropies{
        statistics::entropy(b_with_pred_1),
        statistics::entropy(b_with_pred_2),
        statistics::entropy(b_with_pred_3),
        statistics::entropy(b_with_pred_4),
        statistics::entropy(b_with_pred_5),
        statistics::entropy(b_with_pred_6),
        statistics::entropy(b_with_pred_7),
        statistics::entropy(b_with_pred_new)
    };

    std::cout << std::endl;
    std::cout << "entropia (B) dla predyktora 1=" << b_entropies[0] << "\n";
    std::cout << "entropia (B) dla predyktora 2=" << b_entropies[1] << "\n";
    std::cout << "entropia (B) dla predyktora 3=" << b_entropies[2] << "\n";
    std::cout << "entropia (B) dla predyktora 4=" << b_entropies[3] << "\n";
    std::cout << "entropia (B) dla predyktora 5=" << b_entropies[4] << "\n";
    std::cout << "entropia (B) dla predyktora 6=" << b_entropies[5] << "\n";
    std::cout << "entropia (B) dla predyktora 7=" << b_entropies[6] << "\n";
    std::cout << "entropia (B) dla predyktora new=" << b_entropies[7] << "\n";
    std::cout << "najmniejsza entropia zachodzi dla predyktora " 
        << std::distance(b_entropies.begin(), std::min_element(b_entropies.begin(), b_entropies.end())) + 1 << std::endl;

    auto with_pred_1 = tga::join_channels(
        tga::accessor_MONO {r_with_pred_1, image._width, image._height}, 
        tga::accessor_MONO {g_with_pred_1, image._width, image._height},
        tga::accessor_MONO {b_with_pred_1, image._width, image._height}
    );
    auto with_pred_2 = tga::join_channels(
        tga::accessor_MONO {r_with_pred_2, image._width, image._height}, 
        tga::accessor_MONO {g_with_pred_2, image._width, image._height},
        tga::accessor_MONO {b_with_pred_2, image._width, image._height}
    );
    auto with_pred_3 = tga::join_channels(
        tga::accessor_MONO {r_with_pred_3, image._width, image._height}, 
        tga::accessor_MONO {g_with_pred_3, image._width, image._height},
        tga::accessor_MONO {b_with_pred_3, image._width, image._height}
    );
    auto with_pred_4 = tga::join_channels(
        tga::accessor_MONO {r_with_pred_4, image._width, image._height}, 
        tga::accessor_MONO {g_with_pred_4, image._width, image._height},
        tga::accessor_MONO {b_with_pred_4, image._width, image._height}
    );
    auto with_pred_5 = tga::join_channels(
        tga::accessor_MONO {r_with_pred_5, image._width, image._height}, 
        tga::accessor_MONO {g_with_pred_5, image._width, image._height},
        tga::accessor_MONO {b_with_pred_5, image._width, image._height}
    );
    auto with_pred_6 = tga::join_channels(
        tga::accessor_MONO {r_with_pred_6, image._width, image._height}, 
        tga::accessor_MONO {g_with_pred_6, image._width, image._height},
        tga::accessor_MONO {b_with_pred_6, image._width, image._height}
    );
    auto with_pred_7 = tga::join_channels(
        tga::accessor_MONO {r_with_pred_7, image._width, image._height}, 
        tga::accessor_MONO {g_with_pred_7, image._width, image._height},
        tga::accessor_MONO {b_with_pred_7, image._width, image._height}
    );
    auto with_pred_new = tga::join_channels(
        tga::accessor_MONO {r_with_pred_new, image._width, image._height}, 
        tga::accessor_MONO {g_with_pred_new, image._width, image._height},
        tga::accessor_MONO {b_with_pred_new, image._width, image._height}
    );

    std::array<double, 8> entropies{
        statistics::entropy(with_pred_1),
        statistics::entropy(with_pred_2),
        statistics::entropy(with_pred_3),
        statistics::entropy(with_pred_4),
        statistics::entropy(with_pred_5),
        statistics::entropy(with_pred_6),
        statistics::entropy(with_pred_7),
        statistics::entropy(with_pred_new)
    };

    std::cout << std::endl;
    std::cout << "entropia (RGB) dla predyktora 1=" << entropies[0] << "\n";
    std::cout << "entropia (RGB) dla predyktora 2=" << entropies[1] << "\n";
    std::cout << "entropia (RGB) dla predyktora 3=" << entropies[2] << "\n";
    std::cout << "entropia (RGB) dla predyktora 4=" << entropies[3] << "\n";
    std::cout << "entropia (RGB) dla predyktora 5=" << entropies[4] << "\n";
    std::cout << "entropia (RGB) dla predyktora 6=" << entropies[5] << "\n";
    std::cout << "entropia (RGB) dla predyktora 7=" << entropies[6] << "\n";
    std::cout << "entropia (RGB) dla predyktora new=" << entropies[7] << "\n";
    std::cout << "najmniejsza entropia zachodzi dla predyktora " 
        << std::distance(entropies.begin(), std::min_element(entropies.begin(), entropies.end())) + 1 << std::endl;
    
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