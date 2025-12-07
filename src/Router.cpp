#include "Router.hpp"

#include <optional>

void Router::add_interface(const Interface& interface) {
    _interfaces.push_back(interface);
}

void Router::add_route(const Route& route) {
    _routes.insert(route);
}

Router::Result Router::route_packet(std::uint32_t dest) const {
    auto direct_interface = find_interface(dest);
    if (direct_interface.has_value()) {
        return {true, direct_interface.value()->name, dest};
    }

    auto route = _routes.longest_prefix_match(dest);
    if (!route.has_value()) {
        return {false, "", 0};
    }

    auto interface = find_interface(route->next_hop);
    if (!interface.has_value()) {
        return {false, "", 0};
    }

    return {true, interface.value()->name, route->next_hop};
}

std::optional<const Interface*> Router::find_interface(std::uint32_t addr) const {
    const Interface* best_interface = nullptr;
    std::uint8_t     best_prefix    = 0;
    for (const auto& interface : _interfaces) {
        if (interface.contains(addr) && interface.prefix_len >= best_prefix) {
            best_interface = &interface;
            best_prefix    = interface.prefix_len;
        }
    }
    return best_interface != nullptr ? std::make_optional(best_interface) : std::nullopt;
}
