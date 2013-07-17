#include "runtimelib.as"


PtrCache: with {
	addEntry: (%, BasicTuple, Ptr) -> Ptr;
	getEntry: (%, BasicTuple) -> (Ptr, Boolean);
	newCache: () -> %;
	newCache: SingleInteger -> %;
} == add {
	-- how big to make the cache (if unspecified)
	cacheSize ==> 15;
	Rep ==> Record(posn: SingleInteger, 
		       keys: Array BasicTuple, 
		       values: Array Ptr,
		       size: SingleInteger);
	import from Rep, SingleInteger, Array BasicTuple;
	
	keys(x: %): Array BasicTuple == rep(x).keys;
	values(x: %): Array Ptr == rep(x).values;
	posn(x: %): SingleInteger == rep(x).posn;
	size(x: %): SingleInteger == rep(x).size;

	newCache(): % == per([0, 
			     new(cacheSize, Nil(BasicTuple)), 
			     new(cacheSize, Nil Ptr),
			     cacheSize]);

	-- He who calls with (x < 1) is eternally doomed.
	newCache(x:SingleInteger): % == per([0, 
			     new(x, Nil(BasicTuple)), 
			     new(x, Nil Ptr),
			     x]);

	addEntry(cache: %, key:  BasicTuple, value: Ptr): Ptr == {
		--(val, flag) := getEntry(cache, key);
		--flag => val;
		DEBUG(PRINT()<<NL()<<"Adding: "<<cache pretend SingleInteger
		             <<key<<NL());
		keys(cache).(posn(cache)+1) := key;
		values(cache).(posn(cache)+1) := value;
		rep(cache).posn := mod_+(posn(cache), 1, size(cache));
		value
	}

--	getEntry(cache: %, key: BasicTuple): (Ptr, Boolean) == {
--		for k in keys(cache)
--		for v in values(cache) repeat {
--			not Nil?(BasicTuple) k and k = key => return(v, true);
--		}
--		(nil(), false)
--	}

	getEntry(cache: %, key: BasicTuple): (Ptr, Boolean) == {
--		for i in 1..#keys(cache) repeat {
--		    k := keys(cache)(i);
--		    v := values(cache)(i);
--		    not Nil?(BasicTuple) k and k = key => return(v, true);
--		}
		(nil(), false)
	}
}
