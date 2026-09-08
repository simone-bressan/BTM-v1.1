#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "btm.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Simple LCG random generator for cross-platform portability without external dependencies
static uint32_t lcg_state = 12345;
static inline uint32_t lcg_rand() {
    lcg_state = lcg_state * 1664525 + 1013904223;
    return lcg_state;
}

static inline double random_gaussian() {
    // Box-Muller transform for simulating realistic Gaussian noise in orbit sensors
    double u1 = (double)lcg_rand() / 4294967296.0;
    double u2 = (double)lcg_rand() / 4294967296.0;
    return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

int main() {
    printf("====================================================================\n");
    printf("   Bressan Telemetry Mapping (BTM) v1.1 - Reference Benchmark       \n");
    printf("====================================================================\n\n");

    const int N = 1000; // Simulated orbit telemetry samples
    int16_t *raw_telemetry = malloc(N * sizeof(int16_t));
    int16_t *reconstructed = malloc(N * sizeof(int16_t));
    
    if (raw_telemetry == NULL || reconstructed == NULL) {
        printf("[-] Memory allocation failed!\n");
        if (raw_telemetry) free(raw_telemetry);
        if (reconstructed) free(reconstructed);
        return 1;
    }

    // Generate a physical orbit temperature simulation (Sine wave + random thermal fluctuations)
    double base_temp = 20.0; // 20 degrees Celsius base
    double amplitude = 15.0; // +/- 15 degrees day-to-night variation
    
    raw_telemetry = 20; // CORRETTO! Assegnazione del valore iniziale al primo elemento dell'array
    for (int i = 1; i < N; i++) {
        double cycle = 2.0 * M_PI * (double)i / 200.0; // 5 cycles in the telemetry block
        double noise = random_gaussian() * 2.0;       // Gaussian thermal noise (stddev = 2.0)
        double current_temp = base_temp + amplitude * sin(cycle) + noise;
        raw_telemetry[i] = (int16_t)round(current_temp);
    }
    
    // Allocate a buffer for the bitstream (max worst-case size: N * 4 bytes is extremely safe)
    size_t max_buffer_bytes = N * 4;
    uint8_t *bit_buffer = malloc(max_buffer_bytes);
    if (bit_buffer == NULL) {
        printf("[-] Bit buffer allocation failed!\n");
        free(raw_telemetry);
        free(reconstructed);
        return 1;
    }
    
    BitStream stream;
    // init_bitstream with 'true' to safely zero-out the buffer in RAM and avoid contamination
    init_bitstream(&stream, bit_buffer, max_buffer_bytes, true);
    
    // Execute packing
    printf("[*] Executing BTM v1.1 packing on %d samples...\n", N);
    BressanTransformPack(raw_telemetry, N, &stream);
    
    size_t packed_bits = stream.bit_position;
    double avg_bit_size = (double)packed_bits / N;
    double compression_ratio = (16.0 / avg_bit_size);
    double bandwidth_saving = (1.0 - (avg_bit_size / 16.0)) * 100.0;
    
    printf("[+] Compression Completed.\n");
    printf("    - Total bits packed:    %zu bits\n", packed_bits);
    printf("    - Average sample size:   %.2f bits/sample (Uncompressed: 16.00)\n", avg_bit_size);
    printf("    - Compression Ratio:     %.2fx\n", compression_ratio);
    printf("    - Bandwidth Savings:     %.2f%%\n\n", bandwidth_saving);
    
    // Execute reconstruction
    printf("[*] Executing ground-station reconstruction (lossless unpacking)...\n");
    BitStream in_stream;
    init_bitstream(&in_stream, bit_buffer, max_buffer_bytes, false); // No need to clear buffer for reading!
    BressanReconstruct(&in_stream, N, reconstructed);
    
    // Verify results
    int mismatches = 0;
    for (int i = 0; i < N; i++) {
        if (raw_telemetry[i] != reconstructed[i]) {
            mismatches++;
            if (mismatches <= 5) {
                printf("    - ERROR: Mismatch at index %d: Original=%d, Reconstructed=%d\n", 
                       i, raw_telemetry[i], reconstructed[i]);
            }
        }
    }
    
    if (mismatches == 0) {
        printf("[SUCCESS] Lossless verification passed! 100%% of samples match exactly.\n");
        printf("[SUCCESS] BTM v1.1 is fully functional, prefix-free, and ready for deployment.\n");
    } else {
        printf("[FAILED] Lossless verification failed with %d mismatches!\n", mismatches);
    }
    
    printf("\n====================================================================\n");
    
    free(raw_telemetry);
    free(reconstructed);
    free(bit_buffer);
    return mismatches == 0 ? 0 : 1;
}
