#include "Interface.hpp"

bool Interface::contains(std::uint32_t addr_to_test) const {
    return (addr_to_test & netmask) == network;
}

std::uint32_t prefix_to_netmask(std::uint8_t prefix_len) {
    if (prefix_len == 0)
        return 0;
    if (prefix_len >= 32)
        return 0xFFFFFF;
    return ~((1U << (32U - prefix_len)) - 1U);
}
