#ifndef PARSER_H_
#define PARSER_H_

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
#endif
