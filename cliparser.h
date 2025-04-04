#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <format>
#include <set>


/**
 * @namespace pul
 * Namespace for the CLI parser library.
 */
namespace pul
{
    enum class ParseResult
    {
        Ok,
        HelpRequested,
        VersionRequested,
        Error
    };


    /**
     * @class CliArg
     * @brief Represents a command line argument.
     */
    class CliArg
    {
    private:
        /**
         * @brief Constructor for CliArg.
         * @param short_name The short name of the argument.
         * @param long_name The long name of the argument.
         * @param description Description of the argument.
         * @param required Whether the argument is required.
         * @param needs_arg Whether the argument requires a value.
         */
        constexpr CliArg(char short_name, const std::string &long_name, const char *description, bool required, bool needs_arg)
            : short_name{short_name}, long_name{long_name}, description{description}, required{required}, needs_arg{needs_arg} {}

    public:
        /**
         * @brief Factory method to create a CliArg.
         * @param short_name The short name of the argument.
         * @param long_name The long name of the argument.
         * @param description Description of the argument.
         * @param required Whether the argument is required.
         * @param needs_arg Whether the argument requires a value.
         * @return A CliArg instance.
         */        
        constexpr static CliArg make(char short_name, const std::string &long_name, const char *description, bool required = false, bool needs_arg = false)
        {
            CliArg output(short_name, long_name, description, required, needs_arg);
            return output;
        }
        const char short_name; ///< Short name of the argument.
        const std::string long_name; ///< Long name of the argument.
        const std::string description; ///< Description of the argument.
        const bool required; ///< Whether the argument is required.
        const bool needs_arg; ///< Whether the argument requires a value.
    };

    /**
     * @brief Overloaded stream insertion operator for CliArg.
     * @param stream The output stream.
     * @param arg The CliArg instance.
     * @return The output stream.
     */
    std::ostream &operator<<(std::ostream &stream, const CliArg &arg);

    /**
     * @class CliParsedArgs
     * @brief Represents the parsed command line arguments.
     */
    class CliParsedArgs
    {
    public:
        /**
         * @brief Gets the flags with arguments.
         * @return Map of flags with arguments.
         */
        const std::unordered_map<char, std::string>& flags_with_args() const 
        {
            return m_flags_with_args;
        }

        /**
         * @brief Gets the flags that have been set.
         * @return Set of flags.
         */
        const std::set<char>& flags() const
        {
            return m_flags;
        }

        /**
         * @brief Gets the positional arguments.
         * @return Vector of positional arguments.
         */
        const std::vector<std::string>& positional_args() const
        {
            return m_pos_args;
        }

        friend class CliApp; 
    private:
        std::unordered_map<char, std::string> m_flags_with_args; ///< Flags with associated arguments.
        std::set<char> m_flags; ///< Set of flags.
        std::vector<std::string> m_pos_args; ///< List of positional arguments.
    };

    /**
     * @class CliApp
     * @brief Represents the command line application.
     */
    class CliApp
    {
    public:
        /**
         * @brief Gets the argument configuration.
         * @return Vector of CliArg.
         */
        const std::vector<CliArg> &get_args_config() const
        {
            return m_args_config;
        }

        /**
         * @brief Gets the application name.
         * @return Application name as a string view.
         */
        std::string_view app_name() const
        {
            return m_app_name;
        }

        /**
         * @brief Gets the number of mandatory positional arguments of the application.
         * @return Number of mandatory positional arguments.
         */
        size_t num_mandatory_pos_args() const
        {
            return m_num_mandatory_pos_args;
        }

        /**
         * @brief Gets the parsed cli arguments of the application.
         * @return The CliParsedArgs data structure.
         */
        const CliParsedArgs &parsed_args() const
        {
            return m_parsed_args;
        }

        /**
         * @brief Prints the usage information.
         */
        void print_usage() const;

        /**
         * @brief Prints the help information.
         */
        void print_help() const;
        
         /**
         * @brief Prints the version information.
         */       
        void print_version() const;
        
         /**
         * @brief Parses command line arguments.
         * @param argc Argument count.
         * @param argv Argument vector.
         * @return Tuple containing CliParsedArgs and ParseResult.
         */       
        std::tuple<ParseResult, CliParsedArgs> parse_args(int argc, char **argv);


        bool operator()(char c) const;

        friend class AppBuilder; ///< Allows AppBuilder to access private members of CliApp.

    private:
        std::string m_app_name; ///< Application name.
        std::string m_usage; ///< Usage information.
        std::vector<CliArg> m_args_config; ///< Argument configuration.
        CliParsedArgs m_parsed_args; ///< Parsed arguments.
        std::string m_version; ///< Application version.
        std::string m_author; ///< Application author.
        size_t m_num_mandatory_pos_args; ///< Number of mandatory positional arguments.

        /**
         * @brief Prints an error message and exits the application.
         * @param msg Error message.
         */
        void panic(std::string_view msg);

        /**
         * @brief Gets the argument configuration for a long name.
         * @param long_name Long name of the argument.
         * @return Iterator to the argument configuration.
         */
        std::vector<CliArg>::const_iterator get_arg_from_config(std::string_view long_name) const;

        /**
         * @brief Gets the argument configuration for a short name.
         * @param short_name Short name of the argument.
         * @return Iterator to the argument configuration.
         */
        std::vector<CliArg>::const_iterator get_arg_from_config(char short_name) const;


        /**
         * @brief Inserts an argument if valid.
         * @tparam T Type of the argument name (char or string).
         * @param argc Argument count.
         * @param argv Argument vector.
         * @param i Current index in the argument vector.
         * @param output Parsed arguments output.
         * @param name Argument name.
         * @return True if the argument was inserted, false otherwise.
         */
        template <typename T>
        void insert_arg_if_valid(int argc, char **argv, int &current_pos, CliParsedArgs &output, const T &name)
        {
            std::vector<CliArg>::const_iterator config_arg = get_arg_from_config(name);
            if (config_arg != m_args_config.end())
            {
                if (config_arg->needs_arg)
                {
                    if ((current_pos + 1) >= argc || argv[current_pos + 1][1] == '-')
                    {
                        panic(std::format("Missing mandatory argument for {}", name));
                    }
                    output.m_flags_with_args[config_arg->short_name] = argv[current_pos + 1];
                    current_pos++;
                }
                else
                {
                    output.m_flags.insert(config_arg->short_name);
                }
            }
            else
            {
                panic(std::format("Invalid cli flag: {}", name));
            }
        }
    };

    /**
     * @class AppBuilder
     * @brief Builder class for constructing a CliApp.
     */
    class AppBuilder
    {
    public:
        /**
         * @brief Constructor for AppBuilder.
         * @param name Application name.
         */
        AppBuilder(const std::string &name);
        
        /**
         * @brief Adds an argument to the application.
         * @param arg CliArg instance.
         * @return Reference to the AppBuilder.
         */ 
        AppBuilder &arg(const CliArg &arg);

        /**
         * @brief Sets the author of the application.
         * @param author Author name.
         * @return Reference to the AppBuilder.
         */
        AppBuilder &author(const char *author);

        /**
         * @brief Sets the version of the application.
         * @param version Version string.
         * @return Reference to the AppBuilder.
         */
        AppBuilder &version(const char *version = "0.0.1");

        /**
         * @brief Sets the usage information of the application.
         * @param usage Usage string.
         * @return Reference to the AppBuilder.
         */
        AppBuilder &usage(const std::string &usage);

        /**
         * @brief Sets the number of mandatory posistional arguments of the application.
         * @param num Number of mandatory positional arguments.
         * @return Reference to the AppBuilder.
         */
        AppBuilder &num_mandatory_pos_args(size_t num);

         /**
         * @brief Builds the CliApp instance.
         * @return CliApp instance.
         */
        CliApp build() const
        {
            return m_app;
        }

    private:
        const std::string m_app_name; ///< Application name.
        CliApp m_app; ///< CliApp instance being built.
    };
}

#endif // CLI_PARSER_H