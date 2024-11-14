#include "cliparser.h"
#include <iostream>

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

#if DEBUG
    std::cout << app.app_name() << std::endl;

    for (const pul::CliArg &arg : app.get_args_config())
    {
        std::cout << arg << std::endl;
    }
#endif

    auto parsed_args = app.parse_args(argc, argv);

    for (const auto &arg : parsed_args.args)
    {
        std::cout << arg << std::endl;
    }

    for (char c : parsed_args.flags)
    {
        std::cout << "Flag: " << c << std::endl;
    }

    for (auto [k, v] : parsed_args.flags_with_args)
    {
        std::cout << "KEY: " << k << ", VALUE: " << v << std::endl;
    }
}