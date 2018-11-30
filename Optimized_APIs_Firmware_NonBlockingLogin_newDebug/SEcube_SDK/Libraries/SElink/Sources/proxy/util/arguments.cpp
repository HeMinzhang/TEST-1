/**
 *  \file arguments.cpp
 *  \author Nicola Ferri
 *  \brief Utilities for command line parsing
 */

#include "util/arguments.h"

#include <string>
#include <vector>

namespace selink { namespace util { namespace arguments {

    boost::program_options::basic_parsed_options<char> parse_command_line_utf8(
        int argc, const char** argv,
        const boost::program_options::options_description& desc)
    {
#ifdef _WIN32
        static std::vector<std::string> utf8_arguments;
        static std::vector<const char*> cstrings;

        if (utf8_arguments.empty()) {
            auto arguments = boost::program_options::split_winmain(GetCommandLineW());
            utf8_arguments.reserve(arguments.size());
            for (auto argument : arguments) {
                utf8_arguments.push_back(filesystem::utf16to8(argument));
            }
            cstrings.reserve(utf8_arguments.size() + 1);
            for (auto& argument : utf8_arguments) {
                cstrings.push_back(argument.c_str());
            }
            cstrings.push_back(NULL);
        }
        return boost::program_options::parse_command_line(
            static_cast<int>(utf8_arguments.size()),
            &cstrings[0],
            desc);
#else
        return boost::program_options::parse_command_line(argc, argv, desc);
#endif  // _WIN32
    }

} } }  // namespace selink::util::arguments
