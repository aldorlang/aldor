----------------------------- sit_zfring0.as -------------------------------
-- TO BE INCLUDED IN sit_zfring.as

	integerRoots(P:UnivariatePolynomialAlgebra0 %):P -> Generator RR == {
		(p:P):Generator(RR) +-> {
			import from UnivariateIntegralFactorizer(%, P);
			integerRoots p;
		}
	}

	rationalRoots(P:UnivariatePolynomialAlgebra0 %):P -> Generator RR == {
		(p:P):Generator(RR) +-> {
			import from UnivariateIntegralFactorizer(%, P);
			rationalRoots p;
		}
	}

	factor(P:UnivariatePolynomialAlgebra0 %):P -> (%, Product P) == {
		(p:P):(%, Product P) +-> {
			import from UnivariateIntegralFactorizer(%, P);
			factor p;
		}
	}
