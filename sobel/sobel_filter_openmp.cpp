#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
#include "stb_image_write.h"    
#include <cmath>
#include <string>
#include <omp.h>
#include <filesystem>
#include <sstream>
#include <vector>
#include <mpi.h>
#include <deque>

namespace fs = std::filesystem;

void check_file(char const *filename){
    // Using & allows for pass by reference and gurantees not a NULL value
    // char const *filename is a char [] pointer which is preferrable over a std::string
    // https://stackoverflow.com/questions/1287306/difference-between-string-and-char-types-in-c
    int x, y, n;
    int result = stbi_info(filename, &x, &y, &n);
    std::cout << result << "\n";
}



void do_stuff(char const *filename, char const *outputfile){
    int x,y,n;
    // last parameter forces number of desired channels. We only want grey so we set it to 1

    std::cout << filename << "\n";

    unsigned char* data = stbi_load(filename, &y, &x, &n, 1);

    // if(data == NULL){
    //     printf("Error in loading image");
    //     exit(1);
    // }
    // printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", y, x, n);

    unsigned char nestedArray[x][y];

    // Pre-increment (++row)
    // int row1 = 5;
    // int result1 = ++row1;  **Increment row first, then use the updated value

    // // Post-increment (row++)
    // int row2 = 5;
    // int result2 = row2++;  **Use row's current value first, then increment row

    // Copy into [][] for ease of iteration
    // TODO: refactor to be more efficent
    for(int row = 0; row<x; ++row){
        for(int col = 0; col<y; ++col){
            nestedArray[row][col] = data[row*y+col];
        }
    }

    int kernelX[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int kernelY[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    int gradientX,gradientY,row,col,j,k;
    int G_magnitude = 0;
    int th_id;

    #pragma omp parallel for private(row,col,gradientX,gradientY,j,k,G_magnitude)
    for(int row = 1; row<x-1; ++row){
            // th_id = omp_get_thread_num();
            // printf("%d",th_id);
        for(int col = 1; col<y-1; ++col){
                gradientX = 0;
                gradientY= 0;
            for(int j=-1;j<=1;++j){             
                for(int k=-1;k<=1;++k){
                    gradientX += nestedArray[row+j][col+k] * kernelX[j+1][k+1];
                    gradientY += nestedArray[row+j][col+k] * kernelY[j+1][k+1];
                }
            }
            G_magnitude = sqrt(gradientX*gradientX + gradientY*gradientY);
            data[row*y+col] = std::min(std::max(G_magnitude,0),255);
        }
    }

    stbi_write_bmp(outputfile, y, x, 1, data);
    stbi_image_free(data);
}

int main(int argc, char *argv[]){
    int p, rank;
    const char delimeter='/';
    std::string path = "../video/snail_frames";
    std::string output_path = "../video/snail_frames";
    std::deque<const char*> dq;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank==0){
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry)) {
                std::cout << entry.path().filename() << '\n';
                const char* fn = entry.path().c_str();
                // std::string o = output_path+entry.path().filename().string();
                // const char* outputfile = o.c_str();
                
                std::cout << fn;
                dq.push_back(fn);
            }   
        }

        std::cout << "Main Thing @ Rank: "<<rank<<"\n";
        MPI_Barrier(MPI_COMM_WORLD);
    }
    else {
        MPI_Barrier(MPI_COMM_WORLD);
        std::cout << "Other Thread @ Rank: "<<rank<<"\n";
        // const char* file_bmp = dq.front();
        // dq.pop_front();

        // std::string appended = file_bmp;
        // appended += ".bmp";
        // const char* output_file = appended.c_str();

        // std::cout << output_file;
        // std::cout<< file_bmp << " " << rank;

        // do_stuff(file_bmp,output_file);
        // MPI_Barrier(MPI_COMM_WORLD);

        
    }
    
    MPI_Finalize();    
}
