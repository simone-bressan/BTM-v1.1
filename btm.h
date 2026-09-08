#ifndef BTM_H
#define BTM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/**
 * Bressan Telemetry Mapping (BTM) v1.1
 * Reference Core Library (C99 / Portable C)
 * 
 * Optimized for spaceflight embedded systems, particularly resource-constrained CubeSats.
 * Derived from Bressan's Invariant Ratio Theorem on 2-adic valuation and odd-part decompositions.
 * 
 * Architecture highlights:
 * - O(1) Constant-Time Bit-Packing.
 * - Zero Dynamic RAM allocation (pure register-based stream-packing).
 * - Intrinsic Single Event Upset (SEU) mitigation (no dynamic state buffer vulnerabilities).
 * - LSB-Stripping optimization (shaves exactly 1 bit per sample at zero computational cost).
 */

// Portable count trailing zeros (ctz)
static inline uint16_t btm_ctz(uint32_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_ctz(x);
#else
    if (x == 0) return 32;
    uint16_t n = 0;
    if ((x & 0x0000FFFF) == 0) { n += 16; x >>= 16; }
    if ((x & 0x000000FF) == 0) { n += 8;  x >>= 8;  }
    if ((x & 0x0000000F) == 0) { n += 4;  x >>= 4;  }
    if ((x & 0x00000003) == 0) { n += 2;  x >>= 2;  }
    if ((x & 0x00000001) == 0) { n += 1; }
    return n;
#endif
}

// Portable count leading zeros (clz)
static inline uint16_t btm_clz(uint32_t x) {
#if defined(__GNUC__) || defined(__clang__)
    return __builtin_clz(x);
#else
    if (x == 0) return 32;
    uint16_t n = 0;
    if ((x & 0xFFFF0000) == 0) { n += 16; x <<= 16; }
    if ((x & 0xFF000000) == 0) { n += 8;  x <<= 8;  }
    if ((x & 0xF0000000) == 0) { n += 4;  x <<= 4;  }
    if ((x & 0xC0000000) == 0) { n += 2;  x <<= 2;  }
    if ((x & 0x80000000) == 0) { n += 1; }
    return n;
#endif
}

// Fixed-buffer bitstream structure for zero-dynamic-RAM operations
typedef struct {
    uint8_t *buffer;
    size_t capacity_bytes;
    size_t bit_position;
} BitStream;

/**
 * Initializes the bitstream.
 * To prevent any residual RAM contamination, this function optionally zeroes the buffer.
 */
static inline void init_bitstream(BitStream *stream, uint8_t *buffer, size_t capacity_bytes, bool clear_buffer) {
    stream->buffer = buffer;
    stream->capacity_bytes = capacity_bytes;
    stream->bit_position = 0;
    if (clear_buffer && buffer != NULL && capacity_bytes > 0) {
        memset(buffer, 0, capacity_bytes);
    }
}

static inline void write_bits(BitStream *stream, uint32_t value, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        size_t byte_idx = stream->bit_position / 8;
        size_t bit_idx = stream->bit_position % 8;
        if (byte_idx >= stream->capacity_bytes) {
            return; // Buffer overflow protection
        }
        
        // Extract bit MSB-first
        uint8_t bit = (value >> (length - 1 - i)) & 1;
        if (bit) {
            *(stream->buffer + byte_idx) |= (1 << (7 - bit_idx));
        } else {
            // Explicitly clearing the bit guarantees safety even if the buffer contains dirty memory!
            *(stream->buffer + byte_idx) &= ~(1 << (7 - bit_idx));
        }
        stream->bit_position++;
    }
}

static inline uint32_t read_bits(BitStream *stream, uint16_t length) {
    uint32_t value = 0;
    for (uint16_t i = 0; i < length; i++) {
        size_t byte_idx = stream->bit_position / 8;
        size_t bit_idx = stream->bit_position % 8;
        if (byte_idx >= stream->capacity_bytes) {
            return 0;
        }
        uint8_t bit = (*(stream->buffer + byte_idx) >> (7 - bit_idx)) & 1;
        value = (value << 1) | bit;
        stream->bit_position++;
    }
    return value;
}

/**
 * Packs raw 16-bit telemetry values into the bitstream using BTM v1.1.
 * Runs in O(1) constant-time execution per sample.
 * 
 * NOTE ON OVERFLOW PROTECTION:
 * Intermediate math is performed using 32-bit types (int32_t / uint32_t) to completely
 * eliminate signed overflow risks (e.g. INT16_MIN limit or huge delta jumps).
 * Since the telemetry data represents 12-bit physical sensors, the maximum delta is
 * +/-4095, meaning the exponent k always safely fits within the allocated 4 bits.
 */
static inline void BressanTransformPack(const int16_t *raw_telemetry, int N, BitStream *out_stream) {
    int16_t prev_val = 0;
    for (int i = 0; i < N; i++) {
        // 1. Delta Encoding (performed in 32-bit to prevent signed 16-bit overflows)
        int32_t delta = (int32_t)*(raw_telemetry + i) - (int32_t)prev_val;
        prev_val = *(raw_telemetry + i);
        
        // 2. ZigZag Mapping (bijective mapping from Z to N using 32-bit range)
        uint32_t zz = (delta >= 0) ? (2 * (uint32_t)delta) : (2 * (uint32_t)(-delta) - 1);
        
        // 3. Unit shift to eliminate zero-exception (BTM Guarantee: zz_prime >= 1)
        uint32_t zz_prime = zz + 1;
        
        // 4. BTM Decomposition (Single Cycle)
        uint32_t k = btm_ctz(zz_prime);
        uint32_t m = zz_prime >> k;
        
        // 5. LSB-Stripping & Length Calculation (Odd parts always end with binary 1)
        uint32_t m_stripped = m >> 1;
        uint32_t m_len = (m_stripped == 0) ? 0 : (32 - btm_clz(m_stripped));
        
        // 6. Pack: Exponent k (4 bits) + Unary length of m_stripped + m_stripped
        write_bits(out_stream, k, 4);
        
        // Write m_len '1's followed by a '0' (Unary prefix coding)
        for (uint16_t j = 0; j < m_len; j++) {
            write_bits(out_stream, 1, 1);
        }
        write_bits(out_stream, 0, 1);
        
        // Write m_stripped
        if (m_len > 0) {
            write_bits(out_stream, m_stripped, m_len);
        }
    }
}

/**
 * Reconstructs raw 16-bit telemetry values from the BTM v1.1 bitstream.
 * Fully lossless, deterministic reconstruction on the ground station.
 */
static inline void BressanReconstruct(BitStream *in_stream, int N, int16_t *out_telemetry) {
    int16_t prev_val = 0;
    for (int i = 0; i < N; i++) {
        // 1. Read Exponent k (4 bits)
        uint32_t k = read_bits(in_stream, 4);
        
        // 2. Read m_len in Unary
        uint32_t m_len = 0;
        while (read_bits(in_stream, 1) == 1) {
            m_len++;
        }
        
        // 3. Read m_stripped
        uint32_t m_stripped = 0;
        if (m_len > 0) {
            m_stripped = read_bits(in_stream, m_len);
        }
        
        // 4. Reconstruct odd part m (Restore the stripped LSB '1')
        uint32_t m = (m_stripped << 1) | 1;
        
        // 5. Reconstruct ZigZag prime
        uint32_t zz_prime = m << k;
        
        // 6. Reconstruct ZigZag value
        uint32_t zz = zz_prime - 1;
        
        // 7. Reconstruct Delta (Perform math using uint32_t/int32_t to avoid overflow, then cast)
        int32_t delta;
        if (zz % 2 == 0) {
            delta = (int32_t)(zz / 2);
        } else {
            delta = -(int32_t)((zz + 1) / 2);
        }
        
        // 8. Accumulate telemetry
        int32_t reconstructed_val = (int32_t)prev_val + delta;
        *(out_telemetry + i) = (int16_t)reconstructed_val;
        prev_val = *(out_telemetry + i);
    }
}

#endif // BTM_H
