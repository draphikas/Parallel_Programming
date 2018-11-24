#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int		main(int ac, char **av)
{
    int ierr, proc_id, nbprocs;
    ierr = MPI_Init(&ac, &av);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &nbprocs);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    // char processor_name[MPI_MAX_PROCESSOR_NAME];
    // int name_len;
    // MPI_Get_processor_name(processor_name, &name_len);
    // printf("Hello world from processor %s, rank %d out of %d processors\n",
    //        processor_name, proc_id, nbprocs);

    double	prec = atof(av[1]), res = 0, x, tmp;
    double starttime = MPI_Wtime();
    int it = (int)(1 / prec / nbprocs);
    for (int i = proc_id * it; i < (proc_id + 1) * it; i++) {
        //res += (1 / (1 + i*prec * i*prec) + 1 / (1 + (i*prec + prec) * (i*prec + prec))) / 2 * prec;
        x = i*prec;
		res += (tmp + 1 / (1 + (x+prec) * (x+prec))) / 2 * prec;
		tmp = 1 / (1 + x * x);
    }
    if (proc_id != 0)
        MPI_Send(&res, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    if (proc_id == 0) {
        double sum = res, ret;
        MPI_Status  status;

        for (int i = 0; i != nbprocs - 1; ++i) {
            int ierr = MPI_Recv(&ret, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (ierr == MPI_SUCCESS)
                sum += ret;
            else MPI_Abort(MPI_COMM_WORLD, 1);
        }
        // double sum = 0;
        // MPI_Reduce(&res, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        printf("TIME = %f\n", MPI_Wtime() - starttime);
	    printf("Resutat = %f\n", sum);
	    printf("Pi = %f\n", sum * 4);
    }
    MPI_Finalize();
    return (0);
}