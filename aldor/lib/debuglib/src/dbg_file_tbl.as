
#include "dbg_defs"


+++ TextFileTable is a domain used to hold the indices for each file
+++ known to the debugger, searchable by unparsed filenames.
TextFileTable:with
{
	new: () -> %;
		++ Creates a new table.

	apply: (%, String) -> TextFileIndex;
		++ `tbl.fname' returns the index associated with the
		++ file `fname'. If no suitable index can be found
		++ then nil is returned.

	set!: (%, String, TextFileIndex) -> TextFileIndex;
		++ `tbl.fname := idx' can be used to change the
		++ index associated with `fname'.
}
== add
{
	Rep == HashTable(String, TextFileIndex);
	import from Rep;


	new():% == per table();

	apply(x:%, f:String):TextFileIndex ==
	{
		import from Partial TextFileIndex;

		-- Search using `nil' as the result for failure.
		--(found, result) := search(rep x, f, nil());

		found := find(f, rep x);
		if failed? found then
			nil();
		else
			-- No need to check whether we found anything.
			result := retract found;
	}


	set!(x:%, f:String, i:TextFileIndex):TextFileIndex ==
		(rep x).f := i;
}

