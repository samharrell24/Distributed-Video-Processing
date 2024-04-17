#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
#include "stb_image_write.h"    
#include <string>
#include <omp.h>
#include <filesystem>
#include <mpi.h>
#include <iostream>
#include <math.h>

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
    // if (argc != 2){
    //     std::cerr << "Invalid number of arguments.\n./bootstrap.sh video.mp4 outputname.mp4";
    //     exit(1);
    // }
    

    const int BROADCAST_ROOT = 0;
    int size, rank;
    std::string filename = "../video/input/out";
    std::string output_file = "../video/output/out";

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank==0){
        std::string directory_path = "../video/input/";
        int bmp_count = 0;
        
        for (const auto& entry : fs::directory_iterator(directory_path)) {
            if (entry.path().extension() == ".bmp") {
                bmp_count++;
            }
        }

        int remainder = bmp_count % size;
        int num_dummies_to_add = (size - remainder) % size;

        std::cout << "rem:" << remainder << "\n";
        std::cout << "dum:" << num_dummies_to_add << "\n";


        int frame_count = bmp_count+num_dummies_to_add;
        int recv;
        int buffer[frame_count];    

        int count = num_dummies_to_add;
        for (int i = 0; i < frame_count+1; ++i) {
            if (count > 0){
                buffer[i] = 0;
                count -= 1;
            }
            else{
                buffer[i] = i+1;
            }
        }

        for (int i = 0; i < frame_count; ++i) {
            std::cout<<buffer[i];
        }

        int num_frames = frame_count / size;
        
        // How to evenly distribute and what to do if not even. Dead frames?
        int my_frames[num_frames];
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&num_frames, 1, MPI_INT, BROADCAST_ROOT, MPI_COMM_WORLD);
        MPI_Scatter(buffer, num_frames, MPI_INT, &my_frames, num_frames, MPI_INT, BROADCAST_ROOT, MPI_COMM_WORLD);
        
        for (int i = 0; i < num_frames; ++i) {
            if (my_frames[i] == 0){
                std::cout << "R" << rank << ": " << my_frames[i] << std::endl;
                continue;
            }
            std::cout << "R" << rank << ": " << my_frames[i] << std::endl;
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
            if (my_frames[i] == 0){
                std::cout << "R" << rank << ": " << my_frames[i] << std::endl;
                continue;
            }
            std::cout << "R" << rank << ": " << my_frames[i] << std::endl;
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
