--------------------------------------------------------------------------------
--
-- sm_dmp0.as: A basic domain for distributed polynomials 
--             with commuting monomials.
--
--------------------------------------------------------------------------------
--  Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Copyright: INRIA, UWO and University of Lille I, 2001
-- Copyright: Marc Moreno Maza
--------------------------------------------------------------------------------

-- dom DistributedMultivariatePolynomial0

#include "algebra"

DistributedMultivariatePolynomial0(R: Join(ArithmeticType, ExpressionType),
   E: GeneralExponentCategory): Join(CopyableType, IndexedFreeModule(R, E)) with {

} == add {
 
	Term == Record(co: R, ex: E);
	Rep  == List Term;

        -------------------------------------------
        --%  The representation is sparse and   %--
        --%   decreasing wrt exponent's order   %--
        -------------------------------------------

        import from R, E, Term, Rep;

        local integralDomain?: Boolean        == R has IntegralDomain;
        local ordered?: Boolean               == R has OrderedArithmeticType;

        -----------------------------------------
        --% Local functions dealing with Term %--
        -----------------------------------------

        local printTerm(p: TextWriter, t: Term): () == {
		(co, ex) := explode t;
		zero? ex => p << co;
                import from String;
		if co ~= 1 then p << co << "*";
                p << ex;
        }
        local - (t: Term): Term == {
               [-t.co, t.ex];
        }
        local neg!(t: Term): Term == {
              t.co := -t.co;
              t;
        }
        local equalTerm(t1: Term, t2: Term): Boolean == {
               (t1.ex = t2.ex) and (t1.co = t2.co);
        }
        local extreeTerm(t: Term): ExpressionTree == {
              (co, ex) := explode t;
              zero? ex => extree(co);
              co = 1 => extree(ex);
              le: List ExpressionTree := [extree(co), extree(ex)];
              ExpressionTreeTimes le;
        }
        local constructTerm(cross: Cross(R,E)): Term == {
              (c: R, e: E) := cross;
              [c, e];
        }
        local copyTerm(t: Term): Term == {
             [explode t];
        }

        ----------------------------------------
        --% Local functions dealing with Rep %--
        ----------------------------------------

        local mapMinus(xx: Rep): Rep == {
              map(-)(xx);
        }
        local mapMinus!(xx: Rep): Rep == {
              map(neg!)(xx);
        }
        local mapCopyTerm(xx: Rep): Rep == {
              map(copyTerm)(xx);
        }

        -----------------------------------------
        --% Exported functions as a ExpressionType %--
        -----------------------------------------

#if USEMYOWNPRINT
	if R has OrderedArithmeticType then {
	(p: TextWriter) << (x: %): TextWriter == {
                import from String;
		xx: Rep := rep x;
                empty? xx => p << 0$R;
                if ((first(xx)).co = -1) then {
                        p << " -";
                        printTerm(p, -(first xx));
                }
                else {
                        printTerm(p, first xx);
                }
                xx := rest xx;
		while not empty? xx repeat {
			tx := first xx;
			if tx.co >= 0 then {
				p << " + ";
				printTerm(p, tx);
			}
			else {
				p << " - ";
				printTerm(p, -tx);
			}
			xx := rest xx;
		}
		p;
	}
	} else {
	(p: TextWriter) << (x: %): TextWriter == {
                import from String;
		xx: Rep := rep x;
                empty? xx => p << 0$R;
                if ((first(xx)).co = -1) then {
                        p << " -";
                        printTerm(p, -(first xx));
                }
                else {
                        printTerm(p, first xx);
                }
                xx := rest xx;
		while not empty? xx repeat {
			p << " + ";
			printTerm(p, first xx);
			xx := rest xx;
		}
		p;
	}
	}
#endif
        (x: %) = (y: %) : Boolean == { 
            xx: Rep := rep(x);
            yy: Rep := rep(y);
            while (not empty? xx) and (not empty? yy) repeat {
                  not equalTerm(first xx, first yy) => return false;
                  xx := rest xx; yy := rest yy;
            }
            (empty? xx) and (empty? yy);
        }
        extree(x: %): ExpressionTree == {
            xx: Rep := rep(x);
            le: List ExpressionTree := [
              extreeTerm(t) for t in xx
            ];
		empty? le => extree(0$R);
		empty? rest le => first le;
		ExpressionTreePlus le;
        }

        --------------------------------------------
        --% Exported functions as a CopyableType %--
        --------------------------------------------

        copy(x: %): % == per mapCopyTerm(rep x);

        ---------------------------------------------
        --% Exported functions as an AdditiveType %--
        ---------------------------------------------

	0: % == per empty;

	(x: %) + (y: %) : %  == {
		xx: Rep := rep x; 
		yy: Rep := rep y;
		res: DoubleEndedList Term := empty();
		while (not empty? xx) and (not empty? yy) repeat {
			tx := first xx;
			ty := first yy;
			if tx.ex > ty.ex then {
				concat!(res,tx);
				xx := rest xx;
			}
			else if ty.ex > tx.ex then {
				concat!(res,ty);
				yy := rest yy;
			}
			else {  
				r:R := tx.co + ty.co;
				if not zero? r then {
					concat!(res, [r, tx.ex]);
				}
				xx := rest xx;
				yy := rest yy;
			}
		}
		tt: Rep := if (not empty? xx)  then xx else yy;
		empty? firstCell(res) => per tt;
                setRest!(lastCell(res),tt);
		per firstCell(res);
	}
        - (x: %) : % == per mapMinus(rep x);

	add!(x: %, y: %): % == {
                zero? x => copy y;
		local res, last, newend: Rep;
		xx: Rep := rep x;
		yy: Rep := rep y;
		res := empty;
		while (not empty? xx) and (not empty? yy) repeat {
			tx := first xx;
			ty := first yy;
			if tx.ex > ty.ex then {
				newend := xx;
				xx := rest xx;
			}
			else if ty.ex > tx.ex then {
				newend := [copyTerm ty];
				yy := rest yy;
			}
			else {
				r: R := tx.co + ty.co;
				yy := rest yy;
				if zero? r then {
					xx := rest xx;
					iterate;
				}
				else {
					tx.co := r;
					newend := xx;
					xx:= rest xx;
				}
			}
			if empty? res then {
				res := newend;
				last := res;
			}
			else {
                                setRest!(last,newend);
				last := newend;
			}
		}
		newend := if not empty? xx then xx else mapCopyTerm yy;
		empty? res => per newend;
		setRest!(last,newend);
		per res;
	}
	(x: %) - (y: %) : %  == {
		xx: Rep := rep x; 
		yy: Rep := rep y;
		res :DoubleEndedList Term := empty();
		while (not empty? xx) and (not empty? yy) repeat {
			tx := first xx;
			ty := first yy;
			if tx.ex > ty.ex then {
				concat!(res,tx);
				xx := rest xx;
			}
			else if ty.ex > tx.ex then {
				concat!(res,-ty);
				yy := rest yy;
			}
			else {  
				r: R := tx.co - ty.co;
				if not zero? r then {
					concat!(res, [r,tx.ex]);
				}
				xx := rest xx;
				yy := rest yy;
			}
		}
		tt: Rep := if (not empty? xx) then xx else mapMinus(yy);
		empty? firstCell(res) => per tt;
		setRest!(lastCell(res),tt);
		per firstCell(res);
	}
	minus!(x: %, y: %): % == {
                zero? x => -y;
		local res, last, newend: Rep;
		xx: Rep := rep x;
		yy: Rep := rep y;
		res := empty;
		while (not empty? xx) and (not empty? yy) repeat {
			tx := first xx;
			ty := first yy;
			if tx.ex > ty.ex then {
				newend := xx;
				xx := rest xx;
			}
			else if ty.ex > tx.ex then {
				newend := [-ty];
				yy := rest yy;
			}
			else {
				r: R := tx.co - ty.co;
				yy := rest yy;
				if zero? r then {
					xx := rest xx;
					iterate;
				}
				else {
					tx.co := r;
					newend := xx;
					xx:= rest xx;
				}
			}
			if empty? res then {
				res := newend;
				last := res;
			}
			else {
				setRest!(last,newend);
				last := newend;
			}
		}
		newend := if not empty? xx then xx else mapMinus(yy);
		empty? res => per newend;
		setRest!(last,newend);
		per res;
	}

        minus!(x: %): % == {
                zero? x => x;
                per mapMinus!(rep x);
        }
        zero? (x: %) : Boolean == empty? rep x;

        -----------------------------------------------------
        --% Exported functions as a LinearCombinationType %--
        -----------------------------------------------------

	(r: R) * (x: %) : % == {
		zero? r => 0;
		if integralDomain? then {
		        times(tx: Term): Term == [r*tx.co, tx.ex];
                        per (map(times)(rep x));
		}
		else {
			per [[s, tx.ex] for tx in rep x | 
				not zero? (s := r * tx.co) ];
		}
	}
	times!(r: R, x: %): % == {
		assert(not(zero? r));
		if integralDomain? then {
			times!(tx:Term):Term == {tx.co:= r*tx.co;tx};
			per (map(times!)(rep x));
		}
		else {
			local res, last, newend : Rep;
			xx := rep x;
			res := empty;
			while not empty? xx repeat {
				tx := first xx;
				tx.co := r * tx.co;
				if zero? tx.co then {
					xx := rest xx;
					iterate;
				}
				else {
					newend := xx;
					xx := rest xx;
				}
				if empty? res then {
					res := newend;
					last := res;
				}
				else {
                                        setRest!(last,newend);
					last := newend;
				}
			}
			per res;
		}
	}

        --------------------------------------------------
        --% Exported functions as an IndexedFreeModule %--
        --------------------------------------------------

	degree(x: %): E == {
		xx := rep x;
		if empty? xx then 0@E else (first(xx)).ex;
	}
        trailingDegree(x: %): E == {
		xx:= rep x;
                empty? xx => 0@E;
                while (not empty? rest xx) repeat xx := rest xx;
                (first xx).ex;
        } 
	generator(x: %): Generator Cross(R, E) == generate {
		for tx in rep x repeat yield (tx.co, tx.ex);    -- decreasing
	}
	terms(x: %): Generator Cross(R, E) == generate {
                xx: Rep := reverse(rep x);
		for tx in xx repeat yield (tx.co, tx.ex);        -- increasing
	}
        leadingTerm(x: %): (R, E) == {
                empty? rep x => (0@R,0@E);
		tx: Term := first rep x;
                (tx.co,tx.ex);
        }
        trailingTerm(x: %): (R, E) == {
 		xx:= rep x;
                empty? xx => (0@R, 0@E);
                while (not empty? rest xx) repeat xx := rest xx;
                ((first xx).co,(first xx).ex);
        } 
	leadingMonomial(x:%):% == {
		xx := rep x;
		if empty? xx then per empty else per [[1,(first(xx)).ex]];
	}
	trailingMonomial(x:%):% == {
		xx:= rep x;
                empty? xx => per empty;
                while (not empty? rest xx) repeat xx := rest xx;
                per [[1,(first(xx)).ex]];
	}
        if (R has HashType and E has HashType) then {
           import from R pretend HashType;
           import from E pretend HashType;
           hash(x: %): MachineInteger == {
               h: MachineInteger := 0;
               xx: Rep := rep x;
               i: MachineInteger := 1;
               for tx in xx repeat {
                   h := h + hash(tx.co) * hash(tx.ex) * i;
                   i := next(i);
               }
               h;
           }
        }
        if (R has SerializableType and E has SerializableType) then {
           import from R pretend SerializableType;
           import from E pretend SerializableType;
                        (port:BinaryWriter) << (p:%):BinaryWriter == {
                                -- g: Generator Cross(R, E) := terms(p); SHOULD BE BETTER ??
                                g: Generator Cross(R, E) := generator(p);
                                for tx in g repeat {
                                        (c: R, e: E) := tx;
                                        port := port << c << e;
                                }
                                port << 0@R << 0@E;
                        }
                        << (port:BinaryReader):% == {
                                p:% := 0;
                                local e:E;
                                local r:R;
                                repeat {
                                   r := << port;
                                   e := << port;
                                   zero? r => break;
                                   p := add!(p, r, e);
                                }
                                p;
                        }
        }

        --------------------------------------------------
        --% Exported functions as an IndexedFreeModule %--
        --------------------------------------------------

        add!(x: %, r: R, e: E): % == add!(x, term(r,e));
        coefficient(x: %, e: E): R == {
                for tx in rep x repeat {
                        e = tx.ex => return tx.co;
                        e > tx.ex => return 0;
                }
                0;
        }
        setCoefficient!(x: %, e: E, r: R): % == {
          zero? x => {
            zero? r => x;
            per [[r,e]$Term];
          }
          local s: R; local d: E;
          l: Rep := rep x;
          t: Term := first(l);
          (s, d) := explode(t);
          e > d => {
             zero? r => x;
             per cons([r,e]$Term,l);
          }
          e = d => {
             zero? r => per rest(l);
             per cons([r,e]$Term,rest(l));
          }
          empty? rest(l) => {
             zero? r => x;
             setRest!(l,[[r,e]$Term]);
             per l;
          }
	  local k: Rep;     
          repeat {
            k := l;
            l := rest l;
            t := first(l);
            t.ex <= e => break;
            empty? rest(l) => break;
          }
          t.ex = e => {
            zero? r => {
              setRest!(k,rest(l));
              return x;
            }
            t.co := r;
            return x;
          }
          t.ex < e => {
            zero? r => return x;
            setRest!(k,cons([r,e]$Term,l));
            return x;
          }
          zero? r => x;
          setRest!(l,[[r,e]$Term]);
          x;
        }
        monomial(e: E): % == per [[1, e]];

	term(r: R, e: E): % == {
		zero? r => 0;
		per [[r, e]];
	}


        -------------------------------------------
        --% Exported functions as a  FreeModule %--
        -------------------------------------------


        ground?(x: %): Boolean == {
		empty? rep x => true;
		tx: Term := first rep x;
		(zero? tx.ex) and (empty? rest rep x);
        }
        term?(x: %): Boolean == {
                -- term?(0) returns truw. see FreeModule.
                (empty? rep x) or (empty? rest rep x);
        }
        leadingCoefficient(x: %): R == {
                -- leadingCoefficient(0) returns 0
		empty? rep x => 0@R;
		tx: Term := first rep x;
		tx.co;
        }
        trailingCoefficient(x: %): R == {
                -- trailingCoefficient(0) returns 0
 		xx:= rep x;
                empty? xx => 0@R;
                while (not empty? rest xx) repeat xx := rest xx;
                (first xx).co;
        }              
        reductum(x: %): % == { zero? x => 0 ; per rest rep x;}

        support(x: %): Generator(Cross(R, %)) == generate {
           -- every pair returned has a non-zero coefficient
           xx := rep x;
           while not empty? xx repeat {
               t := first xx;
               xx := rest xx;
               yield(t.co,term(1,t.ex));
           }
        }
}


#if ALDOC
\thistype{DistributedMultivariatePolynomial0}
\History{Marc Moreno Maza}{08/07/01}{created}
\Usage{import from \this~(R,E)}
\Params{
{\em R} & \astype{ArithmeticType} & The coefficient domain \\
        & \astype{ExpressionType} \\
{\em E} & \altype{GeneralExponentCategory} V & The exponent domain \\
}
\Descr{\this~(R,E) provides an implementation of the free module over $R$ with basis $E$.
Roughly speaking, the elements of \this~(R,E) are polynomials that can be multiplied
only by a constant from $R$.
Each of these {\em polynomials} $x$ is coded
as a list of term $(r,e)$ with $r \in R, r\neq 0$ and $e \in E$
such that $x$ is the sum of these terms.}
\begin{exports}
\category{\astype{CopyableType}} \\
\category{\astype{IndexedFreeModule}(R,E)} \\
\end{exports}
#endif

#if ALDORTEST

---------------------- test dmp0.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	B == Boolean;
        Z == Integer;
}
main():() == {
   import from String, Symbol, MachineInteger, Integer, TextWriter;

   R1 == Integer;
   R2 == SmallPrimeField(41);
   V == OrderedVariableTuple(-"x",-"y",-"z");
   x: V := variable(1)$V;
   y: V := variable(2)$V;
   z: V := variable(3)$V;
   lv: List(V) == [x,y,z];
   E1 == MachineIntegerDegreeLexicographicalExponent(V);
   E2 == MachineIntegerDegreeReverseLexicographicalExponent(V);
   E3 == MachineIntegerLexicographicalExponent(V);
   import from R1, R2, E1, E2, E3;

   P11 == DistributedMultivariatePolynomial0(R1,E1);
   P12 == DistributedMultivariatePolynomial0(R1,E2);
   P13 == DistributedMultivariatePolynomial0(R1,E3);
   P21 == DistributedMultivariatePolynomial0(R2,E1);

   lr1: List(R1) == [-1,-3,6,12,-18,21,24,111,-789,103];
   n1: Z := (#lr1) :: Z; n1 := n1 * n1 * n1;
   lr2: List(R2) == [r1 :: R2 for r1 in lr1];
   n2: Z := (#lr2) :: Z; n2 := n2 * n2 * n2;
   le1: List(E1) == [exponent(0,0,0), exponent(1,0,0), exponent(0,1,0), exponent(0,0,1), exponent(1,2,3),  exponent(3,2,1), exponent(2,3,1), exponent(1,2,0), exponent(2,1,0), exponent(1,1,1)];
   le2: List(E2) == [exponent(0,0,0), exponent(1,0,0), exponent(0,1,0), exponent(0,0,1), exponent(1,2,3),  exponent(3,2,1), exponent(2,3,1), exponent(1,2,0), exponent(2,1,0), exponent(1,1,1)];
   le3: List(E3) == [exponent(0,0,0), exponent(1,0,0), exponent(0,1,0), exponent(0,0,1), exponent(1,2,3),  exponent(3,2,1), exponent(2,3,1), exponent(1,2,0), exponent(2,1,0), exponent(1,1,1)];
   

   stdout  << "Testing  DistributedMultivariatePolynomial0 ..." << endnl;

   pack11 == DistributedMultivariatePolynomial0TestPackage(R1,E1,P11);
   stdout << newline << "With MachineIntegerDegreeLexicographicalExponent ... ";
   lp11: List(P11) == makeZoo(lr1,le1,n1)$pack11;
   errors: Z := DMPTest(lp11,false,false)$pack11;
   if zero? errors  then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
   pack12 == DistributedMultivariatePolynomial0TestPackage(R1,E2,P12);
   stdout << newline << "With MachineIntegerDegreeReverseLexicographicalExponent ... ";
   lp12: List(P12) == makeZoo(lr1,le2,n1)$pack12;
   errors := DMPTest(lp12,false,false)$pack12;
   if zero? errors  then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
   pack13 == DistributedMultivariatePolynomial0TestPackage(R1,E3,P13);
   stdout << newline << "With MachineIntegerLexicographicalExponent ... ";
   lp13: List(P13) == makeZoo(lr1,le3,n1)$pack13;
   errors := DMPTest(lp13,false,false)$pack13;
   if zero? errors  then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
   pack21 == DistributedMultivariatePolynomial0TestPackage(R2,E1,P21);
   stdout << newline << "With MachineIntegerDegreeLexicographicalExponent ... ";
   lp21: List(P21) == makeZoo(lr2,le1,n2)$pack21;
   errors: Z := DMPTest(lp21,false,false)$pack21;
   if zero? errors  then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
--   import from P21;
--   P31 == DistributedMultivariatePolynomial0(P21 pretend Join(ArithmeticType, ExpressionType),E1);
--   lr3: List(P21) := makeZoo(lr2,le1,10)$pack21;
--   import from P31;
--   pack31 == DistributedMultivariatePolynomial0TestPackage(P21 pretend Join(ArithmeticType, ExpressionType),E1,P31);
--   stdout << newline << "With MachineIntegerDegreeLexicographicalExponent ... ";
--   lp31: List(P31) == makeZoo(lr3,le1,100)$pack31;
--   errors: Z := DMPTest(lp31,true,true)$pack31;
--   if zero? errors  then {
--      stdout << " OK " << newline;
--   } else {
--      stdout << " ERROR " << newline;
--   }
}


main();

stdout << endnl;
#endif
