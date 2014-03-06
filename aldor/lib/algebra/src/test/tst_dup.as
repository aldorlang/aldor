#include "aldor"
#include "aldorio"
#include "algebra"
#pile

-- Very incomplete test, just sufficient to show that
-- factorisation works for at least one case.
TestDenseUnivariatePolynomial: TestCategory with == add
 test(): () == 
  import from AldorInteger
  import from DenseUnivariatePolynomial(AldorInteger)
  import from Product DenseUnivariatePolynomial(AldorInteger)
  import from Assert DenseUnivariatePolynomial AldorInteger
  import from Assert AldorInteger

  test()$TestRing(DenseUnivariatePolynomial AldorInteger);
  
  u:DenseUnivariatePolynomial(AldorInteger) := monomial(1)
  p:DenseUnivariatePolynomial(AldorInteger) := u^2 + 2*u + 1

  assertEquals(2, degree(p))
  assertEquals(6, degree(p^3))
  qq := AldorInteger has FactorizationRing
  assertTrue(qq)
  (n, x) := factor(p)
  assertEquals(1, n)
  assertEquals(p, expand x)
  assertEquals(1, (# x)::AldorInteger)

import from TestDenseUnivariatePolynomial
test()

