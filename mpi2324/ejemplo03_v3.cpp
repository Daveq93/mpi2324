#include<iostream>
#include<mpi.h>
#include <cmath>
#include<vector>

#define MAX_ITEMS 25

int sumar(int *tmp, int n) {
    int suma = 0;
    for (int i = 0; i < n; i++) {
        suma = suma+tmp[i];
    }
    return suma;
}

//el in place es como enviar null
int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   int block_size;
   int real_size;
   int padding = 0;

    if (MAX_ITEMS % nprocs != 0) {
        real_size = std::ceil((double) MAX_ITEMS / nprocs) * nprocs;
        block_size = real_size/nprocs;
        padding = real_size - MAX_ITEMS;
    }

    std::vector<int> data;
    std::vector<int> data_local;


    if(rank==0){
        std::printf("Dimension: %d, real_size: %d, block_size: %d, padding: %d\n", MAX_ITEMS, real_size,
                    block_size, padding);
        //inicializar
        for(int i=0;i<MAX_ITEMS;i++){
            data[i] = i;
        }
    }

    MPI_Scatter(data.data(),block_size,MPI_INT, //encio
                 data_local.data(),block_size,MPI_INT,//que envia
                 0,MPI_COMM_WORLD); //que rank

                 //calcular la suma parcial
    if(rank==nprocs-1){
        block_size = block_size-padding;
    }

    int suma_parcial = sumar(data_local.data(),block_size);
    std::printf("RANK_%d: suma parcial= %d\n",rank,suma_parcial);

    //enviar la suma parcial al Rank_0
    int suma_total = 0;
    MPI_Reduce(&suma_parcial,&suma_total,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    //imprimir el resultado
    if(rank==0){
        std::printf("SUMA TOTAL: %d\n",suma_total);
    }

    MPI_Finalize();

    return 0;
}
