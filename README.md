# Bressan Telemetry Mapping (BTM) v1.1

[![ESA OSIP - Candidate](https://img.shields.io/badge/ESA%20OSIP-Candidate-blue.svg)](https://ideas.esa.int/)
[![Zenodo Report DOI](https://img.shields.io/badge/Zenodo%20Report-10.5281%2Fzenodo.22288196-success.svg)](https://doi.org/10.5281/zenodo.22288196)
[![Zenodo Preprint DOI](https://img.shields.io/badge/Zenodo%20Preprint-10.5281%2Fzenodo.22215083-blueviolet.svg)](https://doi.org/10.5281/zenodo.22215083)
[![Language](https://img.shields.io/badge/Language-C99-orange.svg)](#)
[![License](https://img.shields.io/badge/License-CC%20BY%204.0-green.svg)](https://creativecommons.org/licenses/by/4.0/)

An ultra-lightweight, high-performance, and radiation-robust telemetry encoding framework designed for modern space missions, CubeSats, and deep-space probes. Derived from a novel application of 2-adic valuation and odd-part integer decomposition, BTM enables deterministic, constant-time \\(O(1)\\) bit-packing with **zero dynamic RAM overhead**.

Developed by **Simone Bressan** (Independent Aerospace & Telecommunications Researcher, Cormons, Italy) and submitted to the **European Space Agency (ESA)** under the Open Space Innovation Platform (OSIP).

---

## 🛰️ Operational Challenges in Space Telemetry

Traditional data compression and encoding standards, such as **CCSDS 121.0-B (Rice coding)**, achieve excellent compression ratios but suffer from three critical bottlenecks in safety-critical space embedded systems:

1. **Vulnerability to Space Radiation (SEUs):** Rice encoders rely on dynamic RAM buffers and packet accumulation blocks. When exposed to cosmic rays in orbit, these volatile memories are highly susceptible to **Single Event Upsets (SEU)**, which can corrupt the encoder state and cause complete flight computer crashes.
2. **Execution Jitter (Non-Deterministic Latency):** The execution latency of traditional entropy coders varies dynamically with the entropy of the input block. This jitter complicates real-time control loops and Guidance, Navigation, and Control (GNC) tasks.
3. **Hardware Overhead (SWaP Constraint):** Standard implementations require complex lookup tables, variable block buffers, and significant computational overhead, which are highly undesirable for power-limited CubeSats.

---

## ⚡ The BTM v1.1 Solution

BTM v1.1 solves these challenges by utilizing number theory properties to restructure and pack integers at the register level:

* **Zero-RAM Footprint:** The algorithm processes telemetry streams in a purely sequential, bitwise manner. It requires **no dynamic memory allocation or static tables**, executing entirely inside the CPU's registers.
* **Deterministic \\(O(1)\\) Complexity:** Since there are no variable loops, dynamic trees, or statistical training phases, every single sample is packed in constant time \\(O(1)\\) and a fixed number of CPU clock cycles.
* **Prefix-Free Stream Decodability:** BTM introduces a unit-shift mapping (\\(Z' = Z + 1\\)) to eliminate the zero-exception, combined with a high-speed unary prefix coding of the odd residue length. This ensures lossless, prefix-free, real-time stream decoding without metadata overhead.
* **LSB-Stripping Optimization:** By exploiting the mathematical invariant that the odd residue \\(m_n\\) of any positive integer always ends with a binary `1`, BTM shaves exactly **1 bit per sample** at zero computational cost.

### 📊 Performance and Efficiency Benchmark

Using a simulated orbital thermal telemetry sequence of **50,000 samples**, BTM paired with a Delta-ZigZag pre-processor demonstrates:

| Scenario | Average Sample Size | Transmitter Energy Savings | SEU Immunity |
| :--- | :---: | :---: | :---: |
| **Raw 12-bit Transmission** | 12.00 bits | *Baseline* | Vulnerable |
| **BTM (Clean Orbit Conditions)** | **3.22 bits** | **-73.16%** | **Inherent** |
| **BTM (Heavy Thermal Noise)** | **7.23 bits** | **-39.74%** | **Inherent** |

---

## 📂 Repository Contents

This repository contains the reference implementation of BTM v1.1 in standard ANSI C (C99):

1. **`btm.h`**: The header-only core library. It contains portable bit-manipulation utilities, the fixed-memory `BitStream` implementation, the `BressanTransformPack` encoder, and the `BressanReconstruct` decoder.
2. **`main.c`**: A standalone reference benchmark program. It simulates an orbital sensor's physical temperature fluctuations, injects Gaussian thermal noise, executes BTM packing, measures bitstream size, and verifies 100% lossless reconstruction on the ground.

---

## ⚙️ How to Build and Run the Benchmark

Since the codebase is written in pure portable C without external dependencies, it compiles out-of-the-box on any standard compiler (GCC, Clang, MSVC) and is directly compatible with space-grade microcontrollers (such as LEON3/LEON4, RISC-V, and ARM Cortex-M cores).

To compile and execute the benchmark program on your local system:

```bash
# Compile using GCC with O3 optimization
gcc -O3 main.c -lm -o btm_benchmark

# Run the benchmark
./btm_benchmark
Expected Output
====================================================================
   Bressan Telemetry Mapping (BTM) v1.1 - Reference Benchmark       
====================================================================

[*] Executing BTM v1.1 packing on 1000 samples...
[+] Compression Completed.
    - Total bits packed:    3852 bits
    - Average sample size:   3.85 bits/sample (Uncompressed: 16.00)
    - Compression Ratio:     4.15x
    - Bandwidth Savings:     75.93%

[*] Executing ground-station reconstruction (lossless unpacking)...
[SUCCESS] Lossless verification passed! 100% of samples match exactly.
[SUCCESS] BTM v1.1 is fully functional, prefix-free, and ready for deployment.

====================================================================
📜 Academic and Research References
To read the theoretical foundations, detailed mathematical proofs, and industrial applications:
Applied Engineering Report (BTM Protocol v11):
Bressan, S. (2026). BTM v11: A Deterministic O(1) Zero-RAM Telemetry Compression Protocol for Space Systems via 2-Adic Valuation and Odd-Part Integer Decomposition. Zenodo.
📄 Technical Report (DOI: 10.5281/zenodo.22288196) (Resolves always to the latest version).
Mathematical Foundations (Bressan's Invariant Theorem):
Bressan, S. (2026). On the Invariant Ratio of 2-Adic Valuation and Odd Residue Transitions in Natural Integer Sequences. Zenodo.
📄 Mathematical Preprint (DOI: 10.5281/zenodo.22215083) (Resolves always to the latest version).
🌐 Open Science Commitment
This project is released under the Creative Commons Attribution 4.0 International (CC BY 4.0) license. We believe in collaborative, open, and accessible space engineering.
Aerospace organizations, private satellite operators (including Starlink/SpaceX, Kuiper, and OneWeb), and academic CubeSat teams are welcome to integrate, modify, and deploy this software in their onboard flight software (FSW) stacks without licensing barriers.

---
