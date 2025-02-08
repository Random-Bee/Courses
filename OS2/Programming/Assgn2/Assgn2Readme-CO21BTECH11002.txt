OS2 Assignment 2

This program checks whether a given sudoku is valid or not using multithreading.
Input is given using command line argument. The program takes the name of file containing value of n (size of sides of sudoku) and k (no. of threads to use) as input.

Executing Pthread file:
(i) Compile the cpp file using following command:
g++ -pthread -lm Assgn2SrcPthread-CO21BTECH11002.c -o Assgn2SrcPthread-CO21BTECH11002.out

(ii) Run the executable file with the name of input file as the command line argument using following command:
./Assgn2SrcPthread-CO21BTECH11002.out inp.txt


Executing OpenMP file:
(i) Compile the cpp file using following command:
g++ -fopenmp -lm Assgn2SrcOpenMp-CO21BTECH11002.c -o Assgn2SrcOpenMP-CO21BTECH11002.out

(ii) Run the executable file with the name of input file as the command line argument using following command:
./Assgn2SrcOpenMP-CO21BTECH11002.out input.txt
