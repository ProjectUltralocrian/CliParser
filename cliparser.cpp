#include "cliparser.h"
#include <format>
#include <algorithm>
#include <ranges>
#include <cctype>
#include <cstring>
#include <iostream>
#include <cassert>

using namespace std::ranges::views;

namespace pul
{
    void CliApp::panic(std::string_view msg)
    {
        std::cerr << msg << std::endl;
        print_usage();
        exit(1);
    }

    std::ostream &operator<<(std::ostream &stream, const CliArg &arg)
    {
        return stream << std::format("Short name: {}, long name: {}, required: {}, needs argument: {}", arg.short_name, arg.long_name, arg.required, arg.needs_arg);
    }

    std::vector<CliArg>::const_iterator CliApp::get_arg_from_config(std::string_view long_name) const
    {
        return std::ranges::find_if(m_args_config, [=](const auto &arg)
                                    { return arg.long_name == long_name; });
    }

    std::vector<CliArg>::const_iterator CliApp::get_arg_from_config(char short_name) const
    {
        return std::ranges::find_if(m_args_config, [=](const auto &arg)
                                    { return short_name == arg.short_name; });
    }

    void CliApp::print_help() const
    {
        std::cout << "\n**********************HELP***************************\n";
        print_usage();
        std::cout << "Author: " << m_author << std::endl;
        std::cout << "Version: " << m_version << std::endl;
        std::cout << "Options and flags\n";
        for (const auto &arg : m_args_config)
        {
            std::cout << std::format("-{0} --{1:25s}  {2:45s}\n", arg.short_name, arg.long_name + (arg.needs_arg ? " + <arg> " : ""), arg.description);
        }
        std::cout << "\n******************************************************\n";
    }

    void CliApp::print_version() const
    {
        std::cout << m_app_name << ", version: " << m_version << std::endl;
    }

    void CliApp::print_usage() const
    {
        std::cout << "USAGE: " << m_usage << std::endl;
    }

    std::tuple<ParseResult, CliParsedArgs> CliApp::parse_args(int argc, char **argv)
    {
        CliParsedArgs output;
        for (int i = 0; i < argc; ++i)
        {
            assert(argv[i] && argv[i][0]);
            if (argv[i][0] == '-')
            {
                bool short_arg = argv[i][1] != '-';
                if (std::isspace(argv[i][short_arg ? 1 : 2]) || std::strlen(argv[i]) < (short_arg ? 2 : 3))
                {
                    panic("Invalid flag.");
                }
                if (short_arg)
                {
                    auto curr_pos = argv[i];
                    for (size_t j = 1; curr_pos[j] != 0; ++j)
                    {
                        insert_arg_if_valid(argc, argv, i, output, curr_pos[j]);
                    }
                }
                else
                {
                    insert_arg_if_valid(argc, argv, i, output, argv[i] + 2);
                }
            }
            else
            {
                output.args.emplace_back(argv[i]);
            }
        }
        if (output.flags.contains('h'))
        {
            print_help();
            return {ParseResult::HelpRequested, output};
        }
        if (output.flags.contains('v'))
        {
            print_version();
            return {ParseResult::VersionRequested, output};
        }

        auto not_provided_mandatory = filter(m_args_config, [](const CliArg &arg)
                                             { return arg.required; }) |
                                      filter([&](const CliArg &arg)
                                             { return !output.flags.contains(arg.short_name) && !output.flags_with_args.contains(arg.short_name); });

#if DEBUG
        for (const auto &arg : output.args)
        {
            std::cout << arg << std::endl;
        }

        for (char c : output.flags)
        {
            std::cout << "Flag: " << c << std::endl;
        }
        for (auto [k, v] : output.flags_with_args)
        {
            std::cout << "KEY: " << k << ", VALUE: " << v << std::endl;
        }
#endif

        if (!not_provided_mandatory.empty())
        {
            for (auto c : not_provided_mandatory)
            {
                std::cerr << "Not provided: " << "-" << c.short_name << "/--" << c.long_name << std::endl;
            }
            return {ParseResult::Error, output};
        }
        return {ParseResult::Ok, output};
    }

    AppBuilder::AppBuilder(const std::string &name) : m_app_name{name}
    {
        m_app.m_app_name = std::move(m_app_name);
        m_app.m_args_config.emplace_back(CliArg::make('h', "help", "Prints help information."));
        m_app.m_args_config.emplace_back(CliArg::make('v', "version", "Prints version of app."));
        m_app.m_version = "0.0.1";
        m_app.m_author = "Author Name";
    }
    AppBuilder &AppBuilder::arg(const CliArg &arg)
    {
        m_app.m_args_config.push_back(std::move(arg));
        return *this;
    }
    AppBuilder &AppBuilder::author(const char *author)
    {
        m_app.m_author = author;
        return *this;
    }
    AppBuilder &AppBuilder::version(const char *version)
    {
        m_app.m_version = version;
        return *this;
    }
    AppBuilder &AppBuilder::usage(const std::string &usage)
    {
        m_app.m_usage = std::move(usage);
        return *this;
    }

} // namespace pul