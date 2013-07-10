--------------------------------------------------------------------------------
--
-- sm_dirprod.as: A category for finite cartesian (or direct) product
--                of a type.
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO, and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- dom DirectProduct

#include "algebra"

+++ `DirectProduct(n, T)' provides n-ary cartesian products
+++ of elements from `T'. Such a product is represented by
+++ a primitive array of elements from `T' with size `n'. 
+++ Hence the indices of its components are in the range `0..n-1'.
+++ Author: Marc Moreno Maza
+++ Date Created: 09/07/01
+++ Date Last Update: 10/07/01

DirectProduct(dim: MachineInteger, T: ExpressionType): 
 DirectProductCategory(dim, T) with { 
} == add {
	Rep == PrimitiveArray(T);
	import from T, Rep;
        assert(dim >= 1);
        local a: MachineInteger == 0;
        local b: MachineInteger == prev(dim);
        firstIndex: MachineInteger == a;
        lastIndex: MachineInteger == b;
        empty?(x: %): Boolean == false;
        (x: %) = (y: %) : Boolean == {
            xx := rep x;
            yy := rep y;
            for i in a..b repeat {
                xx.i ~= yy.i => return false;
           }
           true;
	}
        generator(x: %): Generator(T) == generate {
            xx := rep x;
            for i in a..b repeat yield xx.i;
        }
        copy(x: %): % == {
           xx := rep x;
           yy: Rep := new(dim);
           for i in a..b repeat set!(yy, i, xx.i);
           per(yy);
        }
        copy!(y: %, x:%): % == {
            xx := rep x;
            yy := rep y;
            for i in a..b repeat set!(yy, i, xx.i);
            y;
        }
	[g: Generator T]: % == {
		xx: Rep := new(dim);
                i: MachineInteger := a;
                for t in g repeat {
                    set!(xx, i, t);
                    i = b => break;
                    i := next(i);
                }
                assert(i = b);
		per xx;
	}
        bracket(t: Tuple(T)): %  == {
                assert(length(t) = dim);
                xx: Rep := new(dim);
                for i in a..b repeat {
                   set!(xx, i, element(t,next(i)));
                }
                per xx;
        }
        apply(x: %, i: MachineInteger): T == {
           assert(a <= i);
           assert(i <= b);
           rep(x).i;
        }
        free!(x: %): () == free!(rep x);
        set!(x: %, i: MachineInteger, t: T): T == set!(rep x, i, t);
        equal?(x: %, y: %, i: MachineInteger): Boolean  == {
            equal?(rep x, rep y, i)$Rep;
        }
        map(f:(T,T) -> T)(x: %, y: %): % == {
           xx := rep x;
           yy := rep y;
           zz: Rep := new(dim);
           for i in a..b repeat set!(zz, i, f(xx.i, yy.i));
           per zz;
        }
        map(f: T -> T)(x: %): %  == {
           xx := rep x;
           yy: Rep := new(dim);
           for i in a..b repeat set!(yy, i, f (xx.i));
           per(yy);
        }
        map!(f: T -> T)(x: %): %  == {
           xx := rep x;
           for i in a..b repeat set!(xx, i, f (xx.i));
           x;
        }
}

#if ALDOC
\thistype{DirectProduct}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{import from \this~(n,T)}
\Params{
{\em n} & \altype{MachineInteger} & The dimension of the direct product \\
{\em T} & \altype{ExpressionType} & The type of the factors \\
}
\Descr{\this~(n,T) provides $n$-ary direct products
 of elements from $T.$ Such a product is represented by
 a primitive array of elements from $T$ with size $n.$ 
 The indices of its components are in the range $0 \cdots n-1.$}
\begin{exports}
\category{\astype{DirectProductCategory}~(n,T)} \\
\end{exports}
#endif
