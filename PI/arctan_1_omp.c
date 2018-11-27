#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>

extern __inline__ uint64_t rdtsc(void) {
  uint64_t a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}

int		main(int ac, const char **av)
{
    double	prec = atof(av[1]), res = 0, it = 1 / prec, x, tmp = 1;
	uint64_t time1 = rdtsc();
	omp_set_num_threads(2);
	#pragma omp parallel
	{
		printf("TOTAL THREADS = %d\n", omp_get_num_threads());
		#pragma omp for reduction(+:res)
		for (int i = 0; i < it; i++) {
			//printf("NB THREADS =%d\n", omp_get_thread_num());
			x = i*prec;
			res += (tmp + 1 / (1 + (x+prec) * (x+prec))) / 2 * prec;
			tmp = 1 / (1 + x * x);
		}
	}
	printf("Exec Time : %f\n", (double)(rdtsc() - time1) / 2500000000);
	printf("Resutat = %f\n", res);
	printf("Pi = %f\n", res * 4);
    return (0);
}
