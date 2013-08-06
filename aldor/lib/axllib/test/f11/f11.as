-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -Mno-ALDOR_W_FunnyEscape
--> testrun -Mno-ALDOR_W_FunnyEscape -l axllib
#pile

#include "axllib.as"

--------- CONFLUENT HYPERGEOMETRIC PROGRAMS -------------                       
--- Computes classical special functions including:                             
---  _1F_1's; _2F_0's; _0F_1's - in particularly:                               
---  J_v,I_v,K_v,Y_v,erf,erfc,incomplete gamma and                              
---  incomplete Gamma, integral sin, cos, logarithm,...                         
---  AND - attention! - Euler gamma function too!                               
---  (that is important)                                                        
---  everywhere (in the complex plane) with any precision!                      
--- ***************************************************                         
---  Definitions:                                                               
---    Usual notations for confluent hypergeometric functions are               
---    the following:                                                           
---    Confluent_M(a,c,z) = \sum_{n=0}^\infnty {(a)_n/(c)_n} x^n/n!             
---    More familiar notations: _1F_1(a;c;z) or \Phi(a,c;z).                    
---    These were entire functions, and the other class of                      
---    confluent hypergeometric functions (of _2F_0 kind)                       
---    Confluent_U(a,c,z) = {1/Gamma(a)} \int_0^\infnty                         
---      exp(-zt) t^{a-1} (1+t)^{b-a-1} dt.                                     
---    Other notations are: \Psi(a,c;z) or in _2F_0 notations:                  
---    U(a,c;z) = z^{-a} _2F_0(a,a-c+1;-1/z).                                   
--- ***************************************************                         

macro I == Integer
macro F == DoubleFloat
macro ** == ^

import from I
import from F
import from String
import from Segment I

--  FIRST - the "epsilon" of the roundoff error - can be made                   
--  even smaller      
digits():I == 15                                                          
epsabs:F==10.0**(-digits())



--  NEXT - the "absolute maximum" on the number of terms in any                 
--  series to be computed (just in case), if we overshoot,                      
--  the user has to be notified and the approximate result will                 
--  be shown                                                                    
MAXNTERMS:I==100000
                                                                                
--  The usual hypergeometric power series for _1F_1 - watch                     
--    for the largest term in series - it should not exceed                     
--    the precision; safest is to take abs(x) << O(1).                          
f11_0(a:F,c:F,x:F):Record(left:F,right:F)== 
--the jlimit is the number of terms to be zero that will guarantee              
--convergence                                                                   
  jlimit:I:=5                                                                   
  y:F:=1.0                                                                 
  z:F:=0.0                                                                 
  coef:F:=1.0                                                              
  jfin:I:=0
  for ii:Integer in 0..MAXNTERMS while jfin<jlimit repeat
    i:F := coerce(ii)
    coef:=coef*(a+i)/((c+i)*(1.0+i))                                            
    z1:F  :=z+coef*(1.0+i)                                                        
    coef:=coef*x                                                                
    y1:F  :=y+coef                                                                
    if ( (y1 = y) and (z1 = z) ) then                                           
      jfin:=jfin+1                                                              
      print<<" took "<<i<<" terms"<<newline
    y:=y1 ; z:=z1                                                               
    print<<"at i "<<i<<" y "<<y<<newline
  [y,z]                                                                         
                                                                                
-- ***********************************************************                  
-- CONFLUENT HYPERGEOMETRIC FUNCTIONS, PART II                                  
--  Here we should restric ourself to real a, c                                 
--  (it is NOT necessary);                                                      
--  Power series (asymptotic series) computations can proceed                   
--  best when a, c are near origin ( abs. values in [0,2) say)                  
--  The reduction to this case needs only the recurrence in a                   
--  and in c (though watch for a possible stability and use                     
--  better backward recurrence)                                                 
-- ***********************************************************                  
                                                                                
--  The analytic continuation of an arbitrary solution of the                   
--  _1F_1 hypergeometric equation xy"+(c-x)y'-ay = 0                            
--  from x=x0 to x=x1.  We supply initial conditions:                           
--  y0=y(x0), z0=y'(x0) and determine y1=y(x1), z1=y'(x1)                       
--    Again (as with any exponential like series) watch                         
--    for the largest term in series - it should not exceed                     
--    the precision; safest is to take abs(x1-x0) << O(1).                      
f11_1(a:F,c:F,x0:F,y0:F,z0:F,x1:F):Record(left:F,right:F)==
--the jlimit is the number of terms to be zero that will guarantee              
--convergence                                                                   
  jlimit:I:=5       ; jfin:I:=0                                                 
  t:F  :=x1 - x0                                                                  
  c0:F :=y0 ; c1:F:=z0 * t  ; c2:F:=0.0                                   
  y1:F :=c0 + c1      ; z1:F:=z0
  for ii:Integer in 0..MAXNTERMS while jfin<jlimit repeat
    i:F := coerce(ii)
    c2:=( (i+1.0)*(x0-c-i)*c1 + (a+i)*c0 * t )/( x0 * (i+1.0) )                 
    z2:F:=z1 + c2                                                                 
    c0:=c1          ; c1:=(c2 * t)/(i + 2.0)                                    
    y2:F:=y1 + c1                                                                 
    if ( (y2 = y1) and (z2 = z1) ) then                                         
      jfin:=jfin+1                                                              
      print<<" took "<<i<<" terms"<<newline
    y1:=y2          ; z1:=z2                                                    
    print<<"at i "<<i<<" y "<<y1<<newline
  [y1,z1]                                                                       
                                                                                
--  the "absolute maximum" on the number of terms in asymptotics                
--  series to be computed (just in case), if we overshoot,                      
--  the user has to be notified and the approximate result will                 
--  be shown                                                                    
MAXNATERMS:I==1000
                                                                                
--  The asymptotic hypergeometric power series for _2F_0 - use it               
--    only for very small values of x - do not exceed precision                 
--    too much;                                                                 
f20_0(a:F,c:F,x:F):Record(left:F,right:F)==
--the jlimit is the number of terms to be zero that will guarantee              
--convergence                                                                   
  jlimit:I:=3       ; jfin:I:=0                                                 
  y:F:=1.0     ; z:F:=0.0      ; coef:F:=1.0
  for ii:Integer in 0..MAXNATERMS while jfin<jlimit repeat
    i:F := coerce(ii)
    coef:=coef * (a + i) * (c + i)                                              
    z1:F  :=z + coef                                                              
    coef:=coef * x/(1.0 + i)                                                    
    y1:F  :=y + coef                                                              
    if ( (y1 = y) and (z1 = z) ) then                                           
      jfin:=jfin+1                                                              
      print<<" took "<<i<<" terms"<<newline
    y:=y1 ; z:=z1                                                               
    print<<"at i "<<i<<" y "<<y<<newline
  [y,z]                                                                         
                                                                                
-------------------------------- TestRun -----------------------------------
yz : Record(left:F, right: F) := f110(1.0,1.0,1.0)
print<<apply(yz,left)<<newline
print<<apply(yz,right)<<newline
