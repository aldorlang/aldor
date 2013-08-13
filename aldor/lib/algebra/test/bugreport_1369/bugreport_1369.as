#include "algebra"
import from String, TextWriter, WriterManipulator;

macro {
    MI == MachineInteger;
    I == Integer;
    Vt == OrderedVariableTuple(-"X");
    Et == Order(Vt);
    Pt == DistributedMultivariatePolynomial1(I, Vt, Et);
}

-- a degree anticompatible ordering
Order(V: FiniteVariableType): MachineIntegerExponentVectorCategory(V) ==
MachineIntegerDegreeReverseLexicographicalExponent(V) add { 
    Rep == PrimitiveArray(MI);
    macro OLDORDER == MachineIntegerDegreeReverseLexicographicalExponent(V);
    import from OLDORDER;
    
    -- new order
    <(a: %, b: %): Boolean == {
	-- degree anti-compatibility
	tda: Integer := totalDegree(a);
	tdb: Integer := totalDegree(b);
	(tda > tdb) => return true;
	(tda < tdb) => return false;
	
	-- otherwise use old order
	return (a pretend OLDORDER) < (b pretend OLDORDER);
    }
}

-- imports
import from Symbol, I, Vt, Et, Pt;

-- bug demonstration
x: Pt := variable(machine(1))::Pt;
p1: Pt := 1 + x;
p2: Pt := 1;
stdout << "p1: " << p1 << endnl;
stdout << "p2: " << p2 << endnl;
stdout << "p1*p2: " << p1*p2 << endnl;
stdout << endnl;
