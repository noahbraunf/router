#include "Parser.hpp"

#include <rapidcheck.h>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// Helper: convert uint32_t back to IP string
namespace {
std::string ip_num_to_str(std::uint32_t addr) {
    return std::to_string((addr >> 24U) & 0xFFU) + "." + std::to_string((addr >> 16U) & 0xFFU) +
           "." + std::to_string((addr >> 8U) & 0xFFU) + "." + std::to_string(addr & 0xFFU);
}

// Helper: build IP string from 4 octets
std::string octets_to_str(std::uint8_t first, std::uint8_t second, std::uint8_t third,
                          std::uint8_t fourth) {
    return std::to_string(first) + "." + std::to_string(second) + "." + std::to_string(third) +
           "." + std::to_string(fourth);
}

// Custom generator for valid octets (0-255)
rc::Gen<std::uint8_t> gen_octet() {
    return rc::gen::inRange<std::uint8_t>(0, 255);
}

// Custom generator for valid IP as 4 octets
rc::Gen<std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>> gen_ip_octets() {
    return rc::gen::tuple(gen_octet(), gen_octet(), gen_octet(), gen_octet());
}
}  // namespace

int main() {
    // Property 1: Valid IPs always parse successfully
    rc::check("valid IPs parse successfully", []() {
        const auto [FIRST, SECOND, THIRD, FOURTH] = *gen_ip_octets();
        const auto IP_STR                         = octets_to_str(FIRST, SECOND, THIRD, FOURTH);
        const auto RESULT                         = ip_str_to_num(IP_STR);
        RC_ASSERT(RESULT.has_value());
    });

    // Property 2: Parsed value matches expected numeric representation
    rc::check("parsed value equals (a<<24)|(b<<16)|(c<<8)|d", []() {
        const auto [FIRST, SECOND, THIRD, FOURTH] = *gen_ip_octets();
        const auto IP_STR                         = octets_to_str(FIRST, SECOND, THIRD, FOURTH);
        const auto RESULT                         = ip_str_to_num(IP_STR);

        RC_ASSERT(RESULT.has_value());

        const std::uint32_t EXPECTED = (static_cast<std::uint32_t>(FIRST) << 24U) |
                                       (static_cast<std::uint32_t>(SECOND) << 16U) |
                                       (static_cast<std::uint32_t>(THIRD) << 8U) |
                                       static_cast<std::uint32_t>(FOURTH);
        RC_ASSERT(*RESULT == EXPECTED);
    });

    // Property 3: Roundtrip - parse then stringify equals original (canonical form)
    rc::check("roundtrip: num_to_str(str_to_num(ip)) == ip", []() {
        const auto [FIRST, SECOND, THIRD, FOURTH] = *gen_ip_octets();
        const auto IP_STR                         = octets_to_str(FIRST, SECOND, THIRD, FOURTH);
        const auto RESULT                         = ip_str_to_num(IP_STR);

        RC_ASSERT(RESULT.has_value());
        RC_ASSERT(ip_num_to_str(*RESULT) == IP_STR);
    });

    // Property 4: Any uint32_t can roundtrip through stringify then parse
    rc::check("any uint32_t roundtrips through string", []() {
        const auto IP_NUM = *rc::gen::arbitrary<std::uint32_t>();
        const auto IP_STR = ip_num_to_str(IP_NUM);
        const auto RESULT = ip_str_to_num(IP_STR);

        RC_ASSERT(RESULT.has_value());
        RC_ASSERT(*RESULT == IP_NUM);
    });

    // Property 5: Leading zeros are rejected
    rc::check("leading zeros are rejected", []() {
        const auto OCTET                   = *rc::gen::inRange<std::uint8_t>(0, 99);
        const auto [SECOND, THIRD, FOURTH] = *rc::gen::tuple(gen_octet(), gen_octet(), gen_octet());

        // Create string with leading zero in first octet
        const auto IP_STR = "0" + std::to_string(OCTET) + "." + std::to_string(SECOND) + "." +
                            std::to_string(THIRD) + "." + std::to_string(FOURTH);

        const auto RESULT = ip_str_to_num(IP_STR);
        RC_ASSERT(!RESULT.has_value());
    });

    // Property 6: Too many octets rejected
    rc::check("5+ octets are rejected", []() {
        const auto [FIRST, SECOND, THIRD, FOURTH] = *gen_ip_octets();
        const auto EXTRA                          = *gen_octet();

        const auto IP_STR =
            octets_to_str(FIRST, SECOND, THIRD, FOURTH) + "." + std::to_string(EXTRA);
        const auto RESULT = ip_str_to_num(IP_STR);
        RC_ASSERT(!RESULT.has_value());
    });

    // Property 7: Too few octets rejected
    rc::check("fewer than 4 octets are rejected", []() {
        const auto FIRST  = *gen_octet();
        const auto SECOND = *gen_octet();
        const auto THIRD  = *gen_octet();

        RC_ASSERT(!ip_str_to_num(std::to_string(FIRST)).has_value());
        RC_ASSERT(!ip_str_to_num(std::to_string(FIRST) + "." + std::to_string(SECOND)).has_value());
        RC_ASSERT(!ip_str_to_num(std::to_string(FIRST) + "." + std::to_string(SECOND) + "." +
                                 std::to_string(THIRD))
                       .has_value());
    });

    // Property 8: Values > 255 rejected
    rc::check("octet values > 255 are rejected", []() {
        const auto BAD_OCTET               = *rc::gen::inRange(256, 999);
        const auto [SECOND, THIRD, FOURTH] = *rc::gen::tuple(gen_octet(), gen_octet(), gen_octet());

        const auto IP_STR = std::to_string(BAD_OCTET) + "." + std::to_string(SECOND) + "." +
                            std::to_string(THIRD) + "." + std::to_string(FOURTH);

        const auto RESULT = ip_str_to_num(IP_STR);
        RC_ASSERT(!RESULT.has_value());
    });

    // Property 9: Empty string rejected
    rc::check("empty string is rejected", []() { RC_ASSERT(!ip_str_to_num("").has_value()); });

    // Property 10: Non-digit/dot characters rejected
    rc::check("non-digit characters are rejected", []() {
        const auto [FIRST, SECOND, THIRD, FOURTH] = *gen_ip_octets();
        const auto BAD_CHAR = *rc::gen::element('a', 'z', 'A', 'Z', '-', '_', ' ', '/', ':', '@');

        // Insert bad character somewhere in a valid IP
        auto       ip_str = octets_to_str(FIRST, SECOND, THIRD, FOURTH);
        const auto POS    = *rc::gen::inRange<std::size_t>(0, ip_str.length());
        ip_str.insert(POS, 1, BAD_CHAR);

        const auto RESULT = ip_str_to_num(ip_str);
        RC_ASSERT(!RESULT.has_value());
    });

    // Property 11: Consecutive dots rejected
    rc::check("consecutive dots are rejected", []() {
        const auto [FIRST, SECOND, THIRD, FOURTH] = *gen_ip_octets();

        const auto IP_STR = std::to_string(FIRST) + ".." + std::to_string(SECOND) + "." +
                            std::to_string(THIRD) + "." + std::to_string(FOURTH);

        const auto RESULT = ip_str_to_num(IP_STR);
        RC_ASSERT(!RESULT.has_value());
    });

    // Property 12: Trailing dot rejected
    rc::check("trailing dot is rejected", []() {
        const auto [FIRST, SECOND, THIRD, FOURTH] = *gen_ip_octets();
        const auto IP_STR = octets_to_str(FIRST, SECOND, THIRD, FOURTH) + ".";

        const auto RESULT = ip_str_to_num(IP_STR);
        RC_ASSERT(!RESULT.has_value());
    });

    // Property 13: Leading dot rejected
    rc::check("leading dot is rejected", []() {
        const auto [FIRST, SECOND, THIRD, FOURTH] = *gen_ip_octets();
        const auto IP_STR = "." + octets_to_str(FIRST, SECOND, THIRD, FOURTH);

        const auto RESULT = ip_str_to_num(IP_STR);
        RC_ASSERT(!RESULT.has_value());
    });

    // Property 14: Edge cases - boundary values
    rc::check("boundary values work correctly", []() {
        // 0.0.0.0
        auto result = ip_str_to_num("0.0.0.0");
        RC_ASSERT(result.has_value());
        RC_ASSERT(*result == 0x00000000);

        // 255.255.255.255
        result = ip_str_to_num("255.255.255.255");
        RC_ASSERT(result.has_value());
        RC_ASSERT(*result == 0xFFFFFFFF);

        // 127.0.0.1 (localhost)
        result = ip_str_to_num("127.0.0.1");
        RC_ASSERT(result.has_value());
        RC_ASSERT(*result == 0x7F000001);

        // 192.168.1.1
        result = ip_str_to_num("192.168.1.1");
        RC_ASSERT(result.has_value());
        RC_ASSERT(*result == 0xC0A80101);
    });

    // Property 15: Ordering is preserved
    rc::check("a.b.c.d < a.b.c.(d+1) when d < 255", []() {
        const auto [FIRST, SECOND, THIRD] = *rc::gen::tuple(gen_octet(), gen_octet(), gen_octet());
        const auto FOURTH                 = *rc::gen::inRange<std::uint8_t>(0, 254);

        const auto IP1 = ip_str_to_num(octets_to_str(FIRST, SECOND, THIRD, FOURTH));
        const auto IP2 = ip_str_to_num(octets_to_str(FIRST, SECOND, THIRD, FOURTH + 1));

        RC_ASSERT(IP1.has_value());
        RC_ASSERT(IP2.has_value());
        RC_ASSERT(*IP1 < *IP2);
    });

    std::cout << "All property tests passed!\n";
    return 0;
}
