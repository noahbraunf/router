#ifndef IPV4_HEADER_H_
#define IPV4_HEADER_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

class IPv4HeaderView {
   public:
    static constexpr std::size_t  MIN_HEADER_BYTES  = 20;
    static constexpr std::size_t  MAX_HEADER_BYTES  = 60;
    static constexpr std::uint8_t VERSION_V4        = 4;
    static constexpr std::size_t  MAX_OPTIONS_BYTES = MAX_HEADER_BYTES - MIN_HEADER_BYTES;

    [[nodiscard]] static std::optional<IPv4HeaderView> from_bytes(const std::uint8_t* data,
                                                                  std::size_t         len);

    [[nodiscard]] std::uint8_t version() const {
        return static_cast<std::size_t>(_buffer[0] >> 4U) & static_cast<std::size_t>(0x0f);
    }

    [[nodiscard]] std::uint8_t ihl() const { return _buffer[0] & 0x0FU; }
    [[nodiscard]] std::size_t header_length() const { return static_cast<std::size_t>(ihl()) * 4U; }

    [[nodiscard]] std::uint8_t dscp() const {
        return static_cast<std::size_t>(_buffer[1] >> 2U) & 0x3FU;
    }
    [[nodiscard]] std::uint8_t ecn() const { return _buffer[1] & 0x03U; }

    [[nodiscard]] std::uint16_t total_length() const { return read_uint16(2); }
    [[nodiscard]] std::uint16_t identification() const { return read_uint16(4); }

    [[nodiscard]] std::uint8_t flags() const {
        return static_cast<std::size_t>(_buffer[6] >> 5U) & 0x07U;
    }
    [[nodiscard]] std::uint16_t fragment_offset() const { return read_uint16(6) & 0x1FFFU; }

    [[nodiscard]] std::uint8_t        ttl() const { return _buffer[8]; }
    [[nodiscard]] std::uint8_t        protocol() const { return _buffer[9]; }
    [[nodiscard]] std::uint16_t       checksum() const { return read_uint16(10); }
    [[nodiscard]] std::uint32_t       src_addr() const { return read_uint32(12); }
    [[nodiscard]] std::uint32_t       dst_addr() const { return read_uint32(16); }
    [[nodiscard]] bool                has_options() const { return _size > MIN_HEADER_BYTES; }
    [[nodiscard]] std::size_t         options_length() const { return _size - MIN_HEADER_BYTES; }
    [[nodiscard]] const std::uint8_t* options_data() const {
        return has_options() ? &_buffer[MIN_HEADER_BYTES] : nullptr;
    }

    [[nodiscard]] const std::uint8_t* data() const { return _buffer; }
    [[nodiscard]] std::size_t         size() const { return _size; }

    [[nodiscard]] bool is_valid_checksum() const;

   private:
    const std::uint8_t* _buffer;
    std::size_t         _size;

    IPv4HeaderView(const std::uint8_t* buffer, std::size_t size) : _buffer{buffer}, _size{size} {}

    [[nodiscard]] std::uint16_t read_uint16(std::size_t offset) const;
    [[nodiscard]] std::uint32_t read_uint32(std::size_t offset) const;
};

class IPv4HeaderBuilder {
   public:
    static constexpr std::size_t  MIN_HEADER_BYTES  = 20;
    static constexpr std::size_t  MAX_HEADER_BYTES  = 60;
    static constexpr std::uint8_t VERSION_V4        = 4;
    static constexpr std::size_t  MAX_OPTIONS_BYTES = MAX_HEADER_BYTES - MIN_HEADER_BYTES;

    IPv4HeaderBuilder() : _size{MIN_HEADER_BYTES} {
        set_version(VERSION_V4);
        set_ihl(5);
    }

    IPv4HeaderBuilder& set_version(std::uint8_t version);
    IPv4HeaderBuilder& set_ihl(std::uint8_t ihl);
    IPv4HeaderBuilder& set_dscp(std::uint8_t dscp);
    IPv4HeaderBuilder& set_ecn(std::uint8_t ecn);
    IPv4HeaderBuilder& set_total_len(std::uint16_t len);
    IPv4HeaderBuilder& set_identification(std::uint16_t idx);
    IPv4HeaderBuilder& set_flags(bool reserved = false, bool do_not_fragment = false,
                                 bool more_fragments = false);
    IPv4HeaderBuilder& set_fragment_offset(std::uint16_t offset);
    IPv4HeaderBuilder& set_ttl(std::uint8_t ttl);
    IPv4HeaderBuilder& set_protocol(std::uint8_t protocol);
    IPv4HeaderBuilder& set_checksum(std::uint8_t checksum);
    IPv4HeaderBuilder& set_src_addr(std::uint32_t addr);
    IPv4HeaderBuilder& set_dst_addr(std::uint32_t addr);
    IPv4HeaderBuilder& clear_opts();
    IPv4HeaderBuilder& append_opt(const std::uint8_t* opt_data, std::size_t opt_len);
    IPv4HeaderBuilder& pad_opts();
    IPv4HeaderBuilder& compute_checksum();

    [[nodiscard]] const std::uint8_t* data() const { return _buffer.data(); }
    [[nodiscard]] std::uint8_t*       data() { return _buffer.data(); }
    [[nodiscard]] std::size_t         size() const { return _size; }
    [[nodiscard]] IPv4HeaderView      view() const;

   private:
    void update_ihl();
    void write_uint16(std::size_t offset, std::uint16_t value);
    void write_uint32(std::size_t offset, std::uint32_t value);
    std::array<std::uint8_t, MAX_HEADER_BYTES> _buffer{};
    std::size_t                                _size{};
};

#endif
