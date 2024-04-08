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

    std::cout << outputfile << "\n";


    stbi_write_bmp(outputfile, y, x, 1, data);
    stbi_image_free(data);
}

int main(int argc, char *argv[]){
    int p, rank;
    const char delimeter='/';
    std::string path = "../video/snail_frames";
    std::string output_path = "../video/snail_frames/out%d.bmp";
    std::deque<const char*> dq;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // MPI_Request req;
    // MPI_Status stat;
    int my_value[100];
    char name[MPI_MAX_OBJECT_NAME];
    int name_length;


    if (rank==0){
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_regular_file(entry)) {
                // std::cout << entry.path().filename() << '\n';
                const char* fn = entry.path().c_str();               
                // std::cout << fn;
                dq.push_back(fn);
            }   
        }
        // 20 -> 1

        int dq_size = dq.size();

        std::cout << "Main Thing @ Rank: "<<rank<<" dq_size: "<<dq_size<<" \n";
        // std::cout << dq_size;
        MPI_Barrier(MPI_COMM_WORLD);

        // while(dq_size > 0){
        //     const char* file_bmp = dq.front();
        //     dq.pop_front();
        //     dq_size -= 1;
        //     MPI_Bcast(&dq_size, 1, MPI_INT, 0, MPI_COMM_WORLD);    
        // }

        // 20,19,18,17 till 0 on all nodes without issue

        // int t = dq_size;
        int recv;
        // while(int i = 0)
        // while (t > 0){
        //     int buffer_sent = t;
        //     MPI_Send(&buffer_sent, 1, MPI_INT, SEND, 0, MPI_COMM_WORLD);
        //     t-=1;
        // }
        int t = 0;
        int buffer[12] = {1,2,3,4,5,6,7,8,9,10,11,12};
        MPI_Scatter(&buffer, 2, MPI_INT, &my_value, 1,MPI_INT,0,MPI_COMM_WORLD);
        // MPI_
        // while (t > 0) {
            

            // MPI_Request requests[7];
            // MPI_Status statuses[7];
            // for (int i=1;i<8;i++){
                // int buffer_sent = t;
                // MPI_Isend( &buffer_sent, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &req);
                // std::this_thread::sleep_for(std::chrono::milliseconds(2000));

                // MPI_Ssend(&buffer_sent, 1, MPI_INT, RECEIVER, 0, MPI_COMM_WORLD);

                
                // MPI_Send(&buffer[i-1], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                // t-=1;
                
            //     int MPI_Isend(const void* buffer,
            //   int count,
            //   MPI_Datatype datatype,
            //   int recipient,
            //   int tag,
            //   MPI_Comm communicator,
            //   MPI_Request* request);

            //     int MPI_Recv(void* buffer,
            //  int count,
            //  MPI_Datatype datatype,
            //  int sender,
            //  int tag,
            //  MPI_Comm communicator,
            //  MPI_Status* status);

                // MPI_Isend(&buffer[i], 1, MPI_INT, i, 6, MPI_COMM_WORLD,&requests[i-1]);
                
                // MPI_Recv()
                // t-=1;
                // std::cout << t<<std::endl;
            // }
            
            // if(MPI_Waitall(7,requests,statuses) == MPI_SUCCESS){
            //     t-=7;
            //     std::cout << "see me"<<std::endl;
            // }
        // }
        

        // MPI_Send();
        
        // MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);

        // Distribute file names among other ranks
        // for (int dest_rank = 1; dest_rank < 12; ++dest_rank) {
        //     int num_files_to_send = dq_size / (12 - 1);
        //     int start_index = (dest_rank - 1) * num_files_to_send;
        //     int end_index = dest_rank * num_files_to_send;
        // if (dest_rank == 12 - 1) {
        //     end_index = dq_size; // Ensure all files are distributed
        // }
        // for (int i = start_index; i < end_index; ++i) {
        //     const char* file_bmp = dq;
        //     MPI_Send(file_bmp, strlen(file_bmp)+1, MPI_CHAR, dest_rank, 0, MPI_COMM_WORLD);
        // }   
    }
    else {
        MPI_Barrier(MPI_COMM_WORLD);
        int comm_size;
        MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

        // int received;
        // int complete=1;
        // while (true){
        //     if (received <= 0){
        //         exit;
        //     }
        //     MPI_Recv(&received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // std::cout << "Other Thread @ Rank: "<<rank<<" recieved: "<<received<<" comm size: "<<comm_size<<"\n";
        //     MPI_Send(&complete, 1, MPI_INT,0,6,MPI_COMM_WORLD)
        
        // }
        MPI_Scatter(NULL, 2, MPI_INT, &my_value, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Get_processor_name(name,&name_length);

        // do_stuff(f1.c_str(),o1.c_str());

        std::cout << "Other Process on Node: " <<name<<" @ Rank: "<<rank<<" recieved: "<<my_value<<" comm size: "<<comm_size<<"\n";
        // MPI_Request request;
        // MPI_Irecv(&received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &req);
        // std::cout << "Other Thread @ Rank: "<<rank<<" recieved: "<<received<<" comm size: "<<comm_size<<"\n";

        // MPI_Test(&req, &received, &stat);

        // const char* file_bmp = dq.front();
        // std::cout << file_bmp << "test";
        // std::cout << "Do you see me? @ Rank: " << rank << "\n";
        // std::cout << "Do something Other Thread @ Rank: "<<rank<<"\n";
        // dq.pop_front();
        // std::cout << "\t" << dq.size();
        
        // int dq_size;
        // MPI_Bcast(&dq_size, 1, MPI_INT, 0, MPI_COMM_WORLD); // Need Broadcast on BOTH ENDS
        // std::cout << "Other Thread @ Rank: "<<rank<<" with DQ Size: "<<dq_size<<"\n";

        // while (dq_size > 0) {
        //     MPI_Bcast(&dq_size, 1, MPI_INT, 0, MPI_COMM_WORLD); // Need Broadcast on BOTH ENDS
        //     std::cout << "Other Thread @ Rank: "<<rank<<" with DQ Size: "<<dq_size<<"\n";
        // }

        // ofstream myfile;
        // myfile.open ("example.txt");
        // myfile << "Writing this to a file.\n";
        // myfile.close();

        // Receive file names from rank 0
        // for (int i = 0; i < dq_size; ++i) {
        //     char file_bmp[MAX_FILENAME_LENGTH]; // Define MAX_FILENAME_LENGTH accordingly
        //     MPI_Recv(file_bmp, MAX_FILENAME_LENGTH, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //     // Process file_bmp here
        // }

        // for (int i = 0; i < dq_size; ++i) {
        //     char file_bmp[MAX_FILENAME_LENGTH]; // Define MAX_FILENAME_LENGTH accordingly
        //     MPI_Recv(file_bmp, MAX_FILENAME_LENGTH, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //     // Process file_bmp here
        // }
        // std::cout << "Other thread @ rank "<<rank<<" do something.";
        // MPI_Wait(&request, MPI_STATUS_IGNORE);

        // std::string appended = file_bmp;
        // appended += ".bmp";
        // const char* output_file = appended.c_str();
        
        // std::cout << output_file;
        // std::cout<< " Other Thread @ Rank " << rank << "\n";


        // do_stuff(file_bmp,output_file);
        // MPI_Barrier(MPI_COMM_WORLD);

        
    }
    
    MPI_Finalize();    
}
