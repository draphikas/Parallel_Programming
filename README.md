  
#Parallel Programming
	Use of MPI and OpenMP for some mathematical problems

	COMMANDS
		ex: PRECISION					= 0.000001
		ex: NB_PROCESSUS / NB_THREADS	= 4

  #PI Calculation

	arctan_1:
		COMPIL	=>	gcc -o arctan_1 arctan_1.c
		RUN		=>	./arctan_1 PRECISION

	OpenMP:
		COMPIL	=>	gcc -o omp_arctan_1 arctan_1_omp.c
		RUN		=>	./omp_arctan_1 PRECISION

	MPI (need mpich package installed):
		COMPIL	=>	mpicc -o mpi_arctan_1 arctan_1_mpi.c
		RUN		=>	mpirun -n NB_PROCESSUS ./mpi_arctan_1 PRECISION

	pthread:
		COMPIL	=>	gcc -o pthread_arctan_1 arctan_1_pthread.c -pthread
		RUN		=>	./pthread_arctan_1 PRECISION NB_THREADS


  #MATRIX Product

	product & openmp:
		COMPIL	=>	gcc -o [omp_]product [omp_]product.c
		RUN		=>	./[omp_]product

	MPI (need mpich package installed)
		COMPIL	=> mpicc -o mpi_product mpi_product.c
		RUN		=> mpirun -n NB_PROCESSUS ./mpi_processus