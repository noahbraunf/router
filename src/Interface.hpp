#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include <cstdint>
#include <string>
struct Interface {
    std::string   name;
    std::uint32_t addr;
    std::uint8_t  prefix_len;
    std::uint32_t network;
    std::uint32_t netmask;

    [[nodiscard]] bool contains(std::uint32_t addr_to_test) const;
};

std::uint32_t prefix_to_netmask(std::uint8_t prefix_length);
#endif
