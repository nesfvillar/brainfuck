#include "brainfuck.hpp"

#include <fstream>
#include <iostream>
#include <ranges>

using namespace bf;

enum class Exit
{
    ArgcError = 1,
    IfstreamError = 2,
};

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::clog << "Error: need a program to execute." << std::endl;
        std::exit(static_cast<int>(Exit::ArgcError));
    }

    std::ifstream ifstream{ argv[1] };
    if (!ifstream)
    {
        std::clog << "Error: could not open program." << std::endl;
        std::exit(static_cast<int>(Exit::IfstreamError));
    }

    std::string const program =
        std::views::istream<char>(ifstream) | std::ranges::to<std::string>();

    Brainfuck bf{ program };
    bf.run();
}
