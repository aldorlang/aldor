
#include "axllib"

RawComplex ==> RawRecord(re:DoubleFloat, im:DoubleFloat);

(t:TextWriter) << (r:RawComplex):TextWriter ==
{
   print << "complex(" << r.re << ", " << r.im << ")";
}


main():() ==
{
   local a, b, c:RawComplex;
   import
   {
      complexAdd: (RawComplex, RawComplex) -> RawComplex;
   } from Foreign C;

   a := [4.25, -8.5];
   b := [1.75,  8.5];
   c := complexAdd(a, b);

   print << a << " + " << b << " = " << c << newline;
}


main();

