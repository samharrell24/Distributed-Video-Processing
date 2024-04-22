mpitest:
	mpicxx sobel/sobel_filter_openmp.cpp -fopenmp -o sobelmpi
	mpirun -np $(np) --hostfile $(hostfile) ./sobelmpi