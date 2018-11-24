#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct
{
	pthread_t	thread;
	int		id;
	double	prec;
	int	nbprocs;
}
calc_t;

void			*calc_pi(void *data)
{
	calc_t		*tab = (calc_t*)data;
	double		prec = tab->prec;
	int			nbprocs = tab->nbprocs;
	double		res = 0;
	int			proc_id = tab->id;

    for (int i = (int)(proc_id * (1/prec/nbprocs)); i < (int)(1/prec/nbprocs * (proc_id + 1)); i++)
        res += (1 / (1 + i*prec * i*prec) + 1 / (1 + (i*prec + prec) * (i*prec + prec))) / 2 * prec;
	double *pd;
	pd = malloc(sizeof(double));
	*pd = res;
	return ((void*)pd);
}

int             main(int argc, char *argv[])
{
	int         ret, i;
	double		sum = 0;
	calc_t		strct[atoi(argv[2])];

	for (i = 0; i < atoi(argv[2]); i++) {
		strct[i].id = i;
		strct[i].prec = atof(argv[1]);
		strct[i].nbprocs = atoi(argv[2]);
		ret = pthread_create(&(strct[i].thread), NULL, calc_pi, (void *)(&strct[i]));
		if (ret)
			fprintf(stderr,"%s", strerror(ret));
	}
	double	*thret;
	for (i = 0; i < atoi(argv[2]); i++) {
    	pthread_join(strct[i].thread, (void*)&thret);
		sum += *thret;
	}
	printf("Resutat = %f\n", sum);
	printf("Pi = %f\n", sum * 4);
	return (0);
}
