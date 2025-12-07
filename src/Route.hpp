#ifndef ROUTE_HPP_
#define ROUTE_HPP_

#include <cstdint>
struct Route {
    std::uint32_t network;
    std::uint8_t  prefix_length;
    std::uint32_t netmask;
    std::uint32_t next_hop;
};

#endif
