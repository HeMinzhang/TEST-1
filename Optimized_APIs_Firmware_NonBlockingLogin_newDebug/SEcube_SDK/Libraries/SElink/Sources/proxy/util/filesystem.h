/**
 *  \file filesystem.h
 *  \author Nicola Ferri
 *  \brief Cross-platform path utilities
 */

#pragma once
#include "util/os.h"

#include <string>
#include <boost/filesystem.hpp>

#include "util/utf8/utf8.h"

namespace selink { namespace util { namespace filesystem {

    /** \brief Get the current program's absolute path
     *  \return absolute path of the executable
     */
    boost::filesystem::path get_exe_path();

    /** \brief Convert utf8 to utf16
     *  \param s the string to convert
     *  \return converted wide char string
     *  
     *  Converts a utf8 string to a utf16 wstring
     */
    std::wstring utf8to16(std::string s);
    
    /** \brief Convert utf8 to utf16
     *  \param s the wide char string to convert
     *  \return converted string
     *  
     *  Converts a utf16 wstring to a utf8 string
     */
    std::string utf16to8(std::wstring s);

    /** \brief Create a boost path object from utf8 string
     *  \param path utf8 string containing the path
     *  \return the boost path object
     *  
     *  use boost::filesystem::path::native() to get the path string
     *  in the correct format (utf16 for windows, utf8 for linux)
     */
    boost::filesystem::path path_from_utf8(std::string path);

    /** \brief Create a utf8 string from a boost path object
     *  \param path the boost path object
     *  \return utf8 string containing the path
     */
    std::string path_to_utf8(boost::filesystem::path path);

    /** \brief Comparable string representation of a path
     *  \param path a boost path object
     *  \return a utf8 string
     *  
     *  The resulting string is all lowercase in Windows,
     *  unchanged in linux.
     */
    std::string path_uniform_case(boost::filesystem::path path);

} } }  // namespace selink::util::filesystem
