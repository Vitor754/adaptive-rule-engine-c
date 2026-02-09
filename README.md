# Network Topology Optimizer (OSPF & LACP)

## Context
Developed as part of the **Algorithms and Data Structures II (CES-11)** course at **Instituto Tecnológico de Aeronáutica (ITA)**.

## Objective
The goal of this project was to simulate a robust Internet Service Provider (ISP) network topology. The system calculates the most efficient routing paths based on bandwidth costs (mimicking the **OSPF** protocol) and handles physical link aggregation (**LACP**) to increase throughput.

## Technical Highlights
* **Language:** C (Focused on low-level memory management and struct optimization).
* [cite_start]**Graph Algorithms:** Implemented **Dijkstra's Algorithm** logic to find the "Shortest Path" (lowest cost) between routers based on the metric $Cost = ReferenceBandwidth / LinkBandwidth$[cite: 272].
* [cite_start]**Link Aggregation:** Simulated **LACP** (Link Aggregation Control Protocol) to dynamically sum bandwidths of parallel physical links into a single logical connection[cite: 279].
* [cite_start]**Load Balancing:** The engine handles "Equal Cost Multi-Path" scenarios, distributing traffic across multiple routes when costs are identical[cite: 277].

## Key Features
* [cite_start]**Dynamic Topology:** Supports adding/removing routers and links in runtime[cite: 293, 297].
* [cite_start]**Route Tracing:** The `TRACE` command outputs the optimal path(s) between any two nodes in the network[cite: 291].
* [cite_start]**Error Handling:** Validates Ethernet standards (10/100/1000 Mbps) and enforces LACP consistency rules[cite: 287, 289].

## How to Run
Compile the source code using GCC:
```bash
gcc main.c -o network_optimizer
./network_optimizer
