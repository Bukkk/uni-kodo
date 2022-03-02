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

    utils::time_it<std::chrono::milliseconds> timer {};

    auto const data = files::read(argv[1]);
    if (data.size() != 0) {

        algo::entropia_warunkowa(data);
        algo::entropia(data);
    }

    std::cout << "<info>: dlugosc: " << data.size() << "\n";
    std::cout << "<info>: program wykonal sie w " << timer.measure() << "ms";
}
