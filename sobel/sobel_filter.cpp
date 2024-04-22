#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image.h"
#include "stb_image_write.h"
#include <cmath>
#include <string>

void check_file(char const *filename){
    // Using & allows for pass by reference and gurantees not a NULL value
    // char const *filename is a char [] pointer which is preferrable over a std::string
    // https://stackoverflow.com/questions/1287306/difference-between-string-and-char-types-in-c
    int x, y, n;
    int result = stbi_info(filename, &x, &y, &n);
    std::cout << result << "\n";
}

int main(){
    int x,y,n;
    // check_file("video/snail_frames/out5.bmp");
    // last parameter forces number of desired channels. We only want grey so we set it to 1
    unsigned char* data = stbi_load("../video/snail_frames/out5.bmp", &y, &x, &n, 1);

    if(data == NULL){
        printf("Error in loading image");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", y, x, n);

    unsigned char nestedArray[x][y];

    // WHY ++row rather than row++?
    
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

    // nestedArray[-1][-1] return 00 is UNDEFINED BEHAVIOR AND BAD C++
    // TODO: Handle edge
    int gradientX,gradientY;
    int G_magnitude = 0;
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
            G_magnitude = sqrt(gradientX * gradientX + gradientY*gradientY);
            data[row*y+col] = std::min(std::max(G_magnitude,0),255);
        }
    }
    // You cannot directly convert unsigned char* to const void*
    // const_cast is used to remove const qualifier
    // const void* data2 = static_cast<const void*>(data);
    int out;
    out = stbi_write_bmp("out5_changed.bmp", y, x, 1, data);
    // std::cout << x;
    // std::cout << y;

    stbi_image_free(data);
}