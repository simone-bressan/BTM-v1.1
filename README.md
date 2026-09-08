# BTM (Bressan Telemetry Mapping) v1.1

[![Language](https://img.shields.io/badge/Language-C99-blue.svg)](#)
[![License](https://img.shields.io/badge/License-CC%20BY%204.0-lightgrey.svg)](https://creativecommons.org/licenses/by/4.0/)
[![ESA OSIP](https://img.shields.io/badge/ESA%20OSIP-Candidate%20I--2026--14190-orange.svg)](#)
[![Preprint DOI](https://img.shields.io/badge/Preprint%20DOI-10.5281%2Fzenodo.22215083-blue.svg)](https://doi.org/10.5281/zenodo.22215083)
[![Technical Report DOI](https://img.shields.io/badge/Protocol%20DOI-10.5281%2Fzenodo.22288196-green.svg)](https://doi.org/10.5281/zenodo.22288196)

Implementazione ufficiale di riferimento in **C99 standard** di **BTM (Bressan Telemetry Mapping) v1.1**, un protocollo software di codifica ultra-leggero per la compressione lossless di telemetria a bordo di CubeSat e piccoli satelliti operanti in ambienti a risorse limitate.

Il framework si basa sulle proprietà algebriche del **Teorema del Rapporto Invariante di Bressan** (scomposizione 2-adica e odd-part dei gap tra interi), registrato formalmente su Zenodo. Il protocollo è attualmente sottomesso alla piattaforma **ESA OSIP (ID: I-2026-14190)** per la valutazione e il benchmarking formale rispetto agli standard esistenti (come il CCSDS 121.0-B).

---

## 🧠 Caratteristiche dell'Architettura Software

L'algoritmo è stato progettato come soluzione di livello fisico per ottimizzare la trasmissione dati riducendo al minimo l'impatto computazionale dell'hardware di bordo:

* **Complessità deterministica $O(1)$:** Il tempo di esecuzione dell'algoritmo è fisso per ciascun campione, garantendo una latenza prevedibile essenziale per i sistemi operativi in tempo reale (RTOS) e i cicli di controllo GNC.
* **Zero Allocazione di RAM Dinamica:** L'algoritmo opera direttamente all'interno dei registri della CPU.
* **Mitigazione dei guasti SEU (Single Event Upset):** Non utilizzando buffer di memoria RAM per l'accumulo temporaneo o tabelle di codifica dinamiche, il software riduce drasticamente il rischio di crash o corruzioni causati dalle radiazioni ionizzanti nello spazio profondo o in orbita LEO.
* **Simmetria di ricostruzione 100% Lossless:** La scomposizione e la successiva estrazione a terra tramite stazione di ricezione garantiscono la conservazione perfetta di ogni bit trasmesso.

---

## 📊 Risultati Sperimentali del Benchmark

Il test-bench incluso simula un flusso reale di telemetria termica orbitale (forti oscillazioni giorno/notte) con l'aggiunta di rumore termico gaussiano, confrontando la trasmissione compressa rispetto a quella a 16-bit non compressa:

* **Efficienza in condizioni nominali (assenza di rumore pesante):** Consumo medio di **3.22 bit per campione**, equivalente a un risparmio teorico di banda e di potenza del trasmettitore radio fino al **73.16%**.
* **Efficienza sotto forte rumore gaussiano:** Consumo medio di **7.00 bit per campione**, con un risparmio netto del **56.27%** e una ricostruzione lossless verificata al 100%.

---

## 🚀 Come Eseguire il Benchmark

### Opzione A: Avvio rapido con un clic su Google Colab
Puoi testare istantaneamente il comportamento dell'algoritmo compilando ed eseguendo il codice direttamente sul cloud attraverso una macchina virtuale Linux pronta all'uso:

1. Apri un nuovo notebook su [Google Colab](https://colab.research.google.com).
2. Copia e avvia la seguente cella di codice:

```bash
!git clone https://github.com/simone-bressan/BTM-v1.1.git
%cd BTM-v1.1
!gcc -O3 main.c -lm -o btm_benchmark
!./btm_benchmark
Opzione B: Compilazione locale (Linux / macOS)
Clona il repository ed esegui la compilazione tramite un comune compilatore C standard (come GCC o Clang):
git clone https://github.com/simone-bressan/BTM-v1.1.git
cd BTM-v1.1
gcc -O3 main.c -lm -o btm_benchmark
./btm_benchmark
📂 Struttura del Progetto
btm.h: Header library contenente la logica matematica di scomposizione 2-adica e le funzioni di impacchettamento bit-level.
main.c: Test-bench per la generazione della telemetria orbitale simulata, l'esecuzione del compression-packing e la verifica di integrità a terra.
LICENSE: Licenza ufficiale d'uso Creative Commons Attribution 4.0 International (CC BY 4.0).
🤝 Contatti e Riferimenti Accademici
Autore della proposta: Simone Bressan (Tecnico delle Telecomunicazioni, Ricercatore Indipendente)
Sede: Cormons (GO), Italia
Email: bressanpolame@gmail.com
Teorema del Rapporto Invariante (Zenodo Preprint DOI): 10.5281/zenodo.22215083
Protocollo BTM v11 (Zenodo Technical Report DOI): 10.5281/zenodo.22288196
