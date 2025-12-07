#include "FileParser.hpp"

#include "Interface.hpp"
#include "IpAddress.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {
std::string trim(std::string_view str) {
    auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string_view::npos)
        return "";
    auto end = str.find_last_not_of(" \t\r\n");
    return std::string{str.substr(start, end - start + 1)};
}

bool is_comment_or_empty(std::string_view line) {
    auto trimmed = trim(line);
    return trimmed.empty() || trimmed[0] == '#';
}
}  // namespace

std::vector<Interface> parse_interfaces(std::string_view filename) noexcept {
    std::vector<Interface> interfaces;
    std::ifstream          file{std::filesystem::path(filename)};
    std::string            line;

    while (std::getline(file, line)) {
        if (is_comment_or_empty(line))
            continue;

        std::istringstream iss(trim(line));
        std::string        name;
        std::string        addr_str;

        if (!(iss >> name >> addr_str))
            continue;

        auto slash_loc = addr_str.find('/');
        if (slash_loc == std::string::npos)
            continue;

        auto ip_str     = addr_str.substr(0, slash_loc);
        auto prefix_str = addr_str.substr(slash_loc + 1);
        auto ip_num     = ip_str_to_num(ip_str);
        auto prefix_len = static_cast<std::uint8_t>(std::stoi(prefix_str));
        auto netmask    = prefix_to_netmask(prefix_len);

        if (!ip_num.has_value())
            continue;

        auto network = ip_num.value() & netmask;

        interfaces.push_back({name, ip_num.value(), prefix_len, network, netmask});
    }

    return interfaces;
}

std::vector<Route> parse_routes(std::string_view filename) noexcept {
    std::vector<Route> routes;
    std::ifstream      file{std::filesystem::path(filename)};
    std::string        line;

    while (std::getline(file, line)) {
        if (is_comment_or_empty(line))
            continue;

        std::istringstream iss(trim(line));
        std::string        net_addr;
        std::string        next_hop_str;

        if (!(iss >> net_addr >> next_hop_str))
            continue;

        auto slash_loc = net_addr.find('/');
        if (slash_loc == std::string::npos)
            continue;

        auto ip_str     = net_addr.substr(0, slash_loc);
        auto prefix_str = net_addr.substr(slash_loc + 1);
        auto ip_num     = ip_str_to_num(ip_str);
        auto next_hop   = ip_str_to_num(next_hop_str);
        auto prefix_len = static_cast<std::uint8_t>(std::stoi(prefix_str));

        if (!ip_num.has_value() || !next_hop.has_value())
            continue;

        routes.push_back(
            {ip_num.value(), prefix_len, prefix_to_netmask(prefix_len), next_hop.value()});
    }

    return routes;
}

std::vector<std::uint32_t> parse_input(std::string_view filename) noexcept {
    std::vector<std::uint32_t> destinations;
    std::istream*              input = &std::cin;
    std::ifstream              file;

    if (!filename.empty()) {
        file.open(std::filesystem::path(filename));
        input = &file;
    }

    std::string line;
    while (std::getline(*input, line)) {
        if (is_comment_or_empty(line))
            continue;

        auto ip_opt = ip_str_to_num(trim(line));
        if (ip_opt.has_value()) {
            destinations.push_back(ip_opt.value());
        }
    }

    return destinations;
}
