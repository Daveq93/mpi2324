#include <iostream>
#include <mpi.h>
#include <cmath>
#include <vector>

#define MAX_ITEMS 25

int sumar (int* tmp, int n,int rank){
    
    int suma=0;
for(int i =0;i<n;i++){
    std::printf("datos a sumar: %d, en rank: %d\n",tmp[i],rank);
    suma = suma + tmp[i];
}
return suma;

}

int main(int argv, char **argc){

MPI_Init(&argv,&argc);

int rank,nprocs;

MPI_Comm_rank (MPI_COMM_WORLD,&rank);
MPI_Comm_size (MPI_COMM_WORLD,&nprocs);

//std::printf("Rank_%d de %d procesos\n",rank,nprocs);

int block_size;
int real_size;
int padding=0;

if(MAX_ITEMS % nprocs != 0){
    real_size = std::ceil((double)MAX_ITEMS/nprocs) * nprocs;
    block_size = real_size/nprocs;
    padding = real_size-MAX_ITEMS;
}else{
    real_size = MAX_ITEMS;
    block_size = MAX_ITEMS/nprocs;
}

std::vector<int> data(real_size);
std::vector<int> data_local(block_size);
int suma_total=0;
/*
if(rank==0){
    std::printf("Dimension %d, real_size: %d, block_size: %d, padding: %d\n",MAX_ITEMS,real_size,block_size,padding);

    //inicializar datos
    for(int i=0; i<MAX_ITEMS;i++){
        data[i] = i+1;
    }
}

MPI_Scatter(data.data(), block_size, MPI_INT,
            data_local.data(),block_size,MPI_INT,
            0,MPI_COMM_WORLD );

if(rank==nprocs-1){
    block_size = block_size-padding;
}

int suma_parcial = sumar(data_local.data(),block_size);
std::printf("Rank_%d, suma parcial: %d\n",rank,suma_parcial);

//enviar la suma parcial al rank 0
int suma_total = 0;
MPI_Reduce(&suma_parcial,&suma_total,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

if(rank==0){
    std::printf("Suma total: %d\n",suma_total);
}
*/
/*
if(rank==0){
    
    int suma_local =0;
    std::printf("Dimension: %d, real_size: %d, block_size: %d, padding: %d\n", MAX_ITEMS, real_size,
                    block_size, padding);

    //inicializar datos
    for(int i = 0;i<MAX_ITEMS;i++){
        data[i]=i+1;
    }                

    std::printf("primer elemento: %d\n",data[MAX_ITEMS-1]);


    MPI_Scatter(data.data(),block_size,MPI_INT,
                data_local.data(),block_size,MPI_INT,
                0,MPI_COMM_WORLD
    );

    suma_local = sumar(data_local.data(),block_size);

    MPI_Gather(MPI_IN_PLACE,0,MPI_INT,
               &suma_local,1,MPI_INT,
               0,MPI_COMM_WORLD
    );
    std::printf("Suma local: %d\n",suma_local);

}else{
    if(rank==nprocs-1){
        block_size=block_size-padding;
    }

    int suma_parcial=sumar(data_local.data(),block_size);
    std::printf("Rank_%d: suma parcial= %d\n",rank,suma_parcial);

    MPI_Reduce(&suma_parcial,&suma_total,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

}

MPI_Barrier(MPI_COMM_WORLD);

if(rank==0){
    std::printf("Suma total: %d\n",suma_total);
}
*/

if (rank == 0) {
        int suma_local = 0;
        std::printf("Dimension: %d, real_size: %d, block_size: %d, padding: %d\n", MAX_ITEMS, real_size,
                     block_size, padding);

        // Inicializar datos
        for (int i = 0; i < MAX_ITEMS; i++) {
            data[i] = i + 1;
        }

        MPI_Scatter(data.data(), block_size, MPI_INT,
                    data_local.data(), block_size, MPI_INT,
                    0, MPI_COMM_WORLD);

        suma_local = sumar(data_local.data(), block_size, rank);

        MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, // Usando MPI_IN_PLACE
                   &suma_local, 1, MPI_INT,
                   0, MPI_COMM_WORLD); // Recolecta las sumas locales en suma_local
        std::printf("Suma local: %d\n", suma_local);
    } else {
        if (rank == nprocs - 1) {
            block_size = block_size - padding;
        }

        int suma_parcial = sumar(data_local.data(), block_size, rank);
        std::printf("Rank_%d: suma parcial= %d\n", rank, suma_parcial);

        MPI_Gather(&suma_parcial, 1, MPI_INT, // Recolecta la suma parcial en el proceso raíz
                   nullptr, 0, MPI_DATATYPE_NULL,
                   0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        MPI_Reduce(MPI_IN_PLACE, &suma_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        std::printf("Suma total: %d\n", suma_total);
    }

    MPI_Finalize();

return 0;
}