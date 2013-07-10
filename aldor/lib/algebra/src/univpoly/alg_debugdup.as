#include "algebra"
#include "algebrauid"


DebuggingDenseUnivariatePolynomial(R:Join(ArithmeticType, ExpressionType),
	avar:Symbol == -"X"): UnivariatePolynomialAlgebra R with { } 
== DenseUnivariatePolynomial(R,avar) add { 

        (p: TextWriter) << (x: %): TextWriter == {
		priterm(co: R, ex:SI): () == {
			numberSystem? => {
			    zero? ex => p << co;
			    if co ~= 1 then p << co << "*";
			    one?  ex => p << s;
			    p << s << "^" << ex;
			}
			zero? ex => p << co;
			if co ~= 1 then p << "(" << co << ")*";
			one?  ex => p << s;
			p << s << "^" << ex;
		}
                zero? x => p << zero__R;
                lc__x := coef(x).(nterms(x));
                if one?(- lc__x) and characteristicZero? then { 
			print << "- ";
			priterm(1@R,degr(x));
		}
		else {
			priterm(lc__x, degr(x));
		}
		for i in degr(x)..1 by -1 repeat {
			(a:R, b:SI):= (coef(x).i, i-1);
			if zero? a then iterate;
			if orderedAbelianGroup? then {
				import from (R pretend OrderedAbelianGroup);
				if a < 0 then {
					p << " - ";
					priterm(-a,b);
				}
				else {
					p << " + ";
					priterm(a,b);
				}
			}
			else {
				p << " + ";
				priterm(a,b);
			}
		}
		p;
	}
}
