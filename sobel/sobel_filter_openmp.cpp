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
#include <chrono>
#include <thread>
#include <deque>
#include <unistd.h>
#include <iostream>

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

    // std::cout << filename << "\n";

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
    const int BROADCAST_ROOT = 0;
    int size, rank;
    const char delimeter='/';
    std::string filename = "../video/input/out";
    std::string output_file = "../video/output/out";
    std::deque<const char*> dq;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank==0){
        std::string directory_path = "../video/input/";
        int frame_count = 0;
        
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            if (entry.path().extension() == ".bmp") {
                frame_count++;
            }
        }

        int recv;
        int buffer[frame_count];
        for (int i = 1; i < frame_count+1; ++i) {
            buffer[i] = i;
        }
        
        int num_frames = frame_count / size;

        std::cout << "f" << 
        int my_frames[num_frames];
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&num_frames, 1, MPI_INT, BROADCAST_ROOT, MPI_COMM_WORLD);
        MPI_Scatter(buffer, num_frames, MPI_INT, &my_frames, num_frames, MPI_INT, BROADCAST_ROOT, MPI_COMM_WORLD);
        
        for (int i = 0; i < num_frames; ++i) {
            // std::cout << "Element " << i << ": " << my_value[i] << std::endl;
            std::string f = filename;
            f += std::to_string(my_frames[i]); 
            f += ".bmp";
            std::string o = output_file;
            o += std::to_string(my_frames[i]); 
            o += ".bmp";
           
            // std::cout << f.c_str()<<std::endl;
            // std::cout << o.c_str()<<std::endl;

            do_stuff(f.c_str(),o.c_str());
        }
        
    }
    else {
        int num_frames;
        char name[MPI_MAX_OBJECT_NAME];
        int name_length;
        MPI_Get_processor_name(name,&name_length);

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&num_frames, 1, MPI_INT, BROADCAST_ROOT, MPI_COMM_WORLD);

        int my_frames[num_frames];

        MPI_Scatter(NULL, num_frames, MPI_INT, my_frames, num_frames, MPI_INT, 0, MPI_COMM_WORLD);

        for (int i = 0; i < num_frames; ++i) {
            // std::cout << "Element " << i << ": " << my_value[i] << std::endl;
            std::string f = filename;
            f += std::to_string(my_frames[i]); 
            f += ".bmp";
            std::string o = output_file;
            o += std::to_string(my_frames[i]); 
            o += ".bmp";
           
            // std::cout << f.c_str()<<std::endl;
            // std::cout << o.c_str()<<std::endl;

            do_stuff(f.c_str(),o.c_str());
        }
    }
    
    MPI_Finalize();    
}
