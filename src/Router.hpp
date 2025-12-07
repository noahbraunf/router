#ifndef ROUTER_HPP_
#define ROUTER_HPP_

#include "Interface.hpp"
#include "Route.hpp"
#include "RouteTable.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

class Router {
   public:
    struct Result {
        bool             is_reachable;
        std::string_view interface_name;
        std::uint32_t    next_hop;
    };

    void add_interface(const Interface& interface);
    void add_route(const Route& route);

    [[nodiscard]] Result route_packet(std::uint32_t dest) const;

   private:
    std::vector<Interface> _interfaces;
    RouteTable             _routes;

    [[nodiscard]] std::optional<const Interface*> find_interface(std::uint32_t addr) const;
};

#endif
