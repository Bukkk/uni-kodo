#include <chrono>
#include <iostream>
#include <vector>

#include "files.hpp"
#include "time_it.hpp"
#include "algo.hpp"

int main(int argc, char** argv)
{
    std::ios::sync_with_stdio(false);

    if (argc != 2) {
        std::cerr << "podaj plik jako argument!\n";
        return 1;
    };

    auto const data = files::read_binary<unsigned char>(argv[1]);

    utils::time_it<std::chrono::milliseconds> timer {};
    if (data.size() != 0) {

        algo::conditional_entropy(data);
        algo::entropy(data);
    }

    std::cout << "<info>: dlugosc: " << data.size() << "\n";
    std::cout << "<info>: program liczyl sie w " << timer.measure() << "ms";
}
