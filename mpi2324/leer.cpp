/*
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <mpi.h>
#include <cmath>

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

int main(){

    auto vec = read_file();
int tam = vec.size();
std::printf("val: %d",tam);

    for(int i=0; i<20;i++){

   std::printf("val: %d",vec[i]);
    }

    return 0;
}*/


#include <iostream>
#include <mpi.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <random>

#define MAX 300

struct vector3D {
    int x, y, z;
};

int generarAleatorios() {
    std::random_device rd;
    std::mt19937 gen(rd());  
    std::uniform_int_distribution<> dis(1, 9);
    return dis(gen);
}

double distanciaCalculada(vector3D A, vector3D B) {
    double mod = 0.0;
    mod = (B.x - A.x) * (B.x - A.x) + (B.y - A.y) * (B.y - A.y) + (B.z - A.z) * (B.z - A.z);
    return std::sqrt(mod);
}

std::vector<int> distancia(std::vector<vector3D>& conjuntoLocal, std::vector<vector3D>& conjuntoB) {
    std::vector<int> indices;
    for (const auto& A : conjuntoLocal) {
        double minDist = distanciaCalculada(A, conjuntoB[0]);
        int minIndex = 0;
        for (size_t i = 1; i < conjuntoB.size(); ++i) {
            double dist = distanciaCalculada(A, conjuntoB[i]);
            if (dist < minDist) {
                minDist = dist;
                minIndex = i;
            }
        }
        indices.push_back(minIndex);
    }
    return indices;
}
int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    int rank, nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    std::vector<vector3D> conjuntoA;
    std::vector<vector3D> conjuntoLocal;
    std::vector<vector3D> conjuntoB(3);

    if (rank == 0) {
        conjuntoA.resize(MAX);
        for (int i = 0; i < MAX; i++) {
            conjuntoA[i].x = generarAleatorios();
            conjuntoA[i].y = generarAleatorios();
            conjuntoA[i].z = generarAleatorios();
        }

        std::cout << "Centros:" << std::endl;
        for (int i = 0; i < 3; i++) {
            conjuntoB[i].x = generarAleatorios();
            conjuntoB[i].y = generarAleatorios();
            conjuntoB[i].z = generarAleatorios();
            std::cout << "(" << conjuntoB[i].x << ", " << conjuntoB[i].y << ", " << conjuntoB[i].z << ")" << std::endl;
        }

        auto indices = distancia(conjuntoA, conjuntoB);


        for(int i=0;i<MAX;i++){
            //std::printf("\nindice: %d",indices[i]);
        }    
    }

    MPI_Bcast(conjuntoB.data(), 3, MPI_INT, 0, MPI_COMM_WORLD);

    int bloqSize = MAX / nprocs;
    conjuntoLocal.resize(bloqSize);
    std::vector<int> localIndices;

    MPI_Scatter(conjuntoA.data(), bloqSize, MPI_INT, conjuntoLocal.data(), bloqSize, MPI_INT, 0, MPI_COMM_WORLD);

    localIndices = distancia(conjuntoLocal, conjuntoB);
    std::vector<int> indicesGlobal(MAX);
    MPI_Gather(localIndices.data(), bloqSize, MPI_INT, indicesGlobal.data(), bloqSize, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < MAX; i++) {
            std::cout << "Índice " << i << ": " << indicesGlobal[i] << std::endl;
        }
    }

    MPI_Finalize();

    return 0;
}