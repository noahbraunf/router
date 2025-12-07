#ifndef ROUTE_TABLE_HPP_
#define ROUTE_TABLE_HPP_

#include "Route.hpp"

#include <array>
#include <memory>
#include <optional>

class RouteTable {
   public:
    void                               insert(const Route& route) noexcept;
    [[nodiscard]] std::optional<Route> longest_prefix_match(std::uint32_t dest) const noexcept;

   private:
    struct Node {
        std::optional<Route>                 route;
        std::array<std::unique_ptr<Node>, 2> children;  // since we are working on bits, only two
                                                        // options for children, 0 and 1
    };

    Node _root;
};

#endif
