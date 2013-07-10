/*
 * sieve.c: A prime number sieve to count primes <= n.
 */
# include "stdio.h"

int
sieve(int n)
{
        int	i, p, np, *isprime;

	isprime = (int *) malloc(n * sizeof(int));
	for (p = 0; p < n; p++) isprime[p] = 1;

	for (np = 0, p = 2; p <= n; p++) {
		if (!isprime[p-1]) continue;
		np++;
		for (i = 2*p; i <= n; i += p) isprime[i-1] = 0;
	}
        return np;
}

int
main()
{
	int	n;
	
	for (n = 10; n <= 1000000; n *= 10)
		printf("There are %d primes <= %d\n", sieve(n), n);
}
