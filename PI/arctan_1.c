#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern __inline__ uint64_t rdtsc(void) {
  uint64_t a, d;
  __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
  return (d<<32) | a;
}

// static inline double	calc_func(double x)
// {	
// 	return (1 / (1 + x * x));
// }

int		main(int ac, const char **av)
{
    double	prec = atof(av[1]), res = 0, it = 1 / prec, x, tmp = 1;
	uint64_t time1 = rdtsc();

	for (int i = 0; i < it; i++) {
		x = i*prec;
		res += (tmp + 1 / (1 + (x+prec) * (x+prec))) / 2 * prec;
		tmp = 1 / (1 + x * x);
	}
	printf("Exec Time : %f\n", (double)(rdtsc() - time1) / 2500000000);
	printf("Resutat = %f\n", res);
	printf("Pi = %f\n", res * 4);
    return (0);
}
