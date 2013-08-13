#include "algebra"
macro {
	MI == MachineInteger;
	Z == Integer;
}
import from Symbol,TextWriter,String,Character;

primeTestMW():() == {
    p:Z := 18446744073692774401;
    K == PrimeField(p);    
    import from K;
    U == DenseUnivariatePolynomial(K,-"x");
    import from U;
    x==monom;
    k:Z := 290;
    ak:K := k::K;
    au:U := x-k::K::U;
    stdout <<"[p > MachineWord case] k-> "<<k<<" pol x - k::K::U -> "<<au<<newline;
}
     
primeTestNMW():() == {
    p:Z := 257;
    K == PrimeField(p);
    import from K,MI;    
    U == DenseUnivariatePolynomial(K,-"x");
    import from U;
    x==monom;
    k:Z := 290;
    ak:K := k::K;
    au:U := x-k::K::U;
    stdout <<" [p < MachineWord Case] k-> "<<k<<" pol x-k::K::U -> "<<au<<newline;
}

primeTestMW();
primeTestNMW();

