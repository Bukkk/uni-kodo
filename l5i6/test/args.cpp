#include <args_helper.hpp>
#include <iostream>
#include <string>

int main(int argc, char** argv) {

    auto args = utils::args_helper("<HELP>");

    std::string pos1 {};
    std::string pos2 {};
    args.set_positional({.write_to=pos1});
    args.set_positional({.write_to=pos2});

    if (!args.parse(argc, argv)) {
        std::cout << "podaj argumenty pozycyjne!\n";
        return 1;
    }

    std::cout << "pos1=" << pos1 << " pos2=" << pos2 << "\n";
}