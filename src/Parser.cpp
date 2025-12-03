#include "Parser.hpp"

#include <optional>

std::optional<std::uint32_t> ip_str_to_num(std::string_view addr) {
    std::uint32_t result      = 0;
    std::uint32_t octet       = 0;
    std::int8_t   shift       = 24;
    std::uint8_t  digit_count = 0;

    for (const char CHR : addr) {
        if (CHR == '.') {  // end of octet
            if (digit_count == 0 || shift < 0) {
                return std::nullopt;
            }
            result |= octet << static_cast<std::uint32_t>(shift);
            shift -= 8;
            octet       = 0;
            digit_count = 0;
        } else if (CHR >= '0' && CHR <= '9') {
            if (digit_count > 0 && octet == 0) {  // reject leading zeros
                return std::nullopt;
            }
            octet = (octet * 10) + static_cast<std::uint32_t>(CHR - '0');
            ++digit_count;
            if (digit_count > 3 || octet > 255) {  // octet too large
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }

    if (digit_count == 0 || octet > 255 || shift != 0) {
        return std::nullopt;
    }
    return result | octet;
}
