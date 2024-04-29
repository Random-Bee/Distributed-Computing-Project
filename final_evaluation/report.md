---
title: Distributed Hashing - Final Report
author: 
- Aayush Kumar (CO21BTECH11002)
- Vishal Vijay Devadiga (CS21BTECH11061)
geometry: margin=1.5cm
documentclass: extarticle
fontsize: 12pt
header-includes:
    - \usepackage{setspace}
    - \onehalfspacing
---

# Table of Contents

- [Table of Contents](#table-of-contents)
- [Introduction](#introduction)
  - [Problem Statement](#problem-statement)
  - [Objective](#objective)
- [Approach](#approach)
- [Implementation](#implementation)
- [Evaluation](#evaluation)
  - [Graphs](#graphs)
- [Conclusion](#conclusion)

\newpage

# Introduction

What is **Distributed Hashing?**

Distributed hashing is a technique used in distributed computing environments to efficiently store and retrieve data across multiple nodes in a network. It combines the principles of hashing, which involves mapping data to a fixed-size value, with distribution, where data is spread across multiple nodes or machines.

Here's how it typically works:

1. **Hashing**: Data is hashed using a hash function, which generates a unique identifier (hash) for each piece of data. This hash serves as the key for storing and retrieving the data.

2. **Distribution**: The hashed data is distributed across multiple nodes in the network. Each node is responsible for storing a subset of the hashed data based on a predetermined mapping scheme.

Distributed hashing is used in various distributed systems and applications for several reasons:

1. **Scalability**: It allows systems to scale horizontally by adding more nodes to the network. As the amount of data increases or the workload grows, additional nodes can be added to handle the load, without affecting the overall performance.

2. **Fault Tolerance**: Distributed hashing provides fault tolerance by replicating data across multiple nodes. If a node fails or becomes unavailable, the data can still be accessed from other nodes, ensuring high availability and reliability.

3. **Load Balancing**: By distributing data across multiple nodes, distributed hashing helps in balancing the workload evenly across the network. This prevents any single node from becoming a bottleneck and ensures optimal performance.

4. **Reduced Latency**: Data can be stored and retrieved from nodes that are geographically closer to the user, reducing latency and improving response times.

Distributed hashing is used in various distributed systems and applications, including distributed databases, content delivery networks (CDNs), peer-to-peer (P2P) networks, and distributed file systems like Hadoop Distributed File System (HDFS) and Amazon S3.

## Problem Statement

In this project

## Objective

# Approach

# Implementation

# Evaluation

## Graphs
1) Number of hops
2) Time
3) Throughput
4) Load balancing

# Conclusion
