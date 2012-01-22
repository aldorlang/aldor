Type: with == add;
Tuple(T: with): with == add;
(A: Tuple Type) -> (B: Tuple Type): with == add;
Boolean: with == add;
--import from Boolean;

XAlgebra(T: with): Category == with;
XIntegralDomain: Category == XAlgebra(%) with;
XLocalAlgebra(R: with, S: XAlgebra R): with == add;

F(A: with): XIntegralDomain with == add;
D: with == add;

local e: XLocalAlgebra(F D, F D);

--XA2(T: with): Category == with { foo: () -> A }
--A: XA2(A) with == add;


#if 0
--MyList(T: with): with == add {}
T: with == add {

   g(): MyList % == {
   	f();	
   }
   f(): MyList T == {
   	g();
   }

}
#endif
