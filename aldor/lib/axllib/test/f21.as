-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp  -l axldem -Mno-ALDOR_W_FunnyEscape
--> testrun   -l axldem -Mno-ALDOR_W_FunnyEscape -l axllib
#pile

#include "axllib.as"
#library DemoLib       "axldem"

import from DemoLib

macro I == Integer
macro SI == SingleInteger
macro F == DoubleFloat
macro ** == ^
macro VF == ZBVector F

-- Zero based vector
ZBVector(S: Ring): Join(BasicType, Aggregate S) with {
	new:   (SI, S)    -> %;
	apply: (%, SI)    -> S;
	set!:  (%, SI, S) -> S;
	empty: ()         -> %;
	+: (%, %)         -> %;
	-: (%, %)         -> %;
	-: %              -> %;
	*: (%, %)         -> %;
	*: (S, %)         -> %;
	dot: (%, %)       -> S;
	vector : Tuple S  -> %;
	vector : Generator S -> %;
} == Vector S add {
	Rep ==> Vector S;
	import from Rep;

	apply(s: %, n: SI): S == rep(s).(n+1);
	set!(v: %, n: SI, s: S): S == set!(rep(v),(n+1), s);
}	

MAXNHTERMS:I==10000
import from Segment(Integer)

default jlimit, jfin, ii, jflag:I
default a,b,c,x,y,cf,cf1,i,y1,yX,cfX:F

(sn:SI) * (a : F):F == (sn::F) * a
(n: I)  * (a : F):F == (n ::F) * a

import { log: F -> F; exp: F -> F } from DoubleFloatElementaryFunctions

(z:F) ** (a: F): F == exp(a*log z)

----First the usual _2F_1 hypergeometric series:                                
F21_Ser0(a:F,b:F,c:F,x:F):F==                                                             
  jlimit :=   1 ; jfin :=   0                                                   
  y    := 1.0 ; cf  := 1.0
  for ii:I in 0..MAXNHTERMS while jfin < jlimit  repeat
    i := coerce(ii)
    cf := x * cf * (a + i) * (b + i)/((i + 1) * (c + i)); y1 := y + cf         
    if ( y1 = y ) then                                                          
      jfin := jlimit                                                            
      --print [" took ",i+1," terms"]                                             
    y := y1                                                                     
  y                                                                             
                                                                                
----Then the _2F_1* hypergeometric series of the second lind in the             
----case when c = 1, but we still keep all paramters (a,b,c) in:                
----both _2F_1 and _2F_1* can be computed all at once:                          
F21X_Ser0(a:F,b:F,c:F,x:F):Record(aa:F,bb:F)==                                                            
  jlimit :I :=   1 ; jfin :=   0                                                   
  y      := 1.0 ; cf   := 1.0 ; yX := 0.0 ; cfX := 0.0                          
  for ii:I in 0..MAXNHTERMS while jfin < jlimit repeat
    i := coerce(ii)
    cf1:= x * cf * (a + i) * (b + i)/((i + 1) * (c + i))                       
    y1 := y + cf1                                                               
    top:F:= x * (cf * (2*i+a+b) + cfX * (a+i) * (b+i)) - 2 * (i+1) * cf1          
    cfX:F:= top/( (i + 1) * (c + i) )                                            
    cf := cf1                                                                   
    y1X:F:= yX + cfX                                                              
    --print [" c cX are ",cf,cfX]                                                 
    if ( y1 = y and y1X = yX ) then                                             
      jfin := jlimit                                                            
      --print [" took ",i+1," terms"]                                             
    y := y1 ; yX := y1X                                                         
  [y,yX]                                                                        
                                                                                
----Now we compute \DiGamma(a) - \DiGamma(1) using the _2F_1                    
----monodromy relation: log(1-z)+\DiGamma(a)-\DiGamma(1) =                      
---- z**a * ( _2F_1*(1,a;1,1-z) - _2F_1(1,a,1+a;z)/a )                          
                                                                                
----Here we are computing at once M functions \Phi(z,m,a) =                     
---- sum_{n=0}^\infinty z**n/(n+a)**m for m = 1,...,M                           
---- for z with abs(z) < 1 (or, better, abs(z) <= 0.5):                         
Riemann_Phis(a:F,M:SI,x:F):VF==                                                           
---make Y a Vector (of NFLOAT or of G NFLOAT):                                  
  Y:VF := new(M,0.0)
  xx:F := 1.0
  jflag := 0
  for (ii:I) in 0..MAXNHTERMS while ( jflag ~= 1 ) repeat
    i := coerce ii
    jflag :=  1                                                                 
    cc:F := 1.0/(a + i) ; cf := xx * cc
    for (m:SI) in 0..M-1 repeat                                                      
      y := apply(Y,m) + cf                                                             
      if ( y ~= apply(Y,m) ) then jflag := 0                                           
      set!(Y,m,y)                                                                  
      cf := cf * cc                                                             
    xx := x * xx                                                                
  --print [" took ",i+1," terms"]                                                 
  Y                                                                             
                                                                                
----Next we compute at once M functions: (d/da)^m {_2F_1*(1,a;a+1;z) }          
---- for m = 0,...,M-1                                                          
Auxiliary_Phis(a:F,M:SI,x:F):VF==                                                         
---make Y and C Vectors (of NFLOAT or of G NFLOAT):                             
  Y  : VF := new(M, 0.0)
  CX : VF := new(M, 0.0)
  C  : VF := new(M, 0.0) ; set!(C,0, 1.0)
  jflag := 0
  for ii:I in 0..MAXNHTERMS while ( jflag ~= 1 ) repeat
    i := coerce ii
    jflag :=  1                                                                 
----make 1.0/(1.0+i) into a precomputed constant:                               
    ci : F := 1/(1 + i)                                                           
    ct : F := x * apply(C,0) * (a + i) * ci                                                
----ct := x * C.0 * (a + i)/(1. + i)                                            
----cxt:=( x * (apply(CX,0) * (a + i) + C.0) - ct )/(1. + i)                           
    cxt : F :=( x * (apply(CX,0) * (a + i) + apply(C,0)) - ct ) * ci                               
    y := apply(Y,0) + cxt                                                              
    if ( y ~= apply(Y,0) ) then jflag := 0                                             
    set!(Y,0, y)
    for m:SI in 1..M-1 repeat                                                      
------cn      := x * ( apply(C,m) * (a + i) + m * apply(C,m-1) )/(1. + i)                   
      cn : F  := x * ( apply(C,m) * (a + i) + m * apply(C,m-1) ) * ci                       
      set!(C,(m-1), ct)        ; ct := cn                                           
------cxn     :=(x * (CX.m * (a + i) + m * CX.(m-1) + apply(C,m)) - ct)/(1. + i)       
      cxn : F :=(x * (apply(CX,m) * (a + i) + m * apply(CX,(m-1)) + apply(C,m)) - ct) * ci           
      set!(CX,(m-1),cxt)       ; cxt:= cxn                                          
      y := apply(Y,m) + cxt                                                            
      if ( y ~= apply(Y,m) ) then jflag := 0                                           
      set!(Y,m, y)                                                                  
    set!(C,(M-1) ,ct)        ;   set!(CX,(M-1), cxt)
    --import from String
    --print ("C = " << C << " CX = " << CX <<newline
  --print [" took ",i+1," terms"]                                                 
  Y                                                                             
                                                                                
---Computing \DiGamma(a) - \DiGamma(1) via the monodromy formula above          
--- and at the same time all PolyGamma functions:                               
--- Poly_Gamma(m,a) = \Psi^(m)(a) for m = 1,...,M:                              
DiGamma_DiGamma1(a:F):F==                                                           
  md :SI := 1 ; z :F :=0.5                                                             
  Y1 : VF := Riemann_Phis(a,md,z) ; Y2 : VF := Auxiliary_Phis(a,md,1.0 - z)               
  -( z**(a) * ( apply(Y2,0) + apply(Y1,0) ) + log( 1.0 - z ) )                                
--Next M=1: You get both \DiGamma(a)-\DiGamma(1) and \Psi^(1)(a):               
Poly_Gamma_1(a:F):VF==
  import from SI
  import from String
  z:F :=0.5                                                                       
  Y1:VF := Riemann_Phis(a,2,z) ; Y2:VF := Auxiliary_Phis(a,2,1.0 - z)
  --print<<"Y1 = " << Y1<<newline
  --print<<"Y2 = " << Y2<<newline
  res:F:= z**(a) * ( apply(Y2,0) + apply(Y1,0) )
  --print<<"res = " << res<<newline
  vres : VF := new(2, 0.0)
  set!(vres,0, -res-log(1.0-z))
  set!(vres,1, -log(z)*res-z**(a)*(apply(Y2,1)-apply(Y1,1)))
  vres
                                                                                
----Now general polylogarithmic functions \Psi^(m)(a)                           
Poly_Gamma_DEBUG(M:SI,a:F):VF==                                                         
  z :F :=0.5         ; ic :F:= 1                                                     
  Y1:VF := Riemann_Phis(a,M+1,z) ; Y2 :VF:= Auxiliary_Phis(a,M+1,1.0 - z)             
----We have to modify Riemann_Phis to make them into:                           
---- (d/da)^m { sum_{n=0}^\infinty z^n/(n+a) }, i.e. to multiply it             
---- by (-1)^m*(m-1)! for m=0,...                                               
  for m:SI in 1..M repeat                                                          
    ic   := -ic * coerce(m)                                                             
    set!(Y1,m,  ic * apply(Y1,m))                                                          
----now we create couple of auxiliary arrays: Y2 = Y1 + Y2;                     
  for m:SI in 0..M repeat                                                          
    set!(Y2,m, apply(Y1,m) + apply(Y2,m))
---- and Y2.m = sum_{k=0}^m binom(m,k)*(log(a))**k*Y2.(m-k)                     
---- this is not the best way to do it : remember polynomial shift!             
  zl:F := log(z) ; za:F := z**a                                                     
  set!(Y1,0, -za * apply(Y2,0) - log(1.0 - z))
  for m:SI in 1..M repeat                                                          
    for k:SI in 0..(M-1) repeat                                                    
      set!(Y2,k, zl * apply(Y2,k) + apply(Y2,(k+1)))                                              

    set!(Y2,M, zl * apply(Y2,M))                                                           
    set!(Y1,m, -za * apply(Y2,0))                                                          
  Y1                                                                            

-------------------------------- TestRun -------------------------------------
import from F
print<<DiGammaDiGamma1(2.3)<<newline
