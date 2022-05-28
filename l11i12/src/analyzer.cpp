#include "coding/graphics_statistics.hpp"
#include "coding/statistics.hpp"

#include <iostream>

struct accessors {
    tga::accessor_MONO r;
    tga::accessor_MONO g;
    tga::accessor_MONO b;
    tga::accessor_RGB rgb;
};

void print_statistics(accessors const& orginal, accessors const& differential)
{
    using namespace coding;

    auto mse = statistics::mse(orginal.rgb._image, differential.rgb._image);
    auto mse_r = statistics::mse(orginal.r._image, differential.r._image);
    auto mse_g = statistics::mse(orginal.g._image, differential.g._image);
    auto mse_b = statistics::mse(orginal.b._image, differential.b._image);

    auto snr = statistics::snr(differential.rgb._image, mse);
    auto snr_r = statistics::snr(differential.r._image, mse_r);
    auto snr_g = statistics::snr(differential.g._image, mse_g);
    auto snr_b = statistics::snr(differential.b._image, mse_b);

    std::cout << "entropia pliku wejsciowego    =" << statistics::entropy(orginal.rgb._image) << std::endl;
    std::cout << "entropia pliku wejsciowego (R)=" << statistics::entropy(orginal.r._image) << std::endl;
    std::cout << "entropia pliku wejsciowego (G)=" << statistics::entropy(orginal.g._image) << std::endl;
    std::cout << "entropia pliku wejsciowego (B)=" << statistics::entropy(orginal.b._image) << std::endl;

    std::cout << "mse    =" << mse << std::endl;
    std::cout << "mse (R)=" << mse_r << std::endl;
    std::cout << "mse (G)=" << mse_g << std::endl;
    std::cout << "mse (B)=" << mse_b << std::endl;

    std::cout << "snr    =" << snr << " (" << statistics::to_decibels(snr) << "dB)" << std::endl;
    std::cout << "snr (R)=" << snr_r << " (" << statistics::to_decibels(snr_r) << "dB)" << std::endl;
    std::cout << "snr (G)=" << snr_g << " (" << statistics::to_decibels(snr_g) << "dB)" << std::endl;
    std::cout << "snr (B)=" << snr_b << " (" << statistics::to_decibels(snr_b) << "dB)" << std::endl;

    std::cout << "entropia pliku wyjsciowego    =" << statistics::entropy(differential.rgb._image) << std::endl;
    std::cout << "entropia pliku wyjsciowego (B)=" << statistics::entropy(differential.b._image) << std::endl;
    std::cout << "entropia pliku wyjsciowego (R)=" << statistics::entropy(differential.r._image) << std::endl;
    std::cout << "entropia pliku wyjsciowego (G)=" << statistics::entropy(differential.g._image) << std::endl;
}

#include "utils/args_helper.hpp"
#include "utils/vector_streams.hpp"

#include <fstream>

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    std::string orginal_path {};
    std::string decoded_path {};

    utils::args_helper parser {
        "kodowanie rownomierne i zapis roznicowy - program analizyjacy wyniki\n"
        "\n"
        "uzycie: program <orginalny plik tga> <zdekodowany plik tga>\n"
    };

    parser.set_positional({ .write_to = orginal_path });
    parser.set_positional({ .write_to = decoded_path });

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


    std::vector<unsigned char> orginal_data {};
    std::ifstream orginal_file { orginal_path };
    if (!orginal_file) {
        throw std::runtime_error { "nie mozna otworzyc pliku=" + decoded_path };
    }

    orginal_file >> orginal_data;

    tga::image orginal_image {};
    orginal_image.from_binary(orginal_data);

    auto [orginal_r, orginal_g, orginal_b] = tga::split_channels(orginal_image._data);


    std::vector<unsigned char> decoded_data {};
    std::ifstream decoded_file { decoded_path };
    if (!decoded_file) {
        throw std::runtime_error { "nie mozna otworzyc pliku=" + decoded_path };
    }

    decoded_file >> decoded_data;

    tga::image decoded_image {};
    decoded_image.from_binary(decoded_data);

    auto [decoded_r, decoded_g, decoded_b] = tga::split_channels(decoded_image._data);


    print_statistics({
        tga::accessor_MONO {orginal_r, orginal_image._width, orginal_image._height},
        tga::accessor_MONO {orginal_g, orginal_image._width, orginal_image._height},
        tga::accessor_MONO {orginal_b, orginal_image._width, orginal_image._height},
        tga::accessor_RGB {orginal_image._data, orginal_image._width, orginal_image._height}
    }, {
        tga::accessor_MONO {decoded_r, decoded_image._width, decoded_image._height},
        tga::accessor_MONO {decoded_g, decoded_image._width, decoded_image._height},
        tga::accessor_MONO {decoded_b, decoded_image._width, decoded_image._height},
        tga::accessor_RGB {decoded_image._data, decoded_image._width, decoded_image._height}
    });
}