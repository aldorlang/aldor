#include "axllib"

-- NB: Do not add-inherit from this package

DebugPackage: with {
	debugActivate: ()->();
	debugStop:     () -> ();
	--debugSelect:   String -> ();
	--debugSelect:   (String, List String) -> ();
	--debugUnSelect: String -> ();
	--debugUnSelect: (String, List String) -> ();
} == add {
	import from DomainName;

	import {
		fiSetDebugVar: Pointer -> ();
	} from Foreign;

	-- from gf_rtime.
	DbgTypes ==> 'assign,entry,exit,dominit';
	
	-- local state
	doItAll   := true;
	activated := false;
	inDebug   := false;
	--
	-- activation
	--

	debugActivate(): () == {
		free doItAll, activated;
		-- only do this once.
		if (not activated) then 
			fiSetDebugVar(doDebug pretend Pointer);		
		activated := true;
		doItAll   := true;
	}
	debugStop(): () == {
		free doItAll := false;
	}

	-- 
	-- The real thing
	--
	doDebug(kind: DbgTypes, name: String, 
			line: SingleInteger, 
			T: Type, v: Pointer, v2: Pointer): () == {
		free  inDebug;
		inDebug		 => return;
		not doItAll      => return;
		inDebug := true;
		if kind = assign then doAssign(name, line, T, v pretend T, 
					       v2 pretend Boolean);
		if kind = entry  then
			 doEntry(name, line, T, v pretend Boolean);
		if kind = exit then 
			doExit(name, line, T, v pretend T, v2 pretend Boolean);
		inDebug := false;
		return;
	}

	-- f is true for parameter initialisations, 
	-- false for ordinary assignments.

	doAssign(name: String, line: SingleInteger, 
		 T: Type, v: T, f:Boolean): () == {
		print << "Line " << line << ": " << name << " : ";
		print << typeName T << " := ";
		printValue(T, v);
		print << newline;
	}

	-- v indicates if the exporter was valid
	-- T is the exporter, if v is true. 
	doEntry(name: String, line: SingleInteger, 
				T: Type, v: Boolean): () == {
		print << "{Line: " << line << " Entering: " << name;
		if v then print << " from: " << typeName T;
		print << newline;
	}

	-- f false indicates that T and v are not bone fide objects
	-- doExit1 is necessary as we don't know if T is a type at this point.
	doExit(name: String, line: SingleInteger, 
	       T: Type, v: T, f: Boolean): () == {
		not f => print << "Line: " << line << "}"<<newline;
		doExit1(name, line, T, v);
	}

	doExit1(name: String, line: SingleInteger, 
		            T: Type, v: T): () == {
		print << "Line: " << line << " Return: " ;
		print << typeName(T) << " is: ";
		printValue(T, v);
		print << "}" << newline;
		
	}

	-- finding an appropriate print routine
	printValue(T: Type, v: T): () == {
		T has (with { dbgPrint: ((TextWriter, %) ->TextWriter) })
			=> dbgPrint(print, v);
		T has BasicType => print << v;
		print << "??";
	}
		
	--
	-- type-matching predicates
	-- 

	debugType?(T: Type): Boolean == {
		return true;
	}

#if 0
	-- should be
	debugType(T: Type): Boolean == {
		name: DomainName := typeName(T);
		for constructorName in debuggedNames repeat {
			if leadingId(name) = name then return true;
		}
		false;
	}

	-- from langx
	ID ==> 0;
	APPLY ==> 1;
	OTHER ==> 2;

	-- Foo(X)(Y,Z)(A) --> "Foo"
	leadingId(nm: DomainName): String == {
		import from List DomainName;
		if type nm = ID then name nm
		else leadingId(first args nm);
	}

	-- more ideas: 
	-- print to fixed length string
	-- filter on function/variable name
	-- cache print functions, and other attributes.
	-- filter based on categories
#endif
}

