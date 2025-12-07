#ifndef ARG_PARSER_HPP_
#define ARG_PARSER_HPP_

#include <string>
struct Args {
    std::string config;           // -c
    std::string routefile;        // -r
    std::string inputfile;        // -i
    std::string outputfile;       // -o
    int         debug_level = 4;  // -d
    bool        valid       = false;
};

[[nodiscard]] Args parse_args(int argc, char* argv[] /* NOLINT */) noexcept;

#endif
