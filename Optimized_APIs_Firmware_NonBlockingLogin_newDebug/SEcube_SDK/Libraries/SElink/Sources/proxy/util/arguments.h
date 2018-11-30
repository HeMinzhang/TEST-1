/**
 *  \file arguments.h
 *  \author Nicola Ferri
 *  \brief Utilities for command line parsing
 */

#pragma once
#include "util/os.h"

#include <iostream>
#include <boost/program_options.hpp>

#include "util/filesystem.h"

namespace selink { namespace util { namespace arguments {

    /** \brief Get the command line as utf8 and parse it
     *  \param argc from main
     *  \param argv from main
     *  \param desc boost options description
     *  \return boost parsed options for use in boost::program_options::store
     *  
     *  A wrapper for boost::program_options::parse_command_line. On Windows it
     *  gets the wide char command line and converts it to utf8 before parsing 
     *  (argc, argv are ignored). On linux it parses the command line from argc, argv.
     */
    boost::program_options::basic_parsed_options<char>  parse_command_line_utf8(
        int argc, const char** argv,
        const boost::program_options::options_description& desc);

} } } // namespace selink::util::arguments