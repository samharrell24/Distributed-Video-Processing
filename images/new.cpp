#include <iostream>
#include <vector>

using namespace std;

// Function to apply the Sobel filter in the x-direction
vector<vector<unsigned char>> sobelXFilter(const vector<vector<unsigned char>>& image) {
    int rows = image.size();
    int cols = image[0].size();

    vector<vector<unsigned char>> result(rows, vector<unsigned char>(cols, 0));

    // Sobel filter kernel for x-direction
    int kernel[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };

    // Apply the Sobel filter
    for (int i = 1; i < rows - 1; ++i) {
        for (int j = 1; j < cols - 1; ++j) {
            int gradientX = 0;
            // Convolution
            for (int k = -1; k <= 1; ++k) {
                for (int l = -1; l <= 1; ++l) {
                    gradientX += image[i + k][j + l] * kernel[k + 1][l + 1];
                }
            }
            // Ensure the result is within the valid range [0, 255]
            result[i][j] = min(max(gradientX, 0), 255);
        }
    }

    return result;
}

int main() {
    // Example input image
    vector<vector<unsigned char>> image = {
        {10, 20, 30, 40, 50},
        {60, 70, 80, 90, 100},
        {110, 120, 130, 140, 150},
        {160, 170, 180, 190, 200},
        {210, 220, 230, 240, 250}
    };

    // Apply Sobel filter in the x-direction
    vector<vector<unsigned char>> result = sobelXFilter(image);

    // Print the result
    for (const auto& row : result) {
        for (unsigned char pixel : row) {
            cout << static_cast<int>(pixel) << " ";
        }
        cout << endl;
    }

    return 0;
}
