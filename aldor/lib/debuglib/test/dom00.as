
#include "aldor"
#include "debuglib"

Z==>Integer;
STR==>String; 

DebugDom: with {
	new: (STR,Z) -> %;

	getStr: % -> STR;
	getInt: % -> Z;
	setStr: (%, STR) -> STR;  
	setInt: (%, Z) -> Z;
} == add {
	Rep == Record(s: STR, i:Z);
	import from Rep;

	new(s:STR, i:Z): % == per[s, i];
	getStr(v:%): STR == rep(v).s;
	getInt(v:%): Z == rep(v).i;
	setStr(v:%, n: STR): STR == { rep(v).s := n;}  
	setInt(v:%, d:Z): Z == {rep(v).i := d;}
}
