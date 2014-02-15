---------------------------- sit_zcrtla.as ---------------------------------
-- Copyright (c) Helene Prieto 2000
-- Copyright (c) INRIA 2000, Version 0.1.12
-- Logiciel Sum^it (c)INRIA 2000, dans sa version 0.1.12
-----------------------------------------------------------------------------

#include "algebra"

macro {
       I==MachineInteger;
       A==PrimitiveArray;
}

IntegerCRTLinearAlgebra(Z:IntegerCategory, M:MatrixCategory Z): with {
      determinant: M -> Partial Z;
      determinant: (M,Z) -> Partial Z;
      determinant: (M,Z,Z) -> Partial Z;
      kernel: (M, I) -> M;
      hadamard: M -> Z;
--      modp: (GMPInteger,I) -> I;
} == add {
      determinant(m:M, fd:Z): Partial Z == determinant(m,fd,2*hadamard m);

      determinant(m:M): Partial Z == {
		import from Z;
		determinant(m,1,2*hadamard m);
	}

	-- fd = garanteed factor of det(m);
	-- B is such that |det(m)| <= B
	determinant(m:M, fd:Z, B:Z): Partial Z == {
		import from Boolean;
		assert(~zero? fd);
		deter(m, fd, next(B quo abs(fd)));
	}

	-- fd = garanteed factor of det(m);
	-- B is such that |det(m)/fd| <= B
      local deter(m:M, fd:Z, B:Z): Partial Z == {
	import from Z, I, A I, A A I;
	import from  ModulopGaussElimination;
	import from Boolean, LazyHalfWordSizePrimes;

	assert(~zero? fd); assert(B > 0);
         N:Z := 1;
         det:Z :=0;
         firststep:=true;
         n := numberOfRows m;
         assert(n=numberOfColumns m);

         mp: A A I := new n;
         i:I := 0;
         while i<n repeat { mp.i := new n; i:= next i;}

         for p in allPrimes() while N<B repeat {
           	i:I :=1; while i<=n repeat {
	      	  j:I := 1; while j<=n repeat {
               	  	mp.(i-1).(j-1) := m(i,j) mod p;
                  	j:= next j;
		  }
              	i:= next i;
	   	}
		fdmod := fd mod p;
		if ~zero?(fd) then {
           		d := mod_/(determinant!(mp,n,p), fdmod, p);
	   		if d > p quo 2 then d:=d-p;
    
           		if firststep then det:=d::Z;
           		else det := combine(N,p)(det,d);
		}
		firststep:=false;
		N:=p::Z * N;
	}
        N>=B => [det];
        failed;
      }

--	modp(a:GMPInteger,p:I):I =={
--	   macro Zg == GMPInteger;
--	   import from Zg,I, Generator I;
--	   ba:Zg := 2^(bytes*8);
--
--	   --if p > ba then
--	   --k:I :=1;
--	   k:I := machine(ba quo (p::Zg-1)^(2@I));
--
--           i:I :=1;
--	   b:I :=0;
--	   ap:I :=0;
--	   (n,r):=divide(length(a),(bytes*8));
--	   if r ~= 0 then n:= next n;
--	   j:I :=1;
--	   mp:= max mod p;
--	   mul:= 2*mp+ 2::I mod p;
--	   for g in limbs(a) repeat {
--		if g < 0 then 
--			b:= b + (g mod p + mul) * (ba^(j-1) mod p);
--		else b:= b+(g mod p)  * (ba^(j-1) mod p) ;
--
--		if j=n then ap:=ap + (b mod p);
--		else {
--		  if i=k then {
--		    ap:= ap + (b mod p);
--		    b:=0;
--		    i:=1;
--		  }
--		  else i:= next i;
--		  j:= next j;
--		}
--	   }
--
--	   ap mod p;
--	}

	-- r = rank mat
      kernel(mat:M, r:I): M == {
	import from Z, I, A I, A A I;
	import from  ModulopGaussElimination;
	import from Boolean, LazyHalfWordSizePrimes;
 
         --B:= 2*hadamard(mat);
         N:Z := 1;
         nr := numberOfRows mat;
	 nc := numberOfColumns mat;
         firststep:=true;

         mp: A A I := new nr;
	 mb: A A I := new nc;
	 ker: M := zero(nc,nc-r);

         i:I := 0;
         while i<nr repeat { 
		mp.i := new nc; 
		i:= next i;
	 }
         i:I := 0;
         while i<nc repeat { 
		mb.i := new(nc-r);
		i := next i;
	 }
         for p in allPrimes() repeat {
           i:I :=1;
           while i<=nr repeat {
	      j:I := 1;
	      while j<=nc repeat {
               	mp.(i-1).(j-1) := mat(i,j) mod p;
                j:= next j;}
              i:= next i;
	   }
	   dim := kernel!(mp,nr,nc,p,mb);

	   assert(dim <= r);
	   if firststep then {
	      i:=0;
              while i<nc repeat {
                  j:=0;
  		  while j<dim repeat {
			ker(i+1,j+1):= (mb.i.j)::Z;
			j := next j;
		  }
		  i := next i;
	      }
	      firststep := false;
	   }
	   else {
	      i:=0;
              while i<nc repeat {
                  j:=0;
  		  while j<dim repeat {
			ker(i+1,j+1):=combine(N,p)(ker(i+1,j+1),mb.i.j);
			j := next j;
		  }
	          i := next i;
	      }
	  }
	  N:=N*p::Z;
	}

        ker(1,1,nc,dim);
     }

	   

      hadamard(m:M): Z == {
         import from I;
         n:=numberOfRows m;

         assert(square? m);
         
         rn:Z := rNorm(m,1,n);
         cn:Z := cNorm(m,1,n);
         i:I := 2;
         while i<=n repeat {
            rn:=rn*rNorm(m,i,n);
            cn:=cn*cNorm(m,i,n);
            i:=next i;
         }

         (exact?, hb):=nthRoot(min(rn,cn),2);
         exact? => hb;
         next hb;
      }

      local rNorm(m:M,i:I,n:I):Z == {
         import from I;
         Nm:Z := m(i,1)^2;
         j:I := 2;
         while j<=n repeat {
            Nm:=Nm + m(i,j)^2;
            j:=j+1;}
         Nm;
      }

      local cNorm(m:M,i:I,n:I):Z == {
         import from I;
         Nm:Z := m(1,i)^2;
         j:I := 2;
         while j<=n repeat {
          Nm:=Nm + m(j,i)^2;
          j:=j+1;}
         Nm;
      }

}

