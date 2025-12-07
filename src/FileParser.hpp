#ifndef FILE_PARSER_HPP_
#define FILE_PARSER_HPP_

#include "Interface.hpp"
#include "Route.hpp"

#include <cstdint>
#include <string_view>
#include <vector>

[[nodiscard]] std::vector<Interface>     parse_interfaces(std::string_view filename) noexcept;
[[nodiscard]] std::vector<Route>         parse_routes(std::string_view filename) noexcept;
[[nodiscard]] std::vector<std::uint32_t> parse_input(std::string_view filename) noexcept;

#endif
