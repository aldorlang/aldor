#include "algebra"

--  univpoly/categories/sit_umonom.as
extend SparseUnivariatePolynomial0(R:Join(ArithmeticType, ExpressionType),
        var:Symbol == new()):with {
  foo : String -> String
} == add {
  foo(s:String):String == s
}
