#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
#include "stb_image_write.h"
#include <cmath>

int main(){
    int x,y,n,x1,y1,n1,ok;

    // Check if file works

    // & allows for pass by reference and guarantees x1,y1,n1 are not NULL
    // since C++ does not allow NULL references
    // Also since you are passing by reference, the object is not copied
    ok = stbi_info("box_320x240.bmp", &x1, &y1, &n1);
    // TODO add OK check

    // std::cout << "Will file work?: (1 is True, 0 is False)" << ok;
    // std::cout << "\n";

    // last parameter forces number of desired channels. We only want grey so we set it to 1
    unsigned char* data = stbi_load("box_320x240.bmp", &x, &y, &n, 1);

    if(data == NULL){
        printf("Error in loading image");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", x, y, n);

    unsigned char nestedArary[x+2][y+2];
    
    std::cout << "rows: " << x << "\n" << "cols: " << y << "\n";

    // WHY ++row rather than row++?
    
    // Pre-increment (++row)
    // int row1 = 5;
    // int result1 = ++row1;  **Increment row first, then use the updated value

    // // Post-increment (row++)
    // int row2 = 5;
    // int result2 = row2++;  **Use row's current value first, then increment row

    for(int row = 0; row<x; ++row){
        for(int col = 0; col<y; ++col){
            nestedArary[row+1][col+1] = data[row*y+col];
        }
    }

    int kernelX[3][3] = {
        {1, 0, -1},
        {2, 0, -2},
        {1, 0, -1}
    };
    int kernelY[3][3] = {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };

    // nestedArray[-1][-1] return 00 is UNDEFINED BEHAVIOR AND BAD C++
    int gradientX,gradientY,temp_j,temp_k;
    int G_magnitude = 0;
    for(int row = 1; row<x; ++row){
        for(int col = 1; col<y; ++col){
            
            for(int j=-1;j<=1;++j){
                gradientX = 0;
                gradientY= 0;
                for(int k=-1;k<=1;++k){
                    // if(row==0){
                    //     temp_j=-1;
                    // }
                    // if(col==0){
                    //     temp_k=-1;
                    // }
                    // std::cout<<k;
                    // break;
                    // gradientX += nestedArary[row+j+temp_j][col+k+temp_k] * kernelX[j+1][k+1];
                    // gradientY += nestedArary[row+j+temp_j][col+k+temp_k] * kernelX[j+1][k+1];

                    gradientX += nestedArary[row+j][col+k] * kernelX[j+1][k+1];
                    gradientY += nestedArary[row+j][col+k] * kernelX[j+1][k+1];
                
                }
            }
            G_magnitude = sqrt((pow(gradientX,2)+pow(gradientY,2)));

            // std::cout << G_magnitude<<"\n";
            // nestedArary[row][col]=gradientX;
            // std::cout << gradientX+gradientY<<"\n";
            nestedArary[row][col]=std::min(std::max(gradientY,0),255);
        }
    }

    // any values over 255 are set to 255

    for (int row = 0; row < x; ++row) {
        for (int col = 0; col < y; ++col) {
            data[row*y+col] = nestedArary[row][col];
        }
    }

    // You cannot directly convert unsigned char* to const void*
    // const_cast is used to remove const qualifier
    const void* data2 = static_cast<const void*>(data);
    int out;
    out = stbi_write_bmp("box_changes.bmp", 320, 240, 1, data2);
    // std::cout << x;
    // std::cout << y;

    stbi_image_free(data);
}
