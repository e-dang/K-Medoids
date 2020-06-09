# K-Mediods

## Description
KMediods library written in C++17 using the Partition Around Mediods (PAM) BUILD and SWAP algorithms, as well as the CLARA approximation algorithm. The PAM algorithms have been implemented for both serial and multi-threaded execution, whereas the CLARA algorithm has been implemented for serial, multi-threaded, distributed, and hybrid (multi-threaded and distributed) execution. Both the parallelism and the distance function can be changed statically at compile time. Benchmarks run on a centOS cluster can be seen below:
![](results/plots/omp_pam.png)
![](results/plots/omp_clara.png)
![](results/plots/mpi_clara.png)
![](results/plots/hybrid_clara.png)

## Dependencies
- C++17
- Boost 1.71.0
- OpenMPI 4.0.2
- OpenMP compatible C++ compiler


## Compiling
Tested on MacOS Mojave 10.14.6 and CentOS with compilers:
- Clang 9.0.1
- GCC 7.2.0

```
mkdir build
cd build
cmake ..
make
```

## Authors
Eric Dang