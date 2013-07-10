#include "foam_c.h"

float 
x_(FiComplexSF *a, char *b, int *c, double *d, char *e, FiComplexDF *f, int g[10], float h[6], int chrlen0, int chrlen1) 
{
	int i;

	printf("C has:\n");
	printf ("a = %f %f\n",a->real,a->imag);
	a->real = 1000.0;
	a->imag = 2000.0;
	printf("b is %s %d\n",b,chrlen0);
	*b = 'X'; 
	printf("c is %d\n",*c);
	*c = 5000;
	printf("d is %f\n",*d);
	*d = (double) 89.8;
	printf("e is %s %d\n",e, chrlen1);
	*e = 'X'; 
	printf("f is %f %f\n",f->real,f->imag);
	f->real = (double) 67.5;
	f->imag = (double) 68.5;
	for (i=0; i<10; i++) {
		printf("Element %d of g is %d\n",i,g[i]);
		g[i] = 0;
	}
	for (i=0; i<6; i++) {
		printf("Element %d of h is %f\n",i,h[i]);
		h[i] = (float) i + 10.0;
	}
	printf("End.\n\n");
	return 3.5;
}

float 
y_(FiComplexSF *a, char *b, int *c, double *d, char *e, FiComplexDF *f, int g[10], float h[6], int chrlen0, int chrlen1) 
{
	int i;

	printf("C has:\n");
	printf ("a = %f %f\n",a->real,a->imag);
	printf("b is %s %d\n",b,chrlen0);
	printf("c is %d\n",*c);
	printf("d is %f\n",*d);
	printf("e is %s %d\n",e, chrlen1);
	printf("f is %f %f\n",f->real,f->imag);
	for (i=0; i<10; i++) 
		printf("Element %d of g is %d\n",i,g[i]);
	for (i=0; i<6; i++) 
		printf("Element %d of h is %f\n",i,h[i]);
	printf("End.\n\n");
	return 3.5;
}






