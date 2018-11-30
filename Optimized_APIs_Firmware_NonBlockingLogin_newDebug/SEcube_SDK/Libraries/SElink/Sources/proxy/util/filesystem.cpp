/**
 *  \file filesystem.cpp
 *  \author Nicola Ferri
 *  \brief Cross-platform path utilities
 */

#include "util/filesystem.h"

namespace selink { namespace util { namespace filesystem {

    boost::filesystem::path get_exe_path() {
#ifdef _WIN32
        HMODULE hModule = GetModuleHandleW(NULL);
        WCHAR path[MAX_PATH];
        DWORD r = GetModuleFileNameW(hModule, path, MAX_PATH);
        if (r <= 0) {
            return boost::filesystem::path(".\\self");
        }
        return boost::filesystem::path(path);
#elif defined(__linux__)
        char dest[PATH_MAX];
        if (readlink("/proc/self/exe", dest, PATH_MAX) == -1) {
            return boost::filesystem::path("./self");
        }
        else {
            return boost::filesystem::path(dest);
        }
#endif  // _WIN32
    }

    std::wstring utf8to16(std::string s) {
        std::wstring result;
        utf8::utf8to16(s.begin(), s.end(), std::back_inserter(result));
        return result;
    }
    std::string utf16to8(std::wstring s) {
        std::string result;
        utf8::utf16to8(s.begin(), s.end(), std::back_inserter(result));
        return result;
    }


    boost::filesystem::path path_from_utf8(std::string path) {
#ifdef _WIN32
        return boost::filesystem::path(utf8to16(path));
#elif defined(__linux__)
        return boost::filesystem::path(path);
#endif  // _WIN32
    }

    std::string path_to_utf8(boost::filesystem::path path) {
#ifdef _WIN32
        return utf16to8(path.native());
#elif defined(__linux__)
        return path.string();
#endif  // _WIN32
    }

    std::string path_uniform_case(boost::filesystem::path path) {
#ifdef _WIN32
        WCHAR wc[MAX_PATH];
        std::wstring tmp = path.native();
        wcscpy_s(wc, &tmp[0]);
        _wcslwr_s(wc);
        return utf16to8(std::wstring(wc));
#elif defined(__linux__)
        return path.string();
#endif  // _WIN32
    }

} } }  // namespace selink::util::filesystem
