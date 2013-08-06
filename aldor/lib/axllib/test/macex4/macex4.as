-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase macex

-- This file tests macro name scoping within where expressions.

macro {
        x  == u;
        u  == v;
        x ** n == expt(x,n);
}

x ** 10;

x ** 10 where {
  macro u == z;
  macro expt(z,m) == m * z - 1;
}

x;
u;
x ** 10;

x ** 10 where {
  macro x == v;
  macro expt(z,m) == {
    m * z - 1 where {
      macro d * e == mult(d,e);
      macro d - e == subt(d,e);
    }
  }
}

x;
x ** 10;
