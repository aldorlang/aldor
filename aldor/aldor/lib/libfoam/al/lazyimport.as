#include "runtimelib.as"


LazyImport: with { 
	makeLazyImport: (Domain, Hash, Hash) -> %;
	force: %->Value;
} == add {
	--Need set!$Union, or for closures to work better under -Wruntime
	Rep ==> Record(got: Boolean,
		       dom: Domain, 
		       name: Hash, 
		       type: Hash, 
		       value: Value);
	import from Rep;
	makeLazyImport(dom: Domain, n: Hash, t: Hash): % == 
		per [false, dom, n, t, Nil Value];

	force(lv: %): Value == {
		if not rep(lv).got then {
		        box: Box := new Nil Value;
			val := getExport!(rep(lv).dom, rep(lv).name, rep(lv).type, box);
			rep(lv).value := val;
			rep(lv).got := true;
			rep(lv).dom := Nil Domain;
		}
		return rep(lv).value;
	}
}		
