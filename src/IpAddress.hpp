#ifndef IP_ADDRESS_HPP_
#define IP_ADDRESS_HPP_

#include <cstdint>
#include <optional>
#include <ostream>
#include <string_view>

class IpAddress {
   public:
    static IpAddress from_string(std::string_view addr);

    explicit IpAddress(std::uint32_t addr) : _addr{addr} {}

    friend std::ostream& operator<<(std::ostream& out, const IpAddress& addr);

   private:
    std::uint32_t _addr;
};

std::optional<std::uint32_t> ip_str_to_num(std::string_view addr);
std::string                  ip_num_to_str(std::uint32_t addr);

#endif
