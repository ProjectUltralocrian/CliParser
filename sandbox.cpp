#include "cliparser.h"
#include <iostream>
#include <cassert>

void do_stuff(const pul::CliParsedArgs &config)
{
    int counter = 1;

    for (const auto &arg : config.args)
    {
        std::cout << counter++ << ": \t";
        std::cout << arg << std::endl;
    }

    for (char c : config.flags)
    {
        std::cout << "Flag: " << c << std::endl;
    }

    for (auto [k, v] : config.flags_with_args)
    {
        std::cout << "KEY: " << k << ", VALUE: " << v << std::endl;
    }
}

int main(int argc, char *argv[])
{
    auto app =
        pul::AppBuilder("Cool app")
            .arg(pul::CliArg::make('n', "numberlines", "Adds line numbers.", true))
            .arg(pul::CliArg::make('i', "insensitive", "Case insensitive pattern matching", true, true))
            .usage("SANDBOX <args> flags...")
            .author("Tamás Polgár")
            .version("0.1.57")
            .build();

    auto [parse_result, parsed_args] = app.parse_args(argc, argv);

    switch (parse_result)
    {
    case pul::ParseResult::HelpRequested:
    {
        return 0;
        break;
    }
    case pul::ParseResult::VersionRequested:
    {
        return 0;
        break;
    }
    case pul::ParseResult::Ok:
    {
        do_stuff(parsed_args);
        return 0;
        break;
    }
    case pul::ParseResult::Error:
    {
        std::cerr << "Invalid command line arguments.\n";
        return 1;
        break;
    }
    default:
        assert(false);
    }
}