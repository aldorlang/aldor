-- testare usa testmat1.as
#pile
#include "axllib.as"
#library DemoLib "axldem"
import from DemoLib

Vector ==> Array
SI     ==> SingleInteger

IntegralDomain : Category == Ring with
   unit? : % -> Boolean
   exquo : (%,%) -> Partial %

MatrixOpDom(R:IntegralDomain) : MOD  == Definition where
   Mat ==> Matrix R

   MOD ==>  with

      rank: Mat -> SingleInteger
        ++ \spad{rank(m)} returns the rank of the matrix m.
      nullity: Mat -> SingleInteger
        ++ \spad{nullity(m)} returns the mullity of the matrix m. This is
        ++ the dimension of the null space of the matrix m.
      fractionFreeGauss! : Mat -> Mat
        ++ \spad{fractionFreeGauss(m)} performs the fraction 
        ++ free gaussian  elimination on the matrix m.
      invertIfCan : Mat -> Partial Mat
        ++ \spad{invertIfCan(m)} returns the inverse of m over R
      adjoint : Mat -> Record(adjMat:Mat, detMat:R)
        ++ \spad{adjoint(m)} returns the ajoint matrix of m (i.e. the matrix
        ++ n such that m*n = determinant(m)*id) and the detrminant of m.

   Definition ==>  add

     import from SingleInteger
     import from Mat
     import from Partial R
     import from List R

     lastStep  : Mat -> Mat

     rowAllZeroes?: (Mat,SingleInteger) -> Boolean

     rowAllZeroes?(x:Mat,i:SingleInteger) : Boolean ==
       -- determines if the ith row of x consists only of zeroes
       -- internal function: no check on index i
       for j in 1..nCols x repeat
         ~zero? x(i,j) => return false
       true

      -- internal function : x is a nrx(nr+nr) matrix
     lastStep(x:Mat)  : Mat ==
        nr := nRows x        
        det:=x(nr,nr)
        dnr : SI := nr+nr
        iCol:= nr-1
        for i in (nr-1)..1 by -1 repeat
          for j in (nr+1)..dnr repeat          
            ss:=reduce(+,[x(i,iCol+k)*x(i+k,j) for k in 1..(nr-i)],0)
            x(i,j) := retract exquo(det * x(i,j) - ss,x(i,nr-iCol))
          iCol:=iCol-1
        subMatrix(x,1,nr,nr+1,dnr)

     rank(x:Mat) : SingleInteger ==
        y :=
          (rk := nRows x) > (rh := nCols x) =>
            rk := rh
            transpose x
          copy x
        y := fractionFreeGauss! y 
        i := rk
        while rk > 0 and rowAllZeroes?(y,i) repeat
          i := i - 1
          rk := (rk - 1)
        rk 

     nullity(x:Mat) : SingleInteger == nCols x - rank x

     determinant(y:Mat) : R  ==
        (nr:=nRows y) ~= (nCols y) => error "determinant: matrix must be square"
        fm:=fractionFreeGauss!(copy y)
        fm(nr,nr)

      -- Fraction-Free Gaussian Elimination
     fractionFreeGauss!(x:Mat) : Mat  ==
        (nr := nRows x) = 1 => x
        nc := nCols x
        ans := 1$R
        b := 1$R
        i:SI:=1
        for j in 1..nc repeat
          if zero? x(i,j) then      -- candidate for pivot = 0
            rown : SI := 0
            for k in (i+1)..nr repeat
              if ~zero? x(k,j)  then
                 rown := k -- found a pivot
                 break
            if rown > 0 then
               swapRows!(x,i,rown)
               ans := -ans
          zero?(c := x(i,j)) =>  iterate  -- try next column
          for k in (i+1)..nr repeat
            if zero? x(k,j)  then
              for l in (j+1)..nc repeat
                x(k,l) := retract exquo(c * x(k,l),b)
            else
              pv := x(k,j)
              x(k,j) := 0
              for l in (j+1)..nc repeat
                val := c * x(k,l) - pv * x(i,l)
                x(k,l) := retract exquo(val,b)
          b := c
          (i := i+1) > nr => break
        if ans=-1 then
          lasti := i-1
          for j in 1..nc repeat x(lasti, j) := -x(lasti,j)
        x
 
     invertIfCan(y:Mat) : Partial Mat  ==
        (nr:=nRows y) ~= (nCols y) => error "invertIfCan: matrix must be square"
        x := horizConcat(copy y,scalarMatrix(nr,1$R))
        ffr:= fractionFreeGauss!(x)
        not unit?(fdet:= ffr(nr,nr)) => failed
        lastStep(ffr) :: Partial Mat

     adjoint(y:Mat): Record(adjMat:Mat, detMat:R) ==
        (nr:=nRows y) ~= (nCols y) => error "invertIfCan: matrix must be square"
        x := horizConcat(copy y,scalarMatrix(nr,1$R))
        ffr:= fractionFreeGauss!(x)
        det:=ffr(nr,nr)
        [lastStep(ffr),det]


