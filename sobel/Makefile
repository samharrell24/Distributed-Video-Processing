sobelmake:
	g++ sobel_filter_openmp.cpp -o test
	./test

mpitest:
	mpicxx sobel_filter_openmp.cpp -fopenmp -o sobelmpi
	mpirun -np 8 --hostfile my_hostfile ./sobelmpi
