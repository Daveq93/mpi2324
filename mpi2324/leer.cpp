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
}