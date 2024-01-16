# mpi2324

##Instalacion MPI en Ubuntu

sudo apt-get update
sudo apt-get install libopenmpi-dev

## Compilacion

mpicxx ejemplo02.cpp -o ejemplo02

##Ejecutar

mpiexec -n 4 ./ejemplo02

