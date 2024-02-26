/*#include <iostream>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int sendbuf = rank;  // Cada proceso tiene un entero único para enviar

    if (rank == 0) {
        int recvbuf[size];  // Búfer de recepción en el proceso raíz
        MPI_Gather(MPI_IN_PLACE, 1, MPI_INT, recvbuf, 1, MPI_INT, 0, MPI_COMM_WORLD);

        // Proceso raíz imprime los enteros recolectados
        std::cout << "Proceso raíz ha recolectado los enteros: ";
        for (int i = 0; i < size; ++i) {
            std::cout << recvbuf[i] << " ";
        }
        std::cout << std::endl;
    } else {
        // Los otros procesos envían su entero al proceso raíz
        MPI_Gather(&sendbuf, 1, MPI_INT, nullptr, 0, MPI_INT, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
*/

/*
#include <iostream>
#include <vector>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int total_elements = 12;
    const int elements_per_proc = total_elements / size;

    // Vector con los elementos en el proceso raíz
    std::vector<int> root_data(total_elements);
    if (rank == 0) {
        for (int i = 0; i < total_elements; ++i) {
            root_data[i] = i + 1; // Valores del 1 al 12
        }
    }

    // Vector para almacenar los elementos locales de cada proceso
    std::vector<int> local_data(elements_per_proc);

    // Scatter los elementos del vector root_data entre los procesos
    MPI_Scatter(root_data.data(), elements_per_proc, MPI_INT,
                local_data.data(), elements_per_proc, MPI_INT,
                0, MPI_COMM_WORLD);

    // Calcular la suma local de los elementos en cada proceso
    int local_sum = 0;
    for (int i = 0; i < elements_per_proc; ++i) {
        local_sum += local_data[i];
    }

    // Reducir todas las sumas locales en la suma global en el proceso raíz
    int global_sum = 0;
    if (rank == 0) {
        MPI_Gather(MPI_IN_PLACE, 1, MPI_INT,
                   &global_sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Gather(&local_sum, 1, MPI_INT,
                   &global_sum, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Imprimir la suma global en el proceso raíz
    if (rank == 0) {
        std::cout << "La suma global de los elementos es: " << global_sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}*/





#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <mpi.h>
#include <cmath>
///workspace/mpi2324/datos.txt
std::vector<int> read_file() {
 std::fstream fs("datos.txt", std::ios::in );
 std::string line;
 std::vector<int> ret;
 while( std::getline(fs, line) ){
 ret.push_back( std::stoi(line) );
 }
 fs.close();
 return ret;
} 

std::vector<int> calcular_histograma(int* datos, int size) {
    std::vector<int> histo(101);
    for(int i=0; i<size;i++){
        histo[datos[i]]++;
    }
    return histo;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, nprocs;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int block_size;
    int real_size;
    int padding = 0;

    std::vector<int> data;
    std::vector<int> data_local;

    if(rank==0) {
        data = read_file();
        int max_items = data.size();

        real_size = max_items;

        if(max_items%nprocs!=0) {
            real_size = std::ceil((double)max_items/nprocs)*nprocs;
            block_size=real_size/nprocs;
            padding=real_size-max_items;
        }

        std::printf("RANK_%d: dimension: %d, real_size: %d, block_size: %d, padding: %d\n",rank,max_items,real_size,block_size,padding);
    }

    //enviar el tamaño del bloque
    MPI_Bcast( &block_size , 1 , MPI_INT , 0 , MPI_COMM_WORLD);

    if(rank==0){
        MPI_Send(&padding,1,MPI_INT,nprocs-1,0,MPI_COMM_WORLD);
    }
    else if(rank==nprocs-1) {
        MPI_Recv(&padding,1,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }

    data_local.resize(block_size);

    MPI_Scatter( data.data() , block_size , MPI_INT ,
                 data_local.data() , block_size, MPI_INT , 
                 0 , MPI_COMM_WORLD);


    //realizar el calculo
    int new_size = block_size;

    if(rank==nprocs-1) {
        new_size = block_size-padding;
    }

    std::printf("RANK_%d calculando histograma, block_size=  %d\n", rank , new_size);

    auto H_local = calcular_histograma(data_local.data(), new_size);

    //recibir los datos
    std::vector<int> H;

    if(rank==0) {
        H.resize(101);
    }

    MPI_Reduce( H_local.data() , H.data() , 101 , 
    MPI_INT , MPI_SUM , 0 , 
    MPI_COMM_WORLD);

    if(rank==0) {
        //imprimir el resultado
        for(int i=0; i<101;i++) {
            std::printf("%3d: %d\n", i, H[i]);        
            }
    }

    MPI_Finalize();
    return 0;
}

