/*
 * Explore memory layout.
 */

#include <stdio.h>
#include <stdlib.h>

extern char **environ;
extern void *sbrk(), *malloc(); 
extern int  main();
extern void mapper();

int	zeros[1024];
int	ones [16] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

void
mapper(a0)
	int	*a0;
{
	int	l1;
	int	*a1;
	char	*s0, *s1;
	char	*m;

	a1 = &l1;
	s0 = (char*)sbrk(0);
	m  = malloc(100);
	s1 = (char*)sbrk(0);
	
	printf("%#lx = fn malloc\n",   (unsigned long) malloc);
	printf("\n");
	printf("%#lx = fn mapper\n",   (unsigned long) mapper);
	printf("%#lx = fn main\n",     (unsigned long) main);
	printf("\n");
	printf("%#lx = ones\n",        (unsigned long) ones);
	printf("\n");
	printf("%#lx = zeros\n",       (unsigned long) zeros);
	printf("\n");
	printf("%#lx = sbrk(0)\n",     (unsigned long) s0);
	printf("%#lx = malloc(100)\n", (unsigned long) m);
	printf("%#lx = sbrk(0)\n",     (unsigned long) s1);
	printf("\n");
	printf("%#lx = &l1\n",         (unsigned long) a1);
	printf("%#lx = &l0\n",         (unsigned long) a0);
	printf("%#lx = environ\n",     (unsigned long) environ);
}

int
main()
{
	int	l0;
	mapper(&l0);
	return 0;
}
