--------------------------- sit_gcdint0.as -------------------------------------
-- TO BE INCLUDED INT sit_gcdint.as

gcdquoUP(P:UnivariatePolynomialAlgebra0 %):(P,P)->(P,P,P) == {
	(p:P, q:P):(P, P, P) +-> {
		import from Partial P;
		import from HeuristicGcd(%, P), ModularUnivariateGcd(%, P);
		(g, y, z) := modularGcd(p, q);
#if REPORTFAILURE
		if failed? g then reportFailure(P, p, q);
#endif
		if failed? g then (g, y, z) := heuristicGcd(p, q);
		failed? g => {
			import from Resultant(%, P);
			srg := subResultantGcd(p, q);
			(srg, quotient(p, srg), quotient(q, srg));
		}
		(retract g, y, z);
	}
}

gcdUP(P:UnivariatePolynomialAlgebra0 %):(P,P)->P == {
	(p:P, q:P):P +-> {
		-- TEMPORARY: COMPILER SEES 2 MEANINGS FOR gcdquoUP !
		(g, y, z) := gcdquoUP(P)(p, q);
		g;
#if LATER
		import from Partial P;
		import from HeuristicGcd(%, P), ModularUnivariateGcd(%, P);
		(g, y, z) := modularGcd(p, q);
-- #if REPORTFAILURE
		if failed? g then reportFailure(P, p, q);
-- #endif
		if failed? g then (g, y, z) := heuristicGcd(p, q);
		failed? g => {
			import from Resultant(%, P);
			subResultantGcd(p, q);
		}
		retract g;
#endif
	}
}

#if REPORTFAILURE
reportFailure(P:UnivariatePolynomialAlgebra0 %, p:P, q:P):() == {
	import from TextWriter, File, String, ExpressionTree;
	tmp := uniqueName "modgcd";
	ftmp := open(tmp, fileWrite);
	stderr <<"ModularGCD failure reported to sumit@sophia.inria.fr";
	stderr << endnl;
	wtmp := ftmp::TextWriter;
	wtmp << "ModularGCD failed on:" << newline;
	maple(wtmp, extree p);
	wtmp << newline << "and" << newline;
	maple(wtmp, extree q);
	wtmp << newline;
	close! ftmp;
	run concat("mail sumit@sophia.inria.fr < ", ntmp);
	run concat("rm -f ", ntmp);
}
#endif

