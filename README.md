Andrea Miller

Mike Watson

Parallel Go

Create an algorithm to simulate a 9x9 board game of Go using influence maps, Depth First Search, and score to make intelligent decisions. 
The decision making process is parallelized using MPI and OpenMP.
There is a serial implementation in go_Serial and parallel in go_MPI.

Popt, MPI, and OpenMP is needed to run this program. 
Parallel implementation must be run with 82 processes.


To Build: 

make

To list command line arguments: 

./go_Serial --help

mpiexec -np 1 ./go_MPI --help