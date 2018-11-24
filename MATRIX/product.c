#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define RAND(min, max)	min + (rand() / (RAND_MAX / (max - min)))
#define M		1000
#define N		1000
#define P		1000

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
	for (int i = 0; i < N; i++)
		for (int j = 0; j < M; j++)
			(*a)[i * M + j] = RAND(0, 3);
	for (int i = 0; i < P; i++)
		for (int j = 0; j < N; j++)
			(*b)[i * N + j] = RAND(0, 3);
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
	double *a, *b, *res;

	srand(time(NULL));
	init(&a, &b, &res);
	//print_mat(a, N, M, "A");
	//print_mat(b, P, N, "B");
	double tmp;
	uint64_t time1 = rdtsc();
	for(int i = 0; i < P; i++){
		for(int k = 0; k < M; k++){
			tmp = b[i * N + k];
			for(int j = 0; j < N; j += 8) {
				res[i * M + j] += (a[k * M + j] * tmp);
				res[i * M + j + 1] += (a[k * M + j + 1] * tmp);
				res[i * M + j + 2] += (a[k * M + j + 2] * tmp);
				res[i * M + j + 3] += (a[k * M + j + 3] * tmp);
				res[i * M + j + 4] += (a[k * M + j + 4] * tmp);
				res[i * M + j + 5] += (a[k * M + j + 5] * tmp);
				res[i * M + j + 6] += (a[k * M + j + 6] * tmp);
				res[i * M + j + 7] += (a[k * M + j + 7] * tmp);
			}
		}
	}
	// 	for(int i = 0; i < P; i++){
	// 	for(int j = 0; j < M; j++){
	// 		res[i * M + j] = 0.0;
	// 		for(int k = 0; k < N; k++)
	// 			res[i * M + j] += (a[k * M + j] * b[i * N + k]);
	// 	}
	// }
	printf("\nExec Time : %f\n", (double)(rdtsc() - time1) / 2500000000);
	//print_mat(res, P, M, "RES");
	return (0);
}