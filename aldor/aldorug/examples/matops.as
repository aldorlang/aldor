#include "axiom.as"
#pile

MatrixSymmetry(R:Field): with

        symmetricPart : Matrix R -> Matrix R
                ++ `symmetricPart(M)' returns a symmetric
                ++ matrix `S', computed as `(M + transpose M)/2'.
                ++ The difference `M - S' is antisymmetric.

        antisymmetricPart : Matrix R -> Matrix R
                ++ `antisymmetricPart(M)' returns an antisymmetric
                ++ matrix `A', computed as `(M - transpose M)/2'.
                ++ The difference `M - A' is symmetric.
== add
        import from R, Integer

        symmetricPart(m: Matrix R): Matrix R ==
                mt := transpose m
                inv(2::R) * (m + mt)

        antisymmetricPart(m: Matrix R): Matrix R ==
                mt := transpose m
                inv(2::R) * (m - mt)
