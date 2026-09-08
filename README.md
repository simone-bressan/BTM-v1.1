# BTM (Bressan Telemetry Mapping) v1.1

[![Language](https://img.shields.io/badge/Language-C99-blue.svg)](#)
[![License](https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by/4.0/)
[![ESA OSIP](https://img.shields.io/badge/ESA%20OSIP-Candidate%20I--2026--14190-orange.svg)](#)
[![Preprint DOI](https://img.shields.io/badge/Preprint%20DOI-10.5281%2Fzenodo.22215083-blue.svg)](https://doi.org/10.5281/zenodo.22215083)
[![Technical Report DOI](https://img.shields.io/badge/Protocol%20DOI-10.5281%2Fzenodo.22288196-green.svg)](https://doi.org/10.5281/zenodo.22288196)

Official C99 reference implementation of **BTM (Bressan Telemetry Mapping) v1.1**, an ultra-lightweight software protocol designed for real-time telemetry compression on CubeSats and small satellites operating under strict resource constraints.

The framework is based on the algebraic properties of **Bressan's Invariant Ratio Theorem** (2-adic valuation and odd-part integer transitions of gaps), formally registered on Zenodo. The protocol is currently submitted as a candidate proposal to the European Space Agency's **ESA OSIP (ID: I-2026-14190)** for formal benchmarking against legacy standards like CCSDS 121.0-B.

---

## 🧠 Core Features & Architecture

BTM v1.1 translates pure number theory into a practical, highly robust physical-layer encoding solution:

* **Deterministic $O(1)$ Complexity:** Execution time remains strictly constant for every single sample, ensuring predictable latency and timing jitter. This is critical for Real-Time Operating Systems (RTOS) and Guidance, Navigation, and Control (GNC) loops.
* **Zero Dynamic RAM Allocation:** The algorithm processes incoming data on-the-fly directly within CPU registers.
* **Radiation-Hardened by Design (SEU Mitigation):** By eliminating memory buffers, accumulation tables, and dynamic lookups in RAM, the code drastically reduces the software's vulnerable state surface, mitigating the risk of Single Event Upsets (SEUs) caused by cosmic rays in LEO.
* **100% Lossless Verification:** The symmetric 2-adic decomposition and ground-station reconstruction guarantee bit-perfect data integrity under all operational conditions.

---

## 📊 Experimental Benchmark Results

The integrated test-bench simulates a realistic orbital thermal telemetry dataset (subject to large day/night temperature swings) combined with additive Gaussian noise. 

The results verify perfect reconstruction against uncompressed 16-bit integers:
* **Nominal Orbital Conditions:** Average size of **3.22 bits/sample**, achieving a **73.16%** bandwidth and transmitter power saving.
* **High Thermal Noise (Gaussian):** Average size of **7.00 bits/sample**, achieving a **56.27%** bandwidth saving with 100% lossless verification passed.

---

## 🚀 Quick Start & Benchmarking

### Option A: Run on Google Colab (One-Click Cloud Execution)
You can compile and run the benchmark instantly in a cloud-hosted Linux environment:

1. Open a blank notebook on [Google Colab](https://colab.research.google.com).
2. Copy, paste, and run the following commands:

```bash
!git clone https://github.com/simone-bressan/BTM-v1.1.git
%cd BTM-v1.1
!gcc -O3 main.c -lm -o btm_benchmark
!./btm_benchmark
Option B: Local Compilation (Linux / macOS)
Clone the repository and compile using any standard C compiler (GCC or Clang):
git clone https://github.com/simone-bressan/BTM-v1.1.git
cd BTM-v1.1
gcc -O3 main.c -lm -o btm_benchmark
./btm_benchmark
📂 Project Structure
btm.h: Header library containing the 2-adic mathematical decomposition logic and bit-level packing routines.
main.c: Complete benchmarking suite generating simulated orbital telemetry, executing the real-time compression, and verifying reconstruction.
LICENSE: Creative Commons Attribution 4.0 International (CC BY 4.0).
🤝 Scientific Context & Open Science Commitment
In accordance with Open Science standards, all theoretical proofs, reference implementations, and datasets are published openly to allow for public peer-review and immediate aerospace integration.
Author: Simone Bressan (Telecommunications Technician, Independent Researcher)
Location: Cormons (GO), Italia
Email: bressanpolame@gmail.com
Mathematical Preprint (Zenodo DOI): 10.5281/zenodo.22215083
Technical Protocol Report (Zenodo DOI): 10.5281/zenodo.22288196

---
