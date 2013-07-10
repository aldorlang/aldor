
#include <stdio.h>
#include <malloc.h>

typedef struct _ComplexDouble
{
   double real;
   double imag;
} *ComplexDouble;


ComplexDouble
complexAdd(ComplexDouble a, ComplexDouble b)
{
   ComplexDouble result;

   result = (ComplexDouble)malloc(sizeof(struct _ComplexDouble));
   result->real = a->real + b->real;
   result->imag = a->imag + b->imag;

   return result;
}

