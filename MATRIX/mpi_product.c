#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <mpi.h>

#define RAND(x)	rand() % 5

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

void	init(int ***tab, int **x, int **y, int size)
{
	int *tmp;

	*tab = (int**)malloc(sizeof(int*) * size);
	*x = (int*)malloc(sizeof(int) * size);
	*y = (int*)malloc(sizeof(int) * size);
	tmp = (int*)malloc(sizeof(int) * size * size);
	for (int i = 0; i < size; i++)
		(*tab)[i] = &tmp[i * size];
	for (int j = 0; j < size; j++)
		for (int i = 0; i < size; i++)
			(*tab)[i][j] = RAND(5);
	for (int i = 0; i < size; i++) {
		(*x)[i] = RAND(5);
		(*y)[i] = 0;
	}
}

void	print_tab(int **tab, int size)
{
	for (int j = 0; j < size; j++) {
		for (int i = 0; i < size; i++)
			printf("%d\t", tab[i][j]);
		printf("\n");
	}
	printf("\n");
}

int		main(int ac, char **av)
{
	int ierr, proc_id, nbprocs;
    ierr = MPI_Init(&ac, &av);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &nbprocs);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, proc_id, nbprocs);
	int **tab, *x, *y;
	int size = atoi(av[1]);

	srand(time(NULL));
	init(&tab, &x, &y, size);
	if (proc_id == 0) {
		for (int i = 0; i < size; i++)
			printf("%d\t", x[i]);
		printf("\n\n");
		print_tab(tab, size);
	}
	double starttime = MPI_Wtime();
	int	*Apos = &tab[proc_id][0];
	int	*ypos = &y[0];
	for (int i = 0; i < size / nbprocs; i++) {
		int *xpos = &x[0];
		int ytmp = 0;
		for (int j = 0; j < size; j++)
			ytmp += (*Apos++) * (*xpos++);
		*ypos++ = ytmp;
	}
	if (proc_id != 0)
        MPI_Send(&y[0], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    if (proc_id == 0) {
        int ret;
        MPI_Status  status;
        for (int i = 1; i < nbprocs; ++i) {
            int ierr = MPI_Recv(&ret, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (ierr == MPI_SUCCESS)
                y[i] = ret;
            else MPI_Abort(MPI_COMM_WORLD, 1);
        }
		printf("Exec Time = %f\n", MPI_Wtime() - starttime);
		for (int i = 0; i < size; i++)
			printf("%d\t", y[i]);
		printf("\n");
	}
	MPI_Finalize();
	return (0);
}