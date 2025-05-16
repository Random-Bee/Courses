This repository contains the implementation of various concurrency control algorithms for database systems. The algorithms implemented are: BTO, MVTO, MVTO-GC, and K-MVTO.

inp-params.txt:
n, m, totTrans, numIters, constVal, lambda, readRatio

Ensure that the .h, .cpp and inp-params.txt files are in the same directory.

BTO:

Compile the code using the following command:

g++ BTO-Test-CO21BTECH11002.cpp BTO-CO21BTECH11002.cpp -std=c++17 -pthread -o bto

Run the code using the following command:

./bto

MVTO:

Compile the code using the following command:

g++ MVTO-Test-CO21BTECH11002.cpp MVTO-CO21BTECH11002.cpp -std=c++17 -pthread -o mvto

Run the code using the following command:

./mvto

MVTO-GC:

Compile the code using the following command:

g++ MVTO-GC-Test-CO21BTECH11002.cpp MVTO-GC-CO21BTECH11002.cpp -std=c++17 -pthread -o mvto_gc

Run the code using the following command:

./mvto_gc

K-MVTO:

Compile the code using the following command:

g++ K-MVTO-Test-CO21BTECH11002.cpp K-MVTO-CO21BTECH11002.cpp -std=c++17 -pthread -o k_mvto

Run the code using the following command:

./k_mvto