#include "RouteTable.hpp"

#include <memory>

void RouteTable::insert(const Route& route) noexcept {
    auto* node = &_root;

    if (route.prefix_length == 0) {
        node->route = route;
        return;
    }

    for (std::uint8_t bit = 31; bit >= (32 - route.prefix_length); --bit) {
        std::uint8_t choice = (route.network >> bit) & 1U;
        auto&        child  = node->children[choice];
        if (!child) {
            child = std::make_unique<Node>();
        }

        node = child.get();
    }

    node->route = route;
}

std::optional<Route> RouteTable::longest_prefix_match(std::uint32_t dest) const noexcept {
    const auto*          node = &_root;
    std::optional<Route> best = _root.route;

    for (std::uint8_t bit = 31; bit >= 0 && node != nullptr; --bit) {
        std::uint8_t choice = (dest >> bit) & 1U;
        node                = node->children[choice].get();

        if (node != nullptr && node->route.has_value()) {
            best = node->route;
        }
    }
    return best;
}
