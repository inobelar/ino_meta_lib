#pragma once

#include <ino/meta/traits/type_at.hpp>

#include <ino/meta/SimplePacket/traits/count_bytes.hpp>
#include <ino/meta/SimplePacket/traits/bytes_offset.hpp>

#include <cstring> // for memcpy()

namespace ino {
namespace meta {
namespace simple_packet {

template <typename ... Args>
struct View
{
    static constexpr size_t N_ARGS  = sizeof...(Args);
    static constexpr size_t N_BYTES = ino::meta::simple_packet::traits::count_bytes<Args...>::value;

    // -------------------------------------------------------------------------

    template <std::size_t Idx>
    using nth_type = typename ino::meta::traits::type_at<Idx, Args...>::type;

    // -------------------------------------------------------------------------

    template <size_t Idx>
    static constexpr size_t get_bytes_offset_at() {
        return ino::meta::simple_packet::traits::bytes_offset<Idx, Args...>::value;
    }

    // -------------------------------------------------------------------------

    template <size_t Idx>
    static void set_value_at(uint8_t* dst, const nth_type<Idx>& value)
    {
        constexpr size_t offset = get_bytes_offset_at<Idx>();
        memcpy((dst + offset), &value, sizeof(value));
    }

    template <size_t Idx>
    static nth_type<Idx> get_value_at(const uint8_t* src)
    {
        nth_type<Idx> value;

        constexpr size_t offset = get_bytes_offset_at<Idx>();
        memcpy(&value, (src + offset), sizeof(value));

        return value;
    }
};

// -----------------------------------------------------------------------------

template <typename ... Args>
struct SimplePacket : View<Args...>
{
    // Shortcut
    using View = View<Args...>;

    // Shortcut
    template <std::size_t Idx>
    using nth_type = typename View::template nth_type<Idx>;

    // -------------------------------------------------------------------------

    uint8_t bytes[View::N_BYTES];

    // -------------------------------------------------------------------------

    template <size_t Idx>
    void set_value_at(const nth_type<Idx>& value) {
        View::template set_value_at<Idx>(bytes, value);
    }

    template <size_t Idx>
    nth_type<Idx> get_value_at() const {
        return View::template get_value_at<Idx>(bytes);
    }

    // -------------------------------------------------------------------------
    // Common helper functions, to behave the same as std::array<T,Size>

    inline uint8_t* data() { return bytes; }
    constexpr const uint8_t* data() const { return bytes; }

    constexpr size_t size() const { return View::N_BYTES; }
};

/**
    # Example of usage

    @code{.cpp}
    using MPU_9250__DataPacket = simple_packet::Packet<
              uint8_t // Packet descriptor
            , uint8_t // GPIO number
            , float   // gyro[0]
            , float   // gyro[1]
            , float   // gyro[2]
            , float   // accel[0]
            , float   // accel[1]
            , float   // accel[2]
            , float   // mag[0]
            , float   // mag[1]
            , float   // mag[2]
            , float   // temperature
            , uint64_t // timestamp (milliseconds since boot)
    >;

    MPU_9250__DataPacket packet;

    // Pre-fill bytes with some random value :D
    for(auto & v : packet.bytes)
        v = 42;

    // Set values
    packet.set_value_at<0>( 25 );
    packet.set_value_at<1>( 26 );
    packet.set_value_at<2>( 27 );
    packet.set_value_at<3>( 28.1f );
    packet.set_value_at<4>( 29.0f );
    packet.set_value_at<5>( 30 );
    packet.set_value_at<6>( 31 );

    // Dump bytes state
    std::size_t idx = 0;
    for(const auto& v : packet.bytes)
    {
        std::printf("[%lu] = %i\n", idx, (int)v);
        idx += 1;
    }

    // Retreive values
    const auto v0 = packet.get_value_at<0>();
    const auto v1 = packet.get_value_at<1>();
    const auto v2 = packet.get_value_at<2>();
    const auto v3 = packet.get_value_at<3>();
    const auto v4 = packet.get_value_at<4>();
    const auto v5 = packet.get_value_at<5>();
    const auto v6 = packet.get_value_at<6>();
    @endcode
*/

} // namespace simple_packet
} // namespace meta
} // namespace ino
