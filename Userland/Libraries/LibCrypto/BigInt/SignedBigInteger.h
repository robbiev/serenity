/*
 * Copyright (c) 2020, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Span.h>
#include <LibCrypto/BigInt/UnsignedBigInteger.h>

namespace Crypto {

struct SignedDivisionResult;

class SignedBigInteger {
public:
    SignedBigInteger(i32 x)
        : m_sign(x < 0)
        , m_unsigned_data(abs(x))
    {
    }

    SignedBigInteger(UnsignedBigInteger&& unsigned_data, bool sign)
        : m_sign(sign)
        , m_unsigned_data(move(unsigned_data))
    {
        ensure_sign_is_valid();
    }

    explicit SignedBigInteger(UnsignedBigInteger unsigned_data)
        : m_sign(false)
        , m_unsigned_data(move(unsigned_data))
    {
    }

    SignedBigInteger()
        : m_sign(false)
        , m_unsigned_data()
    {
    }

    [[nodiscard]] static SignedBigInteger create_invalid()
    {
        return { UnsignedBigInteger::create_invalid(), false };
    }

    [[nodiscard]] static SignedBigInteger import_data(StringView data) { return import_data((u8 const*)data.characters_without_null_termination(), data.length()); }
    [[nodiscard]] static SignedBigInteger import_data(u8 const* ptr, size_t length);

    [[nodiscard]] static SignedBigInteger create_from(i64 value)
    {
        auto sign = false;
        u64 unsigned_value;
        if (value < 0) {
            unsigned_value = static_cast<u64>(-(value + 1)) + 1;
            sign = true;
        } else {
            unsigned_value = value;
        }
        return SignedBigInteger { UnsignedBigInteger::create_from(unsigned_value), sign };
    }

    size_t export_data(Bytes, bool remove_leading_zeros = false) const;

    [[nodiscard]] static SignedBigInteger from_base(u16 N, StringView str);
    [[nodiscard]] String to_base(u16 N) const;

    [[nodiscard]] u64 to_u64() const;
    [[nodiscard]] double to_double() const;

    [[nodiscard]] UnsignedBigInteger const& unsigned_value() const { return m_unsigned_data; }
    [[nodiscard]] Vector<u32, STARTING_WORD_SIZE> const words() const { return m_unsigned_data.words(); }
    [[nodiscard]] bool is_negative() const { return m_sign; }
    [[nodiscard]] bool is_zero() const { return m_unsigned_data.is_zero(); }

    void negate()
    {
        if (!m_unsigned_data.is_zero())
            m_sign = !m_sign;
    }

    void set_to_0()
    {
        m_unsigned_data.set_to_0();
        m_sign = false;
    }

    void set_to(i32 other)
    {
        m_unsigned_data.set_to((u32)other);
        m_sign = other < 0;
    }
    void set_to(SignedBigInteger const& other)
    {
        m_unsigned_data.set_to(other.m_unsigned_data);
        m_sign = other.m_sign;
    }

    void invalidate()
    {
        m_unsigned_data.invalidate();
    }

    [[nodiscard]] bool is_invalid() const { return m_unsigned_data.is_invalid(); }

    // These get + 1 byte for the sign.
    [[nodiscard]] size_t length() const { return m_unsigned_data.length() + 1; }
    [[nodiscard]] size_t trimmed_length() const { return m_unsigned_data.trimmed_length() + 1; };

    [[nodiscard]] SignedBigInteger plus(SignedBigInteger const& other) const;
    [[nodiscard]] SignedBigInteger minus(SignedBigInteger const& other) const;
    [[nodiscard]] SignedBigInteger bitwise_or(SignedBigInteger const& other) const;
    [[nodiscard]] SignedBigInteger bitwise_and(SignedBigInteger const& other) const;
    [[nodiscard]] SignedBigInteger bitwise_xor(SignedBigInteger const& other) const;
    [[nodiscard]] SignedBigInteger bitwise_not() const;
    [[nodiscard]] SignedBigInteger shift_left(size_t num_bits) const;
    [[nodiscard]] SignedBigInteger multiplied_by(SignedBigInteger const& other) const;
    [[nodiscard]] SignedDivisionResult divided_by(SignedBigInteger const& divisor) const;

    [[nodiscard]] SignedBigInteger plus(UnsignedBigInteger const& other) const;
    [[nodiscard]] SignedBigInteger minus(UnsignedBigInteger const& other) const;
    [[nodiscard]] SignedBigInteger multiplied_by(UnsignedBigInteger const& other) const;
    [[nodiscard]] SignedDivisionResult divided_by(UnsignedBigInteger const& divisor) const;

    [[nodiscard]] u32 hash() const;

    void set_bit_inplace(size_t bit_index);

    [[nodiscard]] bool operator==(SignedBigInteger const& other) const;
    [[nodiscard]] bool operator!=(SignedBigInteger const& other) const;
    [[nodiscard]] bool operator<(SignedBigInteger const& other) const;
    [[nodiscard]] bool operator<=(SignedBigInteger const& other) const;
    [[nodiscard]] bool operator>(SignedBigInteger const& other) const;
    [[nodiscard]] bool operator>=(SignedBigInteger const& other) const;

    [[nodiscard]] bool operator==(UnsignedBigInteger const& other) const;
    [[nodiscard]] bool operator!=(UnsignedBigInteger const& other) const;
    [[nodiscard]] bool operator<(UnsignedBigInteger const& other) const;
    [[nodiscard]] bool operator>(UnsignedBigInteger const& other) const;

    enum class CompareResult {
        DoubleEqualsBigInt,
        DoubleLessThanBigInt,
        DoubleGreaterThanBigInt
    };

    [[nodiscard]] CompareResult compare_to_double(double) const;

private:
    void ensure_sign_is_valid()
    {
        if (m_sign && m_unsigned_data.is_zero())
            m_sign = false;
    }

    bool m_sign { false };
    UnsignedBigInteger m_unsigned_data;
};

struct SignedDivisionResult {
    Crypto::SignedBigInteger quotient;
    Crypto::SignedBigInteger remainder;
};

}

template<>
struct AK::Formatter<Crypto::SignedBigInteger> : AK::Formatter<Crypto::UnsignedBigInteger> {
    ErrorOr<void> format(FormatBuilder&, Crypto::SignedBigInteger const&);
};

inline Crypto::SignedBigInteger
operator""_sbigint(char const* string, size_t length)
{
    return Crypto::SignedBigInteger::from_base(10, { string, length });
}
