#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <format>
#include <set>

namespace pul
{
    class CliArg
    {
    public:
        static CliArg make(char short_name, const std::string &long_name, const char *description, bool required = false, bool needs_arg = false);
        const char short_name;
        const std::string long_name;
        const std::string description;
        const bool required;
        const bool needs_arg;

    private:
        CliArg(char short_name, const std::string &long_name, const char *description, bool required = false, bool needs_arg = false);
    };

    std::ostream &operator<<(std::ostream &stream, const CliArg &arg);

    class CliParsedArgs
    {
    public:
        // private:
        std::unordered_map<char, std::string> m_flags_with_args;
        std::set<char> m_flags;
        std::vector<std::string> m_args;
    };

    class CliApp
    {
    public:
        const std::vector<CliArg> &get_args_config() const
        {
            return m_args_config;
        }
        std::string_view app_name() const
        {
            return m_app_name;
        }
        void print_usage() const
        {
            std::cout << "USAGE: " << m_usage << std::endl;
        }

        void print_help() const;
        void print_version() const;
        CliParsedArgs parse_args(int argc, char **argv);

        friend class AppBuilder;

    private:
        std::string m_app_name;
        std::string m_usage;
        std::vector<CliArg> m_args_config;
        std::vector<CliParsedArgs> m_parsed_args;
        std::string m_version;
        std::string m_author;

        void panic(std::string_view msg);
        std::vector<CliArg>::const_iterator get_arg_from_config(std::string_view long_name) const;
        std::vector<CliArg>::const_iterator get_arg_from_config(char short_name) const;

        template <typename T>
        bool insert_arg_if_valid(int argc, char **argv, int i, CliParsedArgs &output, const T &name)
        {

            std::vector<CliArg>::const_iterator config_arg = get_arg_from_config(name);
            if (config_arg != m_args_config.end())
            {
                if (config_arg->needs_arg)
                {
                    if ((i + 1) >= argc || argv[i + 1][1] == '-')
                    {
                        panic(std::format("Missing mandatory argument for {}", name));
                    }
                    output.m_flags_with_args[config_arg->short_name] = argv[i + 1];
                    i++;
                    return true;
                }
                else
                {
                    output.m_flags.insert(config_arg->short_name);
                    return true;
                }
            }
            else
            {
                panic(std::format("Invalid cli flag: {}", name));
            }
            return false;
        }
    };

    class AppBuilder
    {
    public:
        AppBuilder(const std::string &name);
        AppBuilder &arg(const CliArg &arg);
        AppBuilder &author(const char *author);
        AppBuilder &version(const char *version);
        AppBuilder &usage(const std::string &usage);
        CliApp build()
        {
            return m_app;
        }

    private:
        const std::string m_app_name;
        CliApp m_app;
    };
}
#endif // CLI_PARSER_H