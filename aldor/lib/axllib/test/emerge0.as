
#include "axllib"

--> testcomp
--> testrun -Q3 -Qemerge -laxllib

----------------------------------------------------------------
-- This test used to return convergence failure after lots of
-- iterations. It was due to a bug in emerge which failed to
-- deal with aliases correctly when recomputing locals.
----------------------------------------------------------------

R ==> DoubleFloat;
I ==> SingleInteger;

+++ quanc8: Quadrature, Newton-Cotes 8-panel
+++
+++ (This is a literal translation of the Fortran program given
+++  in ``Computer Methods for Mathematical Computations'' by Forsythe, 
+++  Malcolm and Moler, Prentice-Hall 1977.)
+++
+++ Estimate the integral of fun(x) from a to b to a given tolerance.
+++ An automatic adaptive routine based on the 8-panel newton-cotes rule.
+++
+++ Input:
+++   fun     The name of the integrand function subprogram f(x).
+++   a       The lower limit of integration.
+++   b       The upper limit of integration. (b may be less than a.)
+++   relerr  A relative error tolerance. (Should be non-negative)
+++   abserr  An absolute error tolerance. (Should be non-negative)
+++
+++ Output:
+++   result  An approximation to the integral hopefully satisfying
+++           the least stringent of the two error tolerances.
+++   errest  An estimate of the magnitute of the actual error.
+++   nofun   The number of function values used in calculation of result.
+++   flag    A reliability indicator.  If flag is zero, then result
+++           probably satisfies the error tolerance.  If flag is
+++           xxx.yyy then xxx = the number of intervals which have
+++           not converged and 0.yyy = the fraction of the interval
+++           left to do when the limit on nofun was approached.

quanc8(fun: R -> R, a: R, b: R, abserr: R, relerr: R):
      (Xresult: R, Xerrest: R, Xnofun: I, Xflag: R)
== {
        local result, errest, flag: R;
        local nofun: I;
        RETURN ==> return (result, errest, nofun, flag);

        local w0, w1, w2, w3, w4, area, x0, f0, stone, step, cor11: R;
        local qprev, qnow, qdiff, qleft, esterr, tolerr, temp: R;
        default i, j : I;

        qright: Array R       := new(31, 0.0);
        f:      Array R       := new(16, 0.0);
        x:      Array R       := new(16, 0.0);
        fsave:  Array Array R := [new(30, 0.0) for i in 1..8];
        xsave:  Array Array R := [new(30, 0.0) for i in 1..8];

        local levmin, levmax, levout, nomax, nofin, lev, nim: I;

        --
        -- *** Stage 1 ***    General initializations
        -- Set constants
        --
        levmin := 1;
        levmax := 30;
        levout := 6;
        nomax  := 5000;
        nofin  := nomax - 8 * (levmax - levout + 2 ^ (levout + 1));
        --
        -- Trouble when nofun reaches nofin
        --
        w0 :=   3956.0 / 14175.0;
        w1 :=  23552.0 / 14175.0;
        w2 :=  -3712.0 / 14175.0;
        w3 :=  41984.0 / 14175.0;
        w4 := -18160.0 / 14175.0;
        --
        -- Initialize running sums to zero.
        --
        flag := 0.0;
        result := 0.0;
        cor11 := 0.0;
        errest := 0.0;
        area := 0.0;
        nofun := 0;
        if a = b then RETURN;
        --
        -- *** Stage 2 ***   Iniitalization for first interval
        --
        lev := 0;
        nim := 1;
        x0  := a;
        x(16) := b;
        qprev := 0.0;
        qleft := 0.0;
        f0 := fun(x0);
        stone := (b - a)/16.0;
        x(8)  := (x0   + x(16)) / 2.0;
        x(4)  := (x0   + x(8) ) / 2.0;
        x(12) := (x(8) + x(16)) / 2.0;
        x(2)  := (x0   + x(4) ) / 2.0;
        x(6)  := (x(4) + x(8) ) / 2.0;
        x(10) := (x(8) + x(12)) / 2.0;
        x(14) := (x(12)+ x(16)) / 2.0;
        for j in 2..16 by 2 repeat
                f(j) := fun(x(j));
        nofun := 9;
        --
        -- *** Stage 3 ***   Central calculation
        -- Requires qprev,x0,x1,...,x16,f0,f2,f4,...,f16.
        -- Calculates x1,x3,...x15, f1,f3,...f15,qleft,qright,
	--            qnow,qdiff,area.
        --
@L30    print << "****************- (stage 3) -****************" << newline;
        x(1) := (x0 + x(2)) / 2.0;
        f(1) := fun(x(1));
        for j in 3..15 by 2 repeat {
                x(j) := (x(j-1) + x(j+1)) / 2.0;
                f(j) := fun(x(j));
        }
        nofun := nofun + 8;
        step := (x(16) - x0) / 16.0;
        qleft := (w0*(f0+f(8)) + w1*(f(1)+f(7)) + w2*(f(2)+f(6)) +
                  w3*(f(3)+f(5)) + w4*f(4)) * step;
        qright(lev+1) := (w0*(f(8) + f(16))+w1*(f(9)+f(15))+w2*(f(10)+
                  f(14)) + w3*(f(11)+f(13)) + w4*f(12)) * step;
        qnow := qleft + qright(lev+1);
        qdiff := qnow - qprev;
        area := area + qdiff;
        --
        -- *** Stage 4 ***   Interval convergence test
        --
print << "****************- (stage 4) -****************" << newline;
        esterr := abs(qdiff) / 1023.0;
        tolerr := max(abserr, relerr*abs(area)) * (step/stone);
        if lev < levmin then goto L50;
        if lev >=levmax then goto L62;
        if nofun > nofin then goto L60;
        if esterr <= tolerr then goto L70;
        --
        -- *** Stage 5 ***   No convergence
        -- Locate next interval
        --
@L50    print << "****************- (stage 5) -****************" << newline;
        nim := 2*nim;
        lev := lev + 1;
        --
        -- Store right hand elements for future use.
        --
        for i in 1..8 repeat {
                fsave(i)(lev) := f(i+8);
                xsave(i)(lev) := x(i+8);
        }
        --
        -- Assemble left hand elements for immediate use.
        --
        qprev := qleft;
        for i in 1..8 repeat {
                j := -i;
                f(2*j+18) := f(j+9);
                x(2*j+18) := x(j+9);
        }
        goto L30;
        --
        -- *** Stage 6 ***   Trouble section
        -- Number of function values is about to exceed limit.
        --
@L60    print << "****************- (stage 6) -****************" << newline;
        nofin := 2*nofin;
        levmax := levout;
        flag := flag + (b - x0)/(b - a);
        goto L70;
        --
        -- Current level is levmax.
        --
@L62    flag := flag + 1;
        --
        -- *** Stage 7 ***   Interval converged
        -- Add contributions into running sums.
        --
@L70    print << "****************- (stage 7) -****************" << newline;
        result := result + qnow;
        errest := errest + esterr;
        cor11 := cor11 + qdiff / 1023.0;
        --
        -- Locate next interval.
        --
@L72    if nim = 2*(nim quo 2) then goto L75;
        nim := nim quo 2;
        lev := lev - 1;
        goto L72;

@L75    nim := nim + 1;
        if lev <= 0 then goto L80;
        --
        -- Assemble elements required for the next interval.
        --
        qprev := qright(lev);
        x0 := x(16);
        f0 := f(16);
        for i in 1..8 repeat {
                f(2*i) := fsave(i)(lev);
                x(2*i) := xsave(i)(lev);
        }
        goto L30;
        --
        -- *** Stage 8 ***   Finalize and return
        --
@L80    result := result + cor11;
        --
        -- Make sure errest not less than roundoff level.
        --
        if errest = 0.0 then RETURN;
@L82    temp := abs(result) + errest;
        if temp ~= abs(result) then RETURN;
        errest := 2.0 * errest;
        goto L82;
}

import from R, I, String, Format, Character;

f(x:R):R == 4.0/(x*x+1);

(result, errest, nofun, flag) := quanc8(f,0.0,1.0,0.00001,0.00001);

if zero? flag then {
--
--  print << "result = " << result << " with error " << errest << newline;
--
--  Replaced for version 1.1.0
--
--  The goal of this code is to only print out 5 digits to the right of the
--  decimal (possibly followed by an exponent).  Because the expected error
--  value is around e-12 and the precision of the answer is around 16 digits
--  printing out additional digits is showing error beyond the precision
--  that the number is capable of holding.  As a result, any difference in
--  these digits isn't important, and we don't want to find a difference in
--  those later digits.
--
  local bstr : String := new(50);
  print << "result = " << result << " with error ";
  format(errest,bstr,1);

  local iii : I := 1;
  local prn : I := 1;
  while (not end?(bstr,iii)) repeat {
    if (iii = 8) then {
      prn := 0;
    }
    if (ord(bstr(iii)) = ord("e"(1))) then {
      prn := 1;
    }
    if (prn = 1) then {
      print << bstr(iii);
    }
    iii := iii + 1;
  }
  print << newline;

  print << "after " << nofun << " function evaluations." << newline;
}
else
  print << "error flag is " << flag << newline;
