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

    ## Simple

    @code{.cpp}
    using MPU_9250__DataPacket = simple_packet::SimplePacket<
              uint8_t  //  0) Packet descriptor
            , uint8_t  //  1) GPIO number
            , float    //  2) gyro[0]
            , float    //  3) gyro[1]
            , float    //  4) gyro[2]
            , float    //  5) accel[0]
            , float    //  6) accel[1]
            , float    //  7) accel[2]
            , float    //  8) mag[0]
            , float    //  9) mag[1]
            , float    // 10) mag[2]
            , float    // 11) temperature
            , uint64_t // 12) timestamp (milliseconds since boot)
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

    ## Advanced

    @code{.cpp}
    using Packet_sensor_data_ = simple_packet::SimplePacket<
          uint64_t // 0) counter (for debug / tracking missing packets)
        , uint64_t // 1) timestamp (milliseconds from boot)
        , float[3] // 2) gyro_data [x,y,z]
        , float[3] // 3) accel_data [x,y,z]
        , float[3] // 4) mag_data [x,y,z]
        , float    // 5) temperature C
        , float[4] // 6) quaternion [w,x,y,z]
    >;

    struct Packet_sensor_data : Packet_sensor_data_
    {
        inline uint64_t getCounter() const {
            return get_value_at<0>();
        }
        inline void setCounter(uint64_t value) {
            set_value_at<0>(value);
        }

        inline uint64_t getTimestamp() const {
            return get_value_at<1>();
        }
        inline void setTimestamp(uint64_t value) {
            set_value_at<1>(value);
        }

        inline const float* getGyroData() const {
            return reinterpret_cast<const float*>(bytes + get_bytes_offset_at<2>());
        }
        inline void setGyroData(const float (&data)[3]) {
            set_value_at<2>(data);
        }

        inline const float* getAccelData() const {
            return reinterpret_cast<const float*>(bytes + get_bytes_offset_at<3>());
        }
        inline void setAccelData(const float (&data)[3]) {
            set_value_at<3>(data);
        }

        inline const float* getMagData() const {
            return reinterpret_cast<const float*>(bytes + get_bytes_offset_at<4>());
        }
        inline void setMagData(const float (&data)[3]) {
            set_value_at<4>(data);
        }

        inline float getTemperature() const {
            return get_value_at<5>();
        }
        inline void setTemperature(float value) {
            set_value_at<5>(value);
        }

        inline const float* getQuaternion() const {
            return reinterpret_cast<const float*>(bytes + get_bytes_offset_at<6>());
        }
        inline void setQuaternion(const float (&data)[4]) {
            set_value_at<6>(data);
        }
    };
    @endcode
*/

} // namespace simple_packet
} // namespace meta
} // namespace ino
