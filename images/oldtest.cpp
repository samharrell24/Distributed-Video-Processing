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
    unsigned char* data = stbi_load("box_320x240.bmp", &y, &x, &n, 1);

    if(data == NULL){
        printf("Error in loading image");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", x, y, n);

    unsigned char nestedArray[x][y];
    unsigned char nestedArrayCopy[x][y];

    
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
            nestedArray[row][col] = data[row*y+col];
            nestedArrayCopy[row][col] = data[row*y+col];
            printf("(%d,%d): %d/%d, ", row, col, nestedArrayCopy[row][col], data[row*y+col]);
            if (col == 239) {
                std::cout << "\n";
            }
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
    int gradientX,gradientY,temp_j,temp_k;
    int G_magnitude = 0;
    for(int row = 1; row<x-1; ++row){
        for(int col = 1; col<y-1; ++col){
                G_magnitude = 0;
                gradientX = 0;
                gradientY= 0;
            for(int j=-1;j<=1;++j){             
                for(int k=-1;k<=1;++k){
                    gradientX += nestedArray[row+j][col+k] * kernelX[j+1][k+1];
                    gradientY += nestedArray[row+j][col+k] * kernelY[j+1][k+1];
                }
            }
            // for (int a = 0; a < 3; a++) {
            //     for (int b = 0; b < 3; b++) {
            //         gradientX += data[(row - 1 + a) * y + col - 1 + b] * kernelX[a][b];   
            //     }
            // }

            G_magnitude = sqrt(gradientX * gradientX + gradientY*gradientY);
            G_magnitude = std::min(std::max(G_magnitude, 0), 255);
            
            // std::cout << G_magnitude<<"\n";
            // std::cout << gradientX+gradientY<<"\n";
            // nestedArrayCopy[row][col] = std::min(std::max(G_magnitude,0),255);
            data[row*y+col] = std::min(std::max(G_magnitude,0),255);
            // nestedArray[row][col]=std::min(std::max(G_magnitude,0),255);
        }
    }

    std::cout << "\n\n";
    // any values over 255 are set to 255

    for(int row = 0; row<x; ++row){
        for(int col = 0; col<y; ++col){
            printf("(%d,%d): %d, ", row, col, data[row*y+col]);
            if (col == 239) {
                std::cout << "\n";
            }
        }
    }

    // for (int row = 0; row < x; ++row) {
    //     for (int col = 0; col < y; ++col) {
    //         // if (col%2==0){
    //         //     continue;
    //         // }
    //         data[row*y+col] = nestedArrayCopy[row][col];
    //     }
    // }

    // You cannot directly convert unsigned char* to const void*
    // const_cast is used to remove const qualifier
    const void* data2 = static_cast<const void*>(data);
    int out;
    out = stbi_write_bmp("box_changes.bmp", 320, 240, 1, data);
    // std::cout << x;
    // std::cout << y;

    stbi_image_free(data);
}