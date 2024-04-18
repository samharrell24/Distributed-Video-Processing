mpitest:
	mpicxx sobel_filter_openmp.cpp -fopenmp -o sobelmpi
	mpirun -np 3 --hostfile my_hostfile ./sobelmpi