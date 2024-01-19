#include<iostream>
#include<mpi.h>
#include <cstdio>
#include <cmath>

int sumar(int *tmp, int n) {
    int suma = 0;
    for (int i = 0; i < n; i++) {
        suma += tmp[i];
    }
    return suma;
}

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

   // std::printf("Rank %d of %d process \n",rank,nprocs);

    int n =101;

    double div = n / nprocs;
    int resto = n  % nprocs;
    int block_size = std::round(div);

    std::printf("resto = %d\n",resto);
    std::printf("division= %.3f block_size= %d\n",div,block_size);
    int data[n];

    if (rank == 0) {
        std::printf("Total de ranks: %d\n", nprocs);

        for (int i = 0; i < n; i++) {
            data[i] = i;
        }

        for (int rank_id = 1; rank_id < nprocs; rank_id++) {
            std::printf("RANK_0 enviando datos a RANK_%d\n", rank_id);

            int start = (rank_id * block_size)+resto;
            std::printf("-------------  Starts enviados: %d\n",start);
            MPI_Send(
                    &data[start], //datos
                    block_size, //cuantos datos
                    MPI_INT, //tipo de dato
                    rank_id, // rank destino
                    0, //tag
                    MPI_COMM_WORLD //grupo
            );
        }
        int suma_ranks[4];

         int suma_rank0_tam = block_size+resto;
        std::printf("elementos sumados por el rank 0 = %d\n",suma_rank0_tam);
        suma_ranks[0] = sumar(data, suma_rank0_tam);

        for (int rank_id = 1; rank_id < nprocs; rank_id++) {
            MPI_Recv(&suma_ranks[rank_id], 1, MPI_INT, rank_id, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        std::printf("sumas parciales: %d %d %d %d\n", suma_ranks[0], suma_ranks[1], suma_ranks[2], suma_ranks[3]);

        int suma_total = sumar(suma_ranks, 4);

        std::printf("SUMA TOTAL: %d\n", suma_total);

    } else {
        std::printf("RANK_%d Recibiendo datos\n ", rank);
        MPI_Recv(
                data, //datos
                n, //cuantos
                MPI_INT, //tipo de dato
                0, //rank - origen
                0, //tag
                MPI_COMM_WORLD, //grupo
                MPI_STATUS_IGNORE); //status

        int suma_parcial=0;

                 if(rank==0){
                   suma_parcial=  sumar(data,block_size+resto);
                 }else{
                     if(rank==3){
                         int tm = sizeof(data) / sizeof(data[0]);
                         std::printf("size del data Rank 3 = %d",tm);
                     }
                     suma_parcial = sumar(data, block_size);
                 }

        std::printf("RANK_%d enviando suma parcial %d\n", rank, suma_parcial);
        MPI_Send(&suma_parcial, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        //  std::string str= "";

        // for(int i=0;i<10;i++){
        // std::printf("%d, ",data[i]);
        //     str = str+std::to_string(data[i])+",";
        // }
        //  std::printf("RANK_%d recibiendo datos==>%s\n",rank,str.c_str());

    }

    MPI_Finalize();

    return 0;
}
