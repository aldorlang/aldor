-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen f

#include "axllib"
#pile

DynamicSetCategory: Category == BasicType with

  roughEqual?: (%,%) -> Boolean
  reduce: % -> %

DynamicRingCategory: Category == Join(DynamicSetCategory, Ring) with

  roughExquo: (%,%) -> %
  roughZero?: % -> Boolean
  power: (%,Integer) -> %
  * : (Integer,%) -> %
  * : (SingleInteger,%) -> %

  one? : % -> Boolean
  recip : % -> Union(element:%,failed:Enumeration(failed))

  default x,x1,x2: %
  default n: Integer
  default ns: SingleInteger

  default
    n * x :% == (n::%) * x
    ns * x :% == (ns::%) * x

    roughEqual?(x1,x2):Boolean == roughZero?(x1-x2)

    local times(x: %, y: %): % == x * y
    power(x: %, n: Integer): % ==
      import from BinaryPowering(%, times, Integer)
      power(1, x, n)

    (x ^ n):% == power(x,n)

DynamicFieldCategory: Category == Join(DynamicRingCategory, Field) with

  exquo: (%,%) -> Union(element:%,failed:Enumeration(failed))

  default

    default x,x1,x2: %
    default uef: Union(element:%,failed:Enumeration(failed))
    default n: Integer

    (x ^ n):% ==
      n = 0 => 1
      n > 0 => reduce(power(x,n))
      reduce(power(inv(x),-n))

    (x1 / x2):% == reduce(x1 * inv(x2))
    (x1 \ x2):% == reduce(inv(x1) * x2)

    exquo(x1,x2):Union(element:%,failed:Enumeration(failed)) ==
      zero? x2 => [failed]
      zero?(x1 rem x2) => [reduce(x1 quo x2)]
      [failed]

    inv(x):% ==
      (uef:= recip(x)) case failed =>
        error "Division by 0 in inv from DynamicFieldCategory"
      uef.element
