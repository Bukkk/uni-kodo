#include "acs/acs.hpp"
#include "utils/stream_vector.hpp"
#include <fstream>
#include <string>
#include <utils/args_helper.hpp>
#include <utils/time_it.hpp>

#include <iostream>

// Program kodujący powinien dodatkowo na koniec zwrócić na ekran odpowiednio
// entropię kodowanych danych, średnią długość kodowania i stopień kompresji.

int main(int argc, char** argv)
{

    utils::args_helper args { {}, { "-h", "-d", "-e" },
        "program do adaptacyjnego kodowania arytmetycznego ze skalowaniem\n"
        "\n"
        "uzycie: nazwa <plik do odczytania> <plik do zapisu> <tryb dzialania>\n"
        "\n"
        "  <plik do odczytania>\n"
        "  <plik do zapisu>\n"
        "  <tryb dzialania>                        -> czy program ma kodowac czy dekodowac\n"
        "                                             e koduj\n"
        "                                             d dekoduj\n" };

    args.parse(argc, argv);

    if (args.boolean("-h")) {
        std::cout << args.help() << '\n';
        return 0;
    }

    if (args.positional_args().size() < 3) {
        std::cout << "musisz podac argumenty pozycyjne <plik do odczytania> <plik do zapisu> <e/d>\n";
        return 1;
    }

    auto& input_filepath = args.positional_args()[0];
    auto& output_filepath = args.positional_args()[1];
    auto& mode = args.positional_args()[2];

    if (input_filepath == output_filepath) {
        std::cout << "plik zrodlowy i docelowy sa takie same\n";
        // latwo sie da oszukac ale w 80proc przypadkow zaradzi problem
        return 1;
    }

    std::ifstream input_file { input_filepath };
    if (!input_file) {
        std::cout << "nie mozna otworzyc pliku czytania\n";
        return 1;
    }

    std::vector<unsigned char> input_data {};
    {
        using utils::vector_stream_operators::binary::operator>>;
        input_file >> input_data;
    }

    using utils::vector_stream_operators::binary::operator<<;
    // std::cout << input_data << '\n';
    // std::vector<bool> bv = vector_cast(input_data);
    // std::vector<unsigned char> cv = vector_cast(bv);
    // std::cout << cv << '\n';
    // std::cout << "org=" << input_data.size() << " bv=" << bv.size() << " cv=" << cv.size() << '\n';

    std::vector<unsigned char> output_data {};

    if (mode == "e") {
        std::cout << "koduje\n";
        acs::encode(input_data, output_data);

    } else if (mode == "d") {
        std::cout << "dekoduje\n";
        
        if (args.positional_args().size() < 4) {
            std::cout << "podaj ilosc znakow do zdekodowania\n";
            return 1;
        }

        auto amount_to_decode = atoll(args.positional_args()[3].c_str());
        acs::decode(input_data, output_data, amount_to_decode);
    } else {
        std::cout << "nie znany tryb dzialania\n";
        return 1;
    }

    std::cout << output_data;
    std::cout << "plik wejsciowy size=" << input_data.size() << " bajtow" << '\n';

    std::ofstream output_file { output_filepath };
    if (!output_file) {
        std::cout << "nie mozna otworzyc pliku zapisu\n";
        return 1;
    }
    {
        using utils::vector_stream_operators::binary::operator<<;
        output_file << output_data;
    }
}