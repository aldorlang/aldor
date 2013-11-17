#include "algebra"
#include "aldorinterp"

SeriesSample(R:CommutativeRing, Rx:UnivariateTaylorSeriesType R): with {
         hypergeometricSeries: (R, R) -> Rx;
} == add {
         hypergeometricSeries(a0:R, c:R):Rx == {
                import from Sequence R;
                zero? a0 => 0;
                -- the following creates the stream [a0, c a0, c^2 a0, ... ]
                coeffs:Stream R := orbit(a0, (x:R):R +-> c * x);
                series sequence coeffs;
         }
}


test(): () == {
  import from SeriesSample(Integer, DenseUnivariateTaylorSeries Integer);
  import from DenseUnivariateTaylorSeries Integer;
  import from Integer;
  h := hypergeometricSeries(5, 2);
}

test();
