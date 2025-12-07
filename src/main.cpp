#include "ArgParser.hpp"
#include "FileParser.hpp"
#include "IpAddress.hpp"
#include "Logger.hpp"
#include "Router.hpp"

#include <fstream>
#include <iostream>
int main(int argc, char* argv[]) {
    Args args = parse_args(argc, argv);

    if (!args.valid) {
        std::cerr << "Invalid parameters" << std::endl;
        return 1;
    }

    log_level = args.debug_level;

    DEBUG << "Loading interfaces from " << args.config << ENDL;
    auto interfaces = parse_interfaces(args.config);

    DEBUG << "Loading routes from " << args.routefile << ENDL;
    auto routes = parse_routes(args.routefile);

    Router router;
    for (auto& interface : interfaces) {
        INFO << "Interface: " << interface.name << " " << ip_num_to_str(interface.addr) << "/"
             << static_cast<int>(interface.prefix_len) << ENDL;
        router.add_interface(interface);
    }

    for (const auto& route : routes) {
        INFO << "Route: " << ip_num_to_str(route.network) << "/"
             << static_cast<int>(route.prefix_length) << " -> " << ip_num_to_str(route.next_hop)
             << ENDL;
        router.add_route(route);
    }

    auto dests = parse_input(args.inputfile);

    std::ostream* output = &std::cout;
    std::ofstream out_file;
    if (!args.outputfile.empty()) {
        out_file.open(args.outputfile);
        output = &out_file;
    }

    for (auto dest : dests) {
        auto result = router.route_packet(dest);

        if (result.is_reachable) {
            *output << ip_num_to_str(dest) << ": " << result.interface_name << " -> "
                    << ip_num_to_str(result.next_hop) << "\n";
        } else {
            *output << ip_num_to_str(dest) << ": unreachable\n";
        }
    }

    return 0;
}
