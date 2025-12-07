#ifndef LOGGING_HPP_
#define LOGGING_HPP_

#ifndef __FILE_NAME__
    #define __FILE_NAME__ std::filesystem::path(__FILE__).filename().string()
#endif

inline int log_level = 3;  // NOLINT
#define TRACE            \
    if (log_level > 5) { \
    std::cerr << "TRACE: "
#define DEBUG            \
    if (log_level > 4) { \
    std::cerr << "DEBUG: "
#define INFO             \
    if (log_level > 3) { \
    std::cerr << "INFO: "
#define WARNING          \
    if (log_level > 2) { \
    std::cerr << "WARNING: "
#define ERROR            \
    if (log_level > 1) { \
    std::cerr << "ERROR: "
#define FATAL            \
    if (log_level > 0) { \
    std::cerr << "FATAL: "
#define ENDL                                                      \
    " (" << __FILE_NAME__ << ":" << __LINE__ << ")" << std::endl; \
    }

#endif
