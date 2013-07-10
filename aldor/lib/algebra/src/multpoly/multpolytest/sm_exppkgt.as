--------------------------------------------------------------------------------
--
-- sm_exppkgt.as: A package for testing domains from ExponentCategory
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO, University of Lille I, 2001-2002
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- pkg ExponentCategoryTestPackage

#include "algebra"
#include "aldorio"

macro {
        Z == Integer;
}

ExponentCategoryTestPackage(V: VariableType, E: ExponentCategory(V)): with {
        exponentTest: (List V, testting?: Boolean == true) -> Boolean;
} == add {
        import from Z, V, E;
        exponentTest(lv: List V, debugging?: Boolean == true): Boolean == {
           import from List(V);
           -- assert(false);
           assert(not empty? lv); x: V := first(lv); lv := rest(lv); 
           assert(not empty? lv); y: V := first(lv); lv := rest(lv);
           assert(not empty? lv); z: V := first(lv);

           assert(x > y); assert(y > z);
           lv: List V := [x,y,z];
           e__x: E := exponent(x);
           e__y: E := exponent(y);
           e__z: E := exponent(z);
           e__0: E := 0;
           not zero? e__0 => {if debugging? then stdout << "1" << newline; return false; }

           e__0 ~= gcd(e__x,e__y) => {if debugging? then stdout << "2" << newline; return false; }
           e__0 ~= gcd(e__y,e__z) => {if debugging? then stdout << "3" << newline; return false; }
           e__0 ~= gcd(e__z,e__x) => {if debugging? then stdout << "4" << newline; return false; }

           lz := [1,1,0]; 
           e__xy: E := exponent(lv,lz);
           lz := [1,0,1]; 
           e__xz: E := exponent(lv,lz);
           lz := [0,1,1]; 
           e__yz: E := exponent(lv,lz);
           lz: List(Z) := [1,1,1]; 
           e__xyz: E := exponent(lv,lz);

           (e__x + e__0 ~= e__x) => {if debugging? then stdout << "5" << newline; return false; }
           (e__x + e__y ~= e__xy) => {if debugging? then stdout << "6" << newline; return false; }
           (e__y + e__z ~= e__yz) => {if debugging? then stdout << "7" << newline; return false; }
           (e__z + e__x ~= e__xz) => {if debugging? then stdout << "8" << newline; return false; }

           e__x ~= gcd(e__xy,e__xz) => {if debugging? then stdout << "9" << newline; return false; }
           e__y ~= gcd(e__xy,e__yz) => {if debugging? then stdout << "10" << newline; return false; }
           e__z ~= gcd(e__xz,e__yz) => {if debugging? then stdout << "11" << newline; return false; }
           e__0 ~= gcd(e__0,e__xz)  => {if debugging? then stdout << "12" << newline; return false; }
           e__0 ~= gcd(e__0,e__z)   => {if debugging? then stdout << "13" << newline; return false; }
           e__0 ~= gcd(e__0,e__xyz) => {if debugging? then stdout << "14" << newline; return false; }

           e__xyz~= lcm(e__xy,e__xz) => {if debugging? then stdout << "15" << newline; return false; }
           e__xyz~= lcm(e__xy,e__yz) => {if debugging? then stdout << "16" << newline; return false; }
           e__xyz~= lcm(e__xz,e__yz) => {if debugging? then stdout << "17" << newline; return false; }

           tmp: E := lcm(e__x,e__y);
           e__xy2z3: E := add!(add!(tmp,y,1),z,3);
           e__xy2z3 ~= tmp => {if debugging? then stdout << "18" << newline; return false; }
           degree(e__xy2z3,x) ~= 1 => {if debugging? then stdout << "19" << newline; return false; }
           degree(e__xy2z3,y) ~= 2 => {if debugging? then stdout << "20" << newline; return false; }
           degree(e__xy2z3,z) ~= 3 => {if debugging? then stdout << "21" << newline; return false; }

           lz: List(Z) := [3,2,1]; 
           e__x3y2z: E := exponent(lv,lz);
           (e__y + e__xyz) ~= gcd(e__xy2z3, e__x3y2z) => {if debugging? then stdout << "22" << newline; return false; }

           le: List(E) := [e__x, e__y, e__z, e__xyz, e__xy, e__xz, e__yz, e__xy2z3, e__x3y2z];
           lv := [x,y,z,x,x,x,y,x,x];
           ltd: List(Z) := [1,1,1,3,2,2,2,6,6];
           for e in le for v in lv for td in ltd repeat {
               zero? e => {if debugging? then stdout << "23" << newline; return false; }
               e ~= exponent terms(e) => {if debugging? then stdout << "24" << newline; return false; }
               v?: Partial(V) := mainVariable(e);
               retract(v?) ~= v => {if debugging? then stdout << "25" << newline; return false; }
               totalDegree(e) ~= td
           }

           le := cons(e__0,le);
           for e in le repeat {
               for f in le repeat {
                   (a: E, b: E) := syzygy(e,f);
                   c: E := lcm(e,f);
                   (a + e ~= c) => {if debugging? then stdout << "26" << newline; return false; }
                   (b + f ~= c) => {if debugging? then stdout << "27" << newline; return false; }
                   g?: Partial(E) := cancelIfCan(e,f);
                   if cancel?(e,f) then {
                      g: E := cancel(e,f);
                      (g + f ~= e) => {if debugging? then stdout << "28" << newline; return false; }
                      failed? g? => {if debugging? then stdout << "29" << newline; return false; }
                      gg: E := retract(g?);
                      (g ~= gg)  => {if debugging? then stdout << "30" << newline; return false; }
                    } else {
                      not failed? g? => {if debugging? then stdout << "31" << newline; return false; }
                    }
               }
           }
	   stdout << " done " ;
           true;
      }
}
