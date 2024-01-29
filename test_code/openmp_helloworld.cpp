#include <iostream>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        #pragma omp critical
        {
            std::cout << "Hello, World! (Thread ID: " << thread_id << ")\n";
        }
    }

    return 0;
}
