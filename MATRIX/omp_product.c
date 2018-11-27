#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <omp.h>

#define RAND(min, max)	min + (double)(rand() / (RAND_MAX / (max - min)))
#define M		5
#define N		3
#define P		4

#ifdef __i386
extern __inline__ uint64_t rdtsc(void) {
  uint64_t x;
  __asm__ volatile ("rdtsc" : "=A" (x));
  return x;
}
#elif defined __amd64
extern __inline__ uint64_t rdtsc(void) {
  uint64_t a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}
#endif

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
	double *a, *b, *res, tmp;

	srand(time(NULL));
	init(&a, &b, &res);
	print_mat(a, N, M, "A");
	print_mat(b, P, N, "B");
	uint64_t time1 = rdtsc();
    #pragma omp parallel
    {
        #pragma omp for private(j, k)
        for(int i = 0; i < P; i++) {
            for(int k = 0; k < N; k++) {
                tmp = b[i * N + k];
                #pragma omp for reduction(+:res[i * M + j])
                for(int j = 0; j < M; j++)
                    res[i * M + j] += a[k * M + j] * tmp;
            }
        }
    }
	// for(int i = 0; i < P; i++) {
	// 	for(int j = 0; j < M; j++) {
	// 		for(int k = 0; k < N; k++)
	// 			res[i * M + j] += a[k * M + j] * b[i * N + k];
	// 	}
	// }
	printf("\nExec Time : %f\n", (double)(rdtsc() - time1) / 2500000000);
	print_mat(res, P, M, "RES");
	return (0);
}