#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

#define RAND(min, max)	min + (rand() / (RAND_MAX / (max - min)))
#define M		5
#define N		3
#define P		4

void	init(double **a, double **b, double **res)
{
	*a = (double*)malloc(sizeof(double) * M * N);
	*b = (double*)malloc(sizeof(double) * N * P);
	*res = (double*)calloc(M * P, sizeof(double));
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < N; i++)
            for (int j = 0; j < M; j++)
                (*a)[i * M + j] = RAND(0, 3);
        #pragma omp for
        for (int i = 0; i < P; i++)
            for (int j = 0; j < N; j++)
                (*b)[i * N + j] = RAND(0, 3);
    }
}

// Ceci est un commentaire
// Ceci est un autre commentaire

void	print_mat(double *matrix, int mat_i, int mat_j, char *name)
{
	printf("===== MATRIX %s =====\n", name);
	for (int i = 0; i < mat_i; i++) {
		for (int j = 0; j < mat_j; j++)
			printf("%.2f\t", matrix[i * mat_j + j]);
		printf("\n");
	}
	printf("\n");
}

int		main(int ac, char **av)
{
	MPI_Init(&ac, &av);

	int proc_id, nbprocs;
	MPI_Comm_size(MPI_COMM_WORLD, &nbprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

	char	processor_name[MPI_MAX_PROCESSOR_NAME];
	int		name_len;
	MPI_Get_processor_name(processor_name, &name_len);
	printf("Processor %s, rank %d out of %d processors\n",
		processor_name, proc_id, nbprocs);

	double *a, *b, *res, tmp;
	init(&a, &b, &res);
	if (proc_id == 0) {
		print_mat(a, N, M, "A");
		print_mat(b, P, N, "B");
	}
	double time1 = MPI_Wtime();
    #pragma omp parallel
    {
        #pragma omp for private(j, k)
        for(int i = proc_id * P / nbprocs; i < (proc_id + 1) * P / nbprocs; i++) {
            for(int k = 0; k < N; k++) {
                tmp = b[i * N + k];
                #pragma omp for reduction(+:res[i * M + j])
                for(int j = 0; j < M; j++)
                    res[i * M + j] += a[k * M + j] * tmp;
            }
        }
    }
	if (proc_id != 0)
		MPI_Send(&res[proc_id * M * P / nbprocs], M * P / nbprocs, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	if (proc_id == 0) {
		int ierr;
		MPI_Status status;
		for (int i = 1; i < nbprocs; i++) {
			ierr = MPI_Recv(&res[i * M * P / nbprocs], M * P / nbprocs, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
			if (ierr != MPI_SUCCESS)
				MPI_Abort(MPI_COMM_WORLD, 1);
		}
		printf("\nExec Time : %f\n", MPI_Wtime() - time1);
		print_mat(res, P, M, "RES");
	}
	MPI_Finalize();
	return (0);
}