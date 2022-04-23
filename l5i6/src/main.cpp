
// #include "coding/acs.hpp"
#include "coding/lzw.hpp"
#include "coding/statistics.hpp"

#include "utils/args_helper.hpp"
#include "utils/time_it.hpp"
#include "utils/vector_streams.hpp"

#include <chrono>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>

// Program kodujący powinien dodatkowo na koniec zwrócić na ekran odpowiednio
// entropię kodowanych danych, średnią długość kodowania i stopień kompresji.

// Program dodatkowo podczas kodowania ma podawać długość kodowanego pliku, dłu-
// gość uzyskanego kodu, stopień kompresji, entropię kodowanego tekstu i entropię uzy-
// skanego kodu.

auto choose_encoding_algorithm(std::string name) -> std::function<std::vector<unsigned char>(std::vector<unsigned char> const&)>
{
    using namespace coding;
    if (name == "omega" || name == "") {
        return lzw::encode<natural::elias_omega>;
    } else if (name == "gamma") {
        return lzw::encode<natural::elias_gamma>;
    } else if (name == "delta") {
        return lzw::encode<natural::elias_delta>;
    } else if (name == "fib") {
        return lzw::encode<natural::fibonacci>;
    }

    throw std::runtime_error { "nieznane kodowanie: " + name };
}

auto choose_decoding_algorithm(std::string name) -> std::function<std::vector<unsigned char>(std::vector<unsigned char> const&)>
{
    using namespace coding;
    if (name == "omega" || name == "") {
        return lzw::decode<natural::elias_omega>;
    } else if (name == "gamma") {
        return lzw::decode<natural::elias_gamma>;
    } else if (name == "delta") {
        return lzw::decode<natural::elias_delta>;
    } else if (name == "fib") {
        return lzw::decode<natural::fibonacci>;
    }

    throw std::runtime_error { "nieznane kodowanie: " + name };
}

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    //     utils::args_helper args { {}, { "-h", "-d", "-e" },
    //         "program do adaptacyjnego kodowania arytmetycznego ze skalowaniem\n"
    //         "\n"
    //         "uzycie: nazwa <plik do odczytania> <plik do zapisu> <tryb dzialania>\n"
    //         "\n"
    //         "  <plik do odczytania>\n"
    //         "  <plik do zapisu>\n"
    //         "  <tryb dzialania>                        -> czy program ma kodowac czy dekodowac\n"
    //         "                                             e koduj\n"
    //         "                                             d dekoduj\n" };

    utils::args_helper args {
        "program do kodowania lzw kodowaniem uniwersalnym\n"
        "\n"
        "uzycie: program <plik do odczytania> <plik do zapisu> <tryb dzialania> [-a kodowanie] [-h help]\n"
        "\n"
        "\n"
        "  <plik do odczytania>\n"
        "  <plik do zapisu>\n"
        "  <tryb dzialania>                        -> czy program ma kodowac czy dekodowac\n"
        "                                             e koduj\n"
        "                                             d dekoduj\n"
        "  [-a kodowanie]                          -> jakie kodowanie naturalne skompresowanego pliku\n"
        "                                             omega - eliasa omega (domyslne)\n"
        "                                             gamma - eliasa gamma\n"
        "                                             delta - eliasa delta\n"
        "                                             fib - kodowanie fibonaciego\n"
    };

    std::string help {};
    args.set_optional({ .write_to = help, .symbol = "-h" });

    std::string input_filepath {};
    std::string output_filepath {};
    args.set_positional({ .write_to = input_filepath });
    args.set_positional({ .write_to = output_filepath });

    std::string mode {};
    args.set_positional({ .write_to = mode });

    std::string algorithm_name {};
    args.set_optional({ .write_to = algorithm_name, .symbol = "-a" });

    if (!args.parse(argc, argv)) {
        std::cout << args.help_page();
        return 0;
    }

    if (input_filepath == output_filepath) {
        std::cout << "plik zrodlowy i docelowy sa takie same\n";
        // latwo sie da oszukac ale w 80proc przypadkow zaradzi problem
        return 1;
    }

    using utils::vector_streams::binary::operator<<;
    using utils::vector_streams::binary::operator>>;

    //     // std::cout << input_data << '\n';
    //     // std::vector<bool> bv = vector_cast(input_data);
    //     // std::vector<unsigned char> cv = vector_cast(bv);
    //     // std::cout << cv << '\n';
    //     // std::cout << "org=" << input_data.size() << " bv=" << bv.size() << " cv=" << cv.size() << '\n';

    utils::time_it<std::chrono::milliseconds> timer {};

    if (mode == "e") {
        std::cout << "kodowanie\n";

        std::vector<unsigned char> input_data {};
        std::ifstream input_file { input_filepath };
        if (input_file) {
            input_file >> input_data;

            input_file.close();
        } else {
            std::cout << "nie mozna otworzyc pliku czytania\n";
            return 1;
        }

        timer.set();

        // using namespace coding;
        // auto output_data = lzw::encode<natural::elias_gamma>(input_data);

        auto encoding_algorithm = choose_encoding_algorithm(algorithm_name);
        auto output_data = encoding_algorithm(input_data);

        auto millis = timer.measure();

        std::cout << "uzyty algorytm kodowania naturalnego=" + algorithm_name + "\n";
        std::cout << "czas kodowania=" << millis << "ms\n";
        std::cout << "dlugosc pliku wejsiciowego=" << input_data.size() << '\n';
        std::cout << "entropia pliku wejsciowego=" << coding::statistics::entropy(input_data) << '\n';
        std::cout << "dlugosc pliku wyjsiciowego=" << output_data.size() << '\n';
        std::cout << "entropia pliku wyjsciowego=" << coding::statistics::entropy(output_data) << '\n';
        std::cout << "srednia dlugosc kodowania kodu=" << coding::statistics::average_coding_length(output_data.size(), input_data.size()) << '\n';
        std::cout << "stopien kompresji kodu=" << coding::statistics::compression_ratio(output_data.size(), input_data.size()) << '\n';

        std::ofstream output_file { output_filepath };
        if (output_file) {
            // uint64_t to_decode = input_data.size();
            // char buff[8];
            // for (uint32_t i {}; i < 8; ++i) {
            //     buff[i] = (to_decode >> i * 8) & 0xff;
            // }
            // output_file.write(buff, 8);

            output_file << output_data;

            output_file.close();
        } else {
            std::cout << "nie mozna otworzyc pliku zapisu\n";
            return 1;
        }

    } else if (mode == "d") {
        std::cout << "dekodowanie\n";

        // uint64_t to_decode {};
        std::vector<unsigned char> input_data {};
        std::ifstream input_file { input_filepath };
        if (input_file) {
            // char buff[8];
            // input_file.read(buff, 8);
            // for (uint32_t i {}; i < 8; ++i) {
            //     to_decode |= ((uint64_t)buff[i] & 0xff) << 8 * i;
            // }

            input_file >> input_data;

            input_file.close();
        } else {
            std::cout << "nie mozna otworzyc pliku czytania\n";
            return 1;
        }

        timer.set();

        // using namespace coding;
        // auto output_data = lzw::decode<natural::elias_gamma>(input_data);

        auto decoding_algorithm = choose_decoding_algorithm(algorithm_name);
        auto output_data = decoding_algorithm(input_data);

        auto millis = timer.measure();

        // //         std::cout << output_data << "<koniec pliku>---------------\n";

        std::cout << "algorytm kodowania naturalnego=" + algorithm_name + "\n";
        std::cout << "czas dekodownia=" << millis << "ms\n";
        std::cout << "dlugosc pliku wejsiciowego=" << input_data.size() << '\n';
        std::cout << "entropia pliku wejsciowego=" << coding::statistics::entropy(input_data) << '\n';
        std::cout << "dlugosc pliku wyjsiciowego=" << output_data.size() << '\n';
        std::cout << "entropia pliku wyjsciowego=" << coding::statistics::entropy(output_data) << '\n';
        std::cout << "srednia dlugosc kodowania kodu=" << coding::statistics::average_coding_length(input_data.size(), output_data.size()) << '\n';
        std::cout << "stopien kompresji kodu=" << coding::statistics::compression_ratio(input_data.size(), output_data.size()) << '\n';

        std::ofstream output_file { output_filepath };
        if (output_file) {
            output_file << output_data;

            output_file.close();
        } else {
            std::cout << "nie mozna otworzyc pliku zapisu\n";
            return 1;
        }
    } else {
        std::cout << "nie znany tryb dzialania\n";
        return 1;
    }
}
