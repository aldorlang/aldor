#include "axllib.as"
#pile

Vector ==> Array
SI     ==> SingleInteger

+++ Category of matrices .

MatrixCategory(R : Ring): Category == Definition where
   Row ==> Vector R
   Col ==> Vector R 

   Definition ==>   BasicType  with

     export from R

     square?  : % -> Boolean
       ++ `square?(m)' returns true if m is a square matrix
       ++ (i.e. if m has the same number of rows as columns) and false otherwise.

     diagonal?: % -> Boolean
       ++ `diagonal?(m)' returns true if the matrix m is square and
       ++ diagonal (i.e. all entries of m not on the diagonal are zero) and
       ++ false otherwise.

     symmetric?: % -> Boolean
       ++ `symmetric?(m)' returns true if the matrix m is square and
       ++ symmetric (i.e. `m[i,j] = m[j,i]' for all i and j) and false
       ++ otherwise.

     antisymmetric?: % -> Boolean
       ++ `antisymmetric?(m)' returns true if the matrix m is square and
       ++ antisymmetric (i.e. `m[i,j] = -m[j,i]' for all i and j) and false
       ++ otherwise.

--% Size inquiries
     #  :  % -> SingleInteger
       ++ #(m) size of m
 
     nRows :  %  -> SingleInteger
       ++ nRows(m) is the number of rows of m

     nCols : % -> SingleInteger
       ++ nCols(m) is the number of columns of m

--% Creation 
     copy : % -> %
      ++ copy(m) creates a new matrix equal to m
 
     new : (SingleInteger,SingleInteger,R) -> %
      ++ new(m,n,r) is a  matrix  all of whose entries are r

     zero: (SingleInteger,SingleInteger) -> %
      ++ `zero(m,n)' returns an m-by-n zero matrix.

     matrix: List List R -> %
       ++ `matrix(l)' converts the list of lists l to a matrix, where the
       ++ list of lists is viewed as a list of the rows of the matrix.
 
     matrix: Vector Vector R -> %
       ++ `matrix(l)' converts the vector of vectors l to a matrix, where 
       ++ l is viewed as the vector of rows of the matrix.

     matrix: Tuple Vector R -> %
       ++ `matrix(r1,r2,..,rn)' creates a matrix whose rows are
       ++ the vectors ri.

     scalarMatrix: (SingleInteger,R) -> %
       ++ `scalarMatrix(n,r)' returns an n-by-n matrix with r's on the
       ++ diagonal and zeroes elsewhere.

     diagonalMatrix: List R -> %
       ++ `diagonalMatrix(l)' returns a diagonal matrix with the elements
       ++ of l on the diagonal.

     coerce: Col -> %
       ++ `coerce(col)' converts the column col to a column matrix.

     transpose: Row -> %
       ++ `transpose(r)' converts the row r to a row matrix.

--% Creation of new matrices from old

     transpose: % -> %
       ++ `transpose(m)' returns the transpose of the matrix m.

     squareTop: % -> %
       ++ `squareTop(m)' returns an n-by-n matrix consisting of the first
       ++ n rows of the m-by-n matrix m. Error: if
       ++ `m < n'.

     horizConcat: (%,%) -> %
       ++ `horizConcat(x,y)' horizontally concatenates two matrices with
       ++ an equal number of rows. The entries of y appear to the right
       ++ of the entries of x.  Error: if the matrices
       ++ do not have the same number of rows.

     vertConcat: (%,%) -> %
       ++ `vertConcat(x,y)' vertically concatenates two matrices with an
       ++ equal number of columns. The entries of y appear below
       ++ of the entries of x.  Error: if the matrices
       ++ do not have the same number of columns.


--% Part extractions/assignments

     listOfLists: % -> List List R
       ++ `listOfLists(m)' returns the rows of the matrix m as a list
       ++ of lists.

     apply: (%,SingleInteger,SingleInteger) -> R
       ++ x(i,j)
     apply: (%,List SingleInteger,List SingleInteger) -> %
       ++ `apply(x,rowList,colList)' returns an m-by-n matrix consisting
       ++ of elements of x, where `m = # rowList' and `n = # colList'.
       ++ If `rowList = [i<1>,i<2>,...,i<m>]' and \spad{colList =
       ++ [j<1>,j<2>,...,j<n>]}, then the `(k,l)'th entry of
       ++ `apply(x,rowList,colList)' is `x(i<k>,j<l>)'.

     set!: (%, SingleInteger, SingleInteger, R) -> R
       ++ x(i,j):=c
     set!: (%,List SingleInteger,List SingleInteger, %) -> %
       ++ `set!(x,rowList,colList,y)' destructively alters the matrix x.
       ++ If y is `m'-by-`n', `rowList = [i<1>,i<2>,...,i<m>]'
       ++ and `colList = [j<1>,j<2>,...,j<n>]', then `x(i<k>,j<l>)'
       ++ is set to `y(k,l)' for `k = 1,...,m' and `l = 1,...,n'.

     row: (%,SingleInteger) -> Row
       ++ row(m,i) returns the ith row of m
       ++ error check to determine if index is in proper ranges

     column: (%,SingleInteger) -> Col
       ++ column(m,j) returns the jth column of m
       ++ error check to determine if index is in proper ranges

     parts: % -> List R
       ++ parts(m) returns a list of the elements of m in row major order


     swapRows!: (%,SingleInteger,SingleInteger) -> %
       ++ `swapRows!(m,i,j)' interchanges the `i'th and `j'th
       ++ rows of m. This destructively alters the matrix.

     swapColumns!: (%,SingleInteger,SingleInteger) -> %
       ++ `swapColumns!(m,i,j)' interchanges the `i'th and `j'th
       ++ columns of m. This destructively alters the matrix.

     subMatrix: (%,SingleInteger,SingleInteger,SingleInteger,SingleInteger) -> %
       ++ `subMatrix(x,i1,i2,j1,j2)' extracts the submatrix
       ++ `[x(i,j)]' where the index i ranges from `i1' to `i2'
       ++ and the index j ranges from `j1' to `j2'.

     setsubMatrix!: (%,SingleInteger,SingleInteger,%) -> %
       ++ `setsubMatrix(x,i1,j1,y)' destructively alters the
       ++ matrix x. Here `x(i,j)' is set to `y(i-i1+1,j-j1+1)' for
       ++ `i = i1,...,i1-1+nrows y' and `j = j1,...,j1-1+ncols y'.

--% Arithmetic

     +: (%,%) -> %
       ++ `x + y' is the sum of the matrices x and y.
       ++ Error: if the dimensions are incompatible.

     -: (%,%) -> %
       ++ `x - y' is the difference of the matrices x and y.
       ++ Error: if the dimensions are incompatible.

     -:  %    -> %
       ++ `-x' returns the negative of the matrix x.

     *: (%,%) -> %
       ++ `x * y' is the product of the matrices x and y.
       ++ Error: if the dimensions are incompatible.

     *: (R,%) -> %
       ++ `r*x' is the left scalar multiple of the scalar r and the
       ++ matrix x.

     *: (%,R) -> %
       ++ `x * r' is the right scalar multiple of the scalar r and the
       ++ matrix x.

--     *: (Integer,%) -> %
--       ++ `n * x' is an integer multiple.

     *: (%,Col) -> Col
       ++ `x * c' is the product of the matrix x and the column vector c.
       ++ Error: if the dimensions are incompatible.

     *: (Row,%) -> Row
       ++ `r * x' is the product of the row vector r and the matrix x.
       ++ Error: if the dimensions are incompatible.

Matrix(R : Ring): MD  == MatrixDefinition where
   Row ==> Vector R
   Col ==> Vector R 
   MD ==> MatrixCategory(R) with

   MatrixDefinition ==> add

     Rep == Vector Vector R
     import from Rep, Vector SingleInteger
     import from Vector R, R, SingleInteger

     #(x:%) : SI ==  (nRows x) * (nCols x)
 
     nRows(x:%) : SingleInteger == # rep x

     nCols(x:%) : SingleInteger == #((rep x)(1))

     square?(x:%) : Boolean  == nRows x = nCols x

     diagonal?(x:%) :Boolean ==
       ~(square? x) => false
       for i in 1..nRows x repeat
         for j in 1..nCols x | j~=i repeat
           x(i,j) ~= 0  => return false
       true

     symmetric?(x:%) : Boolean ==
       (nr:=nRows x) ~= nCols x => false
       for i in 1..(nr-1) repeat
         for j in (i+1)..nr  repeat
           x(i,j) ~= x(j,i)  => return false
       true

     antisymmetric?(x:%) : Boolean  ==
       (nr:=nRows x) ~= nCols x => false
       for i in 1..(nr-1) repeat
         x(i,i) ~= 0 => return false
         for j in (i+1)..nr  repeat
           x(i,j) ~= - x(j,i)  => return false
       true

--% Creation of matrices
     copy(x:%) : % ==
       res:=new(nr:=nRows x,nc:=nCols x,0$R)
       for i in 1..nr repeat for j in 1..nc repeat res(i,j) := x(i,j)
       res

     new(rows:SI,cols:SI,r:R) : % == 
       per[new(cols,r) for i in 1..rows]

     zero(rows:SI,cols:SI) : % == new(rows,cols,0)

     sample: % == zero(1,1)

     matrix(l: List List R): % ==
       import from List R
       empty? l => new(0,0,0)
       -- error check: this is a top level function
       rows : SI := 1; cols := # first l
       cols = 0 => error "matrix with 0 columns must have 0 rows"
       for ll in rest l repeat
         cols ~= # ll => error "matrix: rows of different lengths"
         rows := rows + 1
       ans := new(rows,cols,0)
       for i in 1..(nRows ans) for ll in l repeat
         for j in 1..(nCols ans)  for r in ll repeat ans(i,j) :=r
       ans

     matrix(l: Vector Vector R): % == per l

     matrix(l: Tuple Vector R): % ==
       zero?(n:=length l) => error "empty matrices are not valid"
       m:Vector Vector R := new(n,element(l,1))
       for i in 2..n repeat m.i := element(l,i)
       per m

     scalarMatrix(n:SI,r:R) : %  ==
       ans := zero(n,n)
       for i in 1..n  repeat ans(i,i):= r
       ans

     diagonalMatrix(l: List R) : %  ==
       n := #l; ans := zero(n,n)
       for i in 1..n repeat ans(i,i) := l(i)
       ans

     coerce(v:Col) : % ==
       n:=#v
       x := new(n,1,0)
       for i in 1..n repeat x(i,1):= v(i)
       x

     transpose(v:Row) : %  ==
       n := #v
       x := new(1,n,0)
       for i in 1..n repeat x(i,1) := v(i)
       x

     transpose(x:%) : % ==
       nr:= nRows x
       nc:=nCols x
       ans := new(nc,nr,0)
       for i in 1..nc repeat
         for j in 1..nr repeat ans(i,j) := x(j,i)
       ans

     squareTop(x : %) : % ==
       nRows x < (cols := nCols x) =>
         error "squareTop: number of columns exceeds number of rows"
       ans := new(cols,cols,0)
       for i in 1..cols repeat 
         for j in 1..cols repeat ans(i,j) := x(i,j)
       ans

     horizConcat(x:%,y:%) : %  ==
       (rows := nRows x) ~= nRows y =>
         error "HConcat: matrices must have same number of rows"
       ans := new(rows,(cols := nCols x) + (ycols := nCols y),0)
       for i in 1..rows repeat
         for j in 1..cols repeat ans(i,j) := x(i,j)
       for i in 1..rows repeat
         for j in 1..ycols repeat ans(i,j + cols) := y(i,j)
       ans

     vertConcat(x:%,y:%) : %  ==
       (cols := nCols x) ~= nCols y =>
         error "HConcat: matrices must have same number of columns"
       ans := new((rows := nRows x) + (yrows:=nRows y),cols,0)
       for i in 1..rows repeat
         for j in 1..cols repeat ans(i,j) := x(i,j)
       for i in 1..yrows repeat
         for j in 1..cols repeat  ans(i+rows,j) :=y(i,j)
       ans

--% Part extraction/assignment

     listOfLists(x:%) : List List R  ==
       ll : List List R := []
       for i in nRows x..1 by -1 repeat
         l : List R := []
         for j in nCols x..1 by -1 repeat
           l := cons(x(i,j),l)
         ll := cons(l,ll)
       ll

     swapRows!(x:%,i1:SI,i2:SI) : % ==
       (i1 < 1) or (i1 > (nr:=nRows x)) or (i2 < 1) or _
           (i2 > nr) => error "swapRows!: index out of range"
       i1 = i2 => x
       for j in 1..nCols x repeat
         r := x(i1,j)
         x(i1,j) := x(i2,j)
         x(i2,j) := r
       x

     swapColumns!(x:%,j1:SI,j2:SI) : % ==
       (j1 < 1) or (j1 > (nc:=nCols x)) or (j2 < 1) or _
           (j2 > nc) => error "swapColumns!: index out of range"
       j1 = j2 => x
       for i in 1..nRows x repeat 
         r := x(i,j1)
         x(i,j1) := x(i,j2)
         x(i,j2) := r
       x

     apply(x:%,i:SI,j:SI) :R == ((rep x)(i))(j)

     apply(x:%,rowList:List SI,colList:List SI) : %  ==
       nr := nRows x
       nc := nCols x
       for ei in rowList repeat
         (ei < 1) or (ei > nr) =>
           error "apply: index out of range"
       for ej in colList repeat
         (ej < 1) or (ej > nc) =>
           error "apply: index out of range"
       y := new(# rowList,# colList,0)
       for ei in rowList for i in 1..nRows y repeat
         for ej in colList for j in 1..nCols y  repeat y(i,j) := x(ei,ej)
       y

     set!(x:%,i:SI,j:SI,r:R) : R ==
       (i < 1) or (i > (nr:=nRows x)) or              
         (j < 1) or (j > (nc:=nCols x)) =>  error "set!: index out of range"
       set!((rep x)(i),j,r)

     set!(x:%,rowList:List SingleInteger,colList:List SingleInteger,y:%) : %  ==
       nr := nRows x
       nc := nCols x
       for ei in rowList repeat
         (ei < 1) or (ei > nr) =>
           error "set!: index out of range"
       for ej in colList repeat
         (ej < 1) or (ej > nc) =>
           error "set!: index out of range"
       ((# rowList) ~= (nry:= nRows y)) or ((# colList) ~= (ncy:= nCols y)) =>
         error "set!: matrix has bad dimensions"
       for ei in rowList for i in 1..ncy repeat
         for ej in colList for j in 1..ncy repeat set!(x,ei,ej,y(i,j))
       y

     subMatrix(x:%,i1:SI,i2:SI,j1:SI,j2:SI) : %  ==
       (i2 < i1) => error "subMatrix: bad row indices"
       (j2 < j1) => error "subMatrix: bad column indices"
       (i1 < 1) or (i2 > (nr:=nRows x)) =>
         error "subMatrix: index out of range"
       (j1 < 1) or (j2 >(nc:=nCols x)) =>
         error "subMatrix: index out of range"
       rows := (i2 - i1 + 1) 
       cols := (j2 - j1 + 1) 
       y := new(rows,cols,0)
       for i in 1..rows for k in i1..i2 repeat
         for j in 1..cols for l in j1..j2 repeat set!(y,i,j,x(k,l))
       y

     setsubMatrix!(x:%,i1:SI,j1:SI,y:%) : % ==
       i2 := i1 + (nr:=nRows y) -1
       j2 := j1 + (nc :=nCols y) -1
       (i1 < 1) or (i2 > nRows x) =>
         error "setsubMatrix!: inserted matrix too big, use subMatrix to restrict it"
       (j1 < 1) or (j2 > nCols x) =>
         error "setsubMatrix!: inserted matrix too big, use subMatrix to restrict it"
       for i in 1..nr for k in i1..i2 repeat
         for j in 1..nc for l in j1..j2 repeat set!(x,k,l,y(i,j))
       x
 
     row(x:%,i:SI) : Row == rep(x)(i)

     column (x:%,j:SI):Col ==
       v:Col:=new(nc:=nCols x,0)
       for i in 1..(nRows x) repeat v(i) := x(i,j)
       v

     parts(x : %) : List R ==
       res:List R := []
       for i in (nRows x)..1 by -1 repeat
         for j in (nCols x)..1 by -1 repeat res:=cons(x(i,j),res)
       res

--% Arithmetic

     (x:%) + (y:%) : %  ==
       ((r := nRows x) ~= nRows y) or ((c := nCols x) ~= nCols y) =>
         error "can't add matrices of different dimensions"
       ans := new(r,c,0)
       for i in 1..r repeat
         for j in 1..c repeat  ans(i,j) := x(i,j) + y(i,j)
       ans

     (x:%) - (y:%) : %  ==
       ((r := nRows x) ~= nRows y) or ((c := nCols x) ~= nCols y) =>
         error "can't subtract matrices of different dimensions"
       ans := new(r,c,0)
       for i in 1..r repeat 
         for j in 1..c repeat  ans(i,j) := x(i,j) - y(i,j)
       ans

     -(x:%) : % == 
       ans := new((r:=nRows x),(c:=nCols x),0)
       for i in 1..r repeat 
         for j in 1..c repeat  ans(i,j) := - x(i,j)
       ans

     (a:R) * (x:%) : %  == 
       ans := new((r:=nRows x),(c:=nCols x),0)
       for i in 1..r repeat 
         for j in 1..c repeat  ans(i,j) := a*x(i,j)
       ans

     (x:%) * (a:R) : %  == 
       ans := new((r:=nRows x),(c:=nCols x),0)
       for i in 1..r repeat 
         for j in 1..c repeat  ans(i,j) := x(i,j)*a
       ans

--     m:Integer * x:% == map(m * #1,x)

     (x:%) * (y:%)  : % ==
       (ncx:=nCols x) ~= nRows y =>
         error "can't multiply matrices of incompatible dimensions"
       ans := new(nrx:=nRows x,ncy := nCols y,0)
       for i in 1..nrx repeat 
         for j in 1..ncy repeat 
           ans(i,j) :=
             sum : R := 0
             for k in 1..ncx  repeat
               sum := sum + x(i,k) * y(k,j)
             sum
       ans

     (x:%) * (v:Col) : Col  ==
       (nc:=nCols x) ~= #v =>
         error "can't multiply matrix A and vector v if #cols A ~= #v"
       w : Col := new(nr:=nRows x,0)
       for i in 1..nr repeat
         w.i :=
           sum : R := 0
           for j in 1..nc repeat 
             sum := sum + x(i,j)*v(j)
           sum
       w

  
     (v:Row) * (x:%) : Row ==
       (nr := nRows x) ~= #v =>
         error "can't multiply vector v and matrix A if #rows A ~= #v"
       w : Row := new(nc:= nCols x,0)
       for j in 1..nc repeat
         w.j :=
           sum : R := 0
           for i in 1..nr repeat 
             sum := sum + v(i)* x(i,j)
           sum
       w
          
     (x:%) = (y:%) : Boolean == 
       ((r := nRows x) ~= nRows y) or ((c := nCols x) ~= nCols y) => false
       for i in 1..r repeat 
         for j in 1..c repeat  
           if  x(i,j) ~= y(i,j) then return false
       true

     (p:TextWriter) << (x:%): TextWriter == p << rep x


MatMappings(R:Ring) : MP  == Definition where
   Mat ==> Matrix R

   MP ==>  with
    map: (R -> R,Mat) -> Mat
      ++ map(f,a) returns `b', where `b(i,j) = f(a(i,j))' for all `i, j'
    map!: (R -> R,Mat) -> Mat
      ++ map!(f,a)  assign `a(i,j)' to `f(a(i,j))' for all `i, j'
    map:((R,R) -> R,Mat,Mat) -> Mat
      ++ map(f,a,b) returns `c', where `c(i,j) = f(a(i,j),b(i,j))'
      ++ for all `i, j'
 
   Definition ==>  add

    import from SingleInteger

    map(f:R -> R ,m:Mat) :Mat  ==
      ans := new(nr:=nRows m,nc:=nCols m,0$R)
      for i in 1..nr repeat
        for j in 1..nc repeat ans(i,j) := f(m(i,j))
      ans
 
    map!(f:R -> R,m:Mat):Mat ==
      for i in 1..nRows m repeat 
        for j in 1..nCols m repeat m(i,j) := f(m(i,j))
      m
 
    map(f:(R,R) -> R,m:Mat,n:Mat) : Mat ==
     (nr:=nRows m) ~= nRows n or (nc:=nCols m) ~= nCols n =>
       error "map: arguments must have same dimensions"
     ans := new(nr,nc,0$R)
     for i in 1..nr repeat 
       for j in 1..nc repeat ans(i,j) := f(m(i,j),n(i,j))
     ans

