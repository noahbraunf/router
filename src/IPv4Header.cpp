#include "IPv4Header.hpp"

#include <arpa/inet.h>
#include <sys/_endian.h>

#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>
std::optional<IPv4HeaderView> IPv4HeaderView::from_bytes(const std::uint8_t* data,
                                                         std::size_t         len) {
    if ((data == nullptr) || len < MIN_HEADER_BYTES) {
        return std::nullopt;
    }

    std::uint8_t ihl        = data[0] & 0x0FU;
    auto         header_len = static_cast<std::size_t>(ihl * 4U);

    if (header_len < MIN_HEADER_BYTES || header_len > MAX_HEADER_BYTES) {
        return std::nullopt;
    }
    if (len < header_len) {
        return std::nullopt;
    }

    return IPv4HeaderView{data, len};
}
std::uint16_t IPv4HeaderView::read_uint16(std::size_t offset) const {
    std::uint16_t value{};
    std::memcpy(&value, &_buffer[offset], sizeof(value));
    return ntohs(value);
}

std::uint32_t IPv4HeaderView::read_uint32(std::size_t offset) const {
    std::uint32_t value{};
    std::memcpy(&value, &_buffer[offset], sizeof(value));
    return ntohl(value);
}

IPv4HeaderBuilder& IPv4HeaderBuilder::set_version(std::uint8_t version) {
    // put version into upper 4 bits of byte 0
    _buffer[0] = static_cast<std::uint8_t>((_buffer[0] & 0x0FU) | ((version & 0x0FU) << 4U));
    return *this;
}

IPv4HeaderBuilder& IPv4HeaderBuilder::set_ihl(std::uint8_t ihl) {
    // put ihl into lower 4 bits of byte 0
    _buffer[0] = static_cast<std::uint8_t>((_buffer[0] & 0xF0U) | (ihl & 0x0FU));
    return *this;
}

IPv4HeaderBuilder& IPv4HeaderBuilder::set_dscp(std::uint8_t dscp) {
    // put dscp into upper 6 bits of byte 1
    _buffer[1] = static_cast<std::uint8_t>((_buffer[1] & 0x03U) | ((dscp & 0x3FU) << 2U));
    return *this;
}

IPv4HeaderBuilder& IPv4HeaderBuilder::set_ecn(std::uint8_t ecn) {
    // put ecn into lower 2 bits of byte 1
    _buffer[1] = static_cast<std::uint8_t>((_buffer[1] & 0xFCU) | (ecn & 0x03U));
    return *this;
}

IPv4HeaderBuilder& IPv4HeaderBuilder::set_total_len(std::uint16_t len) {
    write_uint16(2, len);
    return *this;
}

IPv4HeaderBuilder& IPv4HeaderBuilder::set_identification(std::uint16_t idx) {
    write_uint16(4, idx);
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::set_flags(bool reserved, bool do_not_fragment,
                                                bool more_fragments) {
    const auto FLAG_BITS =
        static_cast<std::uint8_t>(((reserved ? 1U : 0U) << 2U) |
                                  ((do_not_fragment ? 1U : 0U) << 1U) | (more_fragments ? 1U : 0U));
    _buffer[6] = static_cast<std::uint8_t>((_buffer[6] & 0x1FU) | ((FLAG_BITS & 0x07U) << 5U));
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::set_fragment_offset(std::uint16_t offset) {
    _buffer[6] =
        static_cast<std::uint8_t>((_buffer[6] & 0xE0U) | static_cast<std::uint8_t>(offset >> 8U));
    _buffer[7] = static_cast<std::uint8_t>(offset & 0x0FFU);
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::set_ttl(std::uint8_t ttl) {
    _buffer[8] = ttl;
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::set_protocol(std::uint8_t protocol) {
    _buffer[9] = protocol;
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::set_checksum(std::uint8_t checksum) {
    write_uint16(10, checksum);
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::set_src_addr(std::uint32_t addr) {
    write_uint32(12, addr);
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::set_dst_addr(std::uint32_t addr) {
    write_uint32(16, addr);
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::clear_opts() {
    _size = MIN_HEADER_BYTES;
    update_ihl();
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::append_opt(const std::uint8_t* opt_data,
                                                 std::size_t         opt_len) {
    if (_size + opt_len > MAX_HEADER_BYTES) {
        throw std::length_error("Options would exceed maximum IPv4 header size.");
    }
    std::memcpy(&_buffer[_size], opt_data, opt_len);
    _size += opt_len;
    update_ihl();
    return *this;
}
IPv4HeaderBuilder& IPv4HeaderBuilder::pad_opts() {
    while (_size % 4 != 0 && _size < MAX_HEADER_BYTES) {
        _buffer[_size++] = 0;
    }
    update_ihl();
    return *this;
}

void IPv4HeaderBuilder::update_ihl() {
    assert(_size % 4 == 0);  // header is in 4 byte blocks
    auto ihl = static_cast<std::uint8_t>(_size / 4U);

    _buffer[0] = static_cast<std::uint8_t>((_buffer[0] & 0xF0U) | (ihl & 0x0FU));
}

IPv4HeaderBuilder& IPv4HeaderBuilder::compute_checksum() {
    write_uint16(10, 0);
    std::uint32_t sum = 0;
    for (std::size_t i = 0; i < _size; i += 2) {  // go by two bytes each time
        sum += static_cast<std::uint16_t>(static_cast<uint16_t>(_buffer[i] << 8U) | _buffer[i + 1]);
    }
    while ((sum >> 16U) != 0U) {
        sum = (sum & 0xFFFFU) + (sum >> 16U);
    }
    write_uint16(10, static_cast<std::uint16_t>(~sum));
    return *this;
}
IPv4HeaderView IPv4HeaderBuilder::view() const {
    auto view = IPv4HeaderView::from_bytes(_buffer.data(), _size);
    assert(view.has_value());
    return *view;
}
void IPv4HeaderBuilder::write_uint16(std::size_t offset, std::uint16_t value) {
    value = htons(value);
    std::memcpy(&_buffer[offset], &value, sizeof(value));
}
void IPv4HeaderBuilder::write_uint32(std::size_t offset, std::uint32_t value) {
    value = htonl(value);
    std::memcpy(&_buffer[offset], &value, sizeof(value));
}
