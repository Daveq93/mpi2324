#include<iostream>
#include<mpi.h>

int sumar(int* tmp, int n){
    int suma=0;
    for(int i=0;i<n;i++){
          suma+=tmp[i];
    }
    return suma;
}

int main(int argc, char ** argv){

  MPI_Init(&argc,&argv);
 
    int rank, nprocs;
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
   
  //  std::printf("Rank %d of %d process \n",rank,nprocs);
   
    int data[100];
   
    if(rank == 0){
        std::printf("Total de ranks: %d\n", nprocs);
 
        for(int i=0;i<100;i++){
            data[i]=i;
        }
        
        for(int rank_id=1;rank_id<nprocs; rank_id++){
            std::printf("RANK_0 enviando datos a RANK_%d\n",rank_id);
          
          int start = rank_id*25;
            MPI_Send(
            &data[start], //datos
            25, //cuantos datos
            MPI_INT, //tipo de dato
            rank_id, // rank destino
            0, //tag
            MPI_COMM_WORLD //grupo
            ); 
        }
int suma_ranks[4];

suma_ranks[0] = sumar(data,25);
for(int rank_id=1; rank_id<nprocs;rank_id++){
    MPI_Recv(&suma_ranks[rank_id],1,MPI_INT,rank_id,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
}
        std::printf("sumas parciales: %d %d %d %d\n",suma_ranks[0],suma_ranks[1],suma_ranks[2],suma_ranks[3]);
        
        int suma_total = sumar(suma_ranks,4);
        
        std::printf("SUMA TOTAL: %d\n",suma_total);
    
    }else {
        std::printf("RANK_%d Recibiendo datos\n ",rank);
        MPI_Recv(
            data, //datos
            100, //cuantos
            MPI_INT, //tipo de dato
            0, //rank - origen
            0, //tag
            MPI_COMM_WORLD, //grupo
            MPI_STATUS_IGNORE); //status

          int suma_parcial = sumar(data,25);
          std::printf("RANK_%d enviando suma parcial %d\n",rank,suma_parcial);
          MPI_Send(&suma_parcial,1,MPI_INT,0,0,MPI_COMM_WORLD);

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