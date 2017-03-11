This program uses the MPI package to create a parallelized solution for any system of equations in which the number of unknowns is divisible by the number of cores. It also includes a sequential version of the code.

To run, use ./gengs <number of unknowns> <desired error> to generate a input text file named <number of unknowns>.txt, or simply use the provided input.txt.

install mpi package if not already installed, preferably with homebrew.

Compile using the command:
mpicc -o gs gs.c

Then run:
mpirun -n <number of cores> gs inputfile.txt

Output should be the new unknowns and number of iterations it took!