//
// Created by Roumite on 07/04/2026.
//

#pragma once

#include <cstdint>
#include <bit>
#include <cmath>

/*
 * Contains function to compute constant folding
 */

namespace compiler
{
    /* Unsigned Arithmetic */

    inline uint32_t exec_add(const uint32_t a, const uint32_t b)
    {
        return a + b;
    }

    inline uint32_t exec_sub(const uint32_t a, const uint32_t b)
    {
        return a - b;
    }

    inline uint32_t exec_mul(const uint32_t a, const uint32_t b)
    {
        return a * b;
    }

    inline uint32_t exec_div(const uint32_t a, const uint32_t b)
    {
        return a / b;
    }

    inline uint32_t exec_mod(const uint32_t a, const uint32_t b)
    {
        return a % b;
    }

    inline uint32_t exec_neg(const uint32_t a, const uint32_t b)
    {
        return -a;
    }

    inline uint32_t exec_not(const uint32_t a, const uint32_t b)
    {
        return !a;
    }

    /* Float Arithmetic */

    inline uint32_t exec_fadd(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(std::bit_cast<float>(a) + std::bit_cast<float>(b));
    }

    inline uint32_t exec_fsub(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(std::bit_cast<float>(a) - std::bit_cast<float>(b));
    }

    inline uint32_t exec_fmul(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(std::bit_cast<float>(a) * std::bit_cast<float>(b));
    }

    inline uint32_t exec_fdiv(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(std::bit_cast<float>(a) / std::bit_cast<float>(b));
    }

    inline uint32_t exec_fmod(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(std::fmod(std::bit_cast<float>(a), std::bit_cast<float>(b)));
    }

    inline uint32_t exec_fneg(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(-std::bit_cast<float>(a));
    }

    /* Signed Arithmetic */

    inline uint32_t exec_sdiv(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(std::bit_cast<int>(a) / std::bit_cast<int>(b));
    }

    inline uint32_t exec_smul(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(std::bit_cast<int>(a) * std::bit_cast<int>(b));
    }

    /* comparisons */

    inline uint32_t exec_eq(const uint32_t a, const uint32_t b)
    {
        return a == b;
    }

    inline uint32_t exec_neq(const uint32_t a, const uint32_t b)
    {
        return a != b;
    }

    inline uint32_t exec_lt(const uint32_t a, const uint32_t b)
    {
        return a < b;
    }

    inline uint32_t exec_gt(const uint32_t a, const uint32_t b)
    {
        return a > b;
    }

    inline uint32_t exec_leq(const uint32_t a, const uint32_t b)
    {
        return a <= b;
    }

    inline uint32_t exec_geq(const uint32_t a, const uint32_t b)
    {
        return a >= b;
    }

    /* Signed comparisons */

    inline uint32_t exec_slt(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<int>(a) < std::bit_cast<int>(b);
    }

    inline uint32_t exec_sgt(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<int>(a) > std::bit_cast<int>(b);
    }

    inline uint32_t exec_sleq(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<int>(a) <= std::bit_cast<int>(b);
    }

    inline uint32_t exec_sgeq(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<int>(a) >= std::bit_cast<int>(b);
    }

    /* Float comparisons */

    inline uint32_t exec_feq(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<float>(a) == std::bit_cast<float>(b);
    }

    inline uint32_t exec_fneq(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<float>(a) != std::bit_cast<float>(b);
    }

    inline uint32_t exec_flt(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<float>(a) < std::bit_cast<float>(b);
    }

    inline uint32_t exec_fgt(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<float>(a) > std::bit_cast<float>(b);
    }

    inline uint32_t exec_fleq(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<float>(a) <= std::bit_cast<float>(b);
    }

    inline uint32_t exec_fgeq(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<float>(a) >= std::bit_cast<float>(b);
    }

    /* Float Conversions */

    inline uint32_t exec_itof(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(
            static_cast<float>(std::bit_cast<int>(a))
        );
    }

    inline uint32_t exec_utof(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(
            static_cast<float>(a)
        );
    }

    inline uint32_t exec_ftoi(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(
            static_cast<int>(std::bit_cast<float>(a))
        );
    }

    inline uint32_t exec_ftou(const uint32_t a, const uint32_t b)
    {
        return static_cast<uint32_t>(std::bit_cast<float>(a));
    }

    /* boolean logic */

    inline uint32_t exec_and(const uint32_t a, const uint32_t b)
    {
        return a && b;
    }

    inline uint32_t exec_or(const uint32_t a, const uint32_t b)
    {
        return a || b;
    }

    /* Bitwise */

    inline uint32_t exec_bit_and(const uint32_t a, const uint32_t b)
    {
        return a & b;
    }

    inline uint32_t exec_bit_or(const uint32_t a, const uint32_t b)
    {
        return a | b;
    }

    inline uint32_t exec_bit_xor(const uint32_t a, const uint32_t b)
    {
        return a ^ b;
    }

    inline uint32_t exec_bit_not(const uint32_t a, const uint32_t b)
    {
        return ~a;
    }

    inline uint32_t exec_bit_shift_left(const uint32_t a, const uint32_t b)
    {
        return a << b;
    }

    inline uint32_t exec_bit_shift_right(const uint32_t a, const uint32_t b)
    {
        return a >> b;
    }

    inline uint32_t exec_bit_signed_shift_right(const uint32_t a, const uint32_t b)
    {
        return std::bit_cast<uint32_t>(
            std::bit_cast<int>(a) >> b
        );
    }
}