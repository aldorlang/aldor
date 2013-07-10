#define SI		int
#define F		double
#define Infinity	0

typedef struct {
	F real;
	F imag;
} CF;

#define maxIters	((SI) 100)

void
drawPoint(SI x, SI y, SI n)
{
	if (n == Infinity)
		printf("   ");
	else
		printf((n < 10 ? "  %d" : " %d"), n);
}

void
endRow()
{
    printf("\n");
}
	
SI
mandel(CF *pc)
{
	CF z = {0.0, 0.0};
	CF tmp;
	SI nc = 0;
	SI n;

	for (n = 1; n <= maxIters && z.real * z.real + z.imag * z.imag < 4.0;
	     n++) {
			/* z = z*z */
			tmp.real = z.real * z.real - z.imag * z.imag;
			tmp.imag = z.real * z.imag + z.imag * z.real;
			z = tmp;

			/* z = z + *pc */
			z.real = z.real + pc->real;
			z.imag = z.imag + pc->imag;

			nc = n;
		}
	if (nc == maxIters)  nc = Infinity;

	return nc;
}

SI
drawMand(F minR, F maxR, SI numR, F minI, F maxI, SI numI)
{
	F i, r;
	SI ic, rc;
	F del_i = (maxI - minI)/(numI - 1),
	  del_r = (maxR - minR)/(numR - 1);

	CF c;
	SI sum = 0;

	for (i = minI, ic = 1; ic <= numI ; ic++, i += del_i) {
	    	for (r = minR, rc = 1; rc <= numR ; rc++, r += del_r) {
			c.real = r;
			c.imag = i;
			sum = sum + mandel(&c);
			/* drawPoint(rc, ic, mandel(&c)); */
			
		}
		/* endRow(); */
	}
	
	printf("The sum is %d\n", sum);
	return numR * numI;
}


main()
{
	printf("%d\n",drawMand(-2.0, -1.0, 600, -0.5, 0.5, 600));
}
