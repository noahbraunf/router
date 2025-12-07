#include "ArgParser.hpp"

#include <cstring>

Args parse_args(int argc, char* argv[] /* NOLINT */) noexcept {
    Args args;

    for (auto i = 1; i < argc; i++) {
        if (i + 1 >= argc)
            break;

        if (std::strcmp(argv[i], "-c") == 0) {
            args.config = argv[i + 1];
        } else if (std::strcmp(argv[i], "-r") == 0) {
            args.routefile = argv[i + 1];
        } else if (std::strcmp(argv[i], "-i") == 0) {
            args.inputfile = argv[i + 1];
        } else if (std::strcmp(argv[i], "-o") == 0) {
            args.outputfile = argv[i + 1];
        } else if (std::strcmp(argv[i], "-d") == 0) {
            args.debug_level = std::stoi(argv[i + 1]);
        }
    }
    args.valid = !args.config.empty() && !args.config.empty();
    return args;
}
