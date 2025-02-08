Distributed Computing Programming Assignment 1

Executing the file:

Make sure that the input file (inp-params.txt) is in the same directory as the executable file.

VC:

(i) Compile the cpp file using following command:
mpicxx VC-CO21BTECH11002.cpp -o VC

(ii) Run the executable file with input parameter as number of processes + 1, using following command:
mpirun --oversubscribe -n N+1 VC

where N is the number of processes in the input file. For example, if N=15, then the command will be:

mpirun --oversubscribe -n 16 VC

The one extra process is the root process that handles logging and termination of the program.

SK:

(i) Compile the cpp file using following command:
mpicxx SK-CO21BTECH11002.cpp -o SK

(ii) Run the executable file with input parameter as number of processes + 1, using following command:
mpirun --oversubscribe -n N+1 SK

where N is the number of processes in the input file. For example, if N=15, then the command will be:

mpirun --oversubscribe -n 16 SK

The one extra process is the root process that handles logging and termination of the program.