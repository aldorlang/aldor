#include "aldor"

extend Union(T: Tuple Type): with {
   OutputType;
}
== add {
    import from Machine;
    import from MachineInteger;
    Rep == Record(n: SInt, p: Pointer);
    import from String;
    import from Rep;

    (<<)(tw: TextWriter, v: %): TextWriter == {
        display(v: Pointer, t: Type, i: MachineInteger): () == {
	   import from TypeUtils;
	   import from DomainName;
	   import from t;
	   if t has OutputType then
	      tw << (v pretend t);
	   else
	      tw << "?";
	   tw << "@" << typeName(t);
	}

        type := element(T, rep(v).n::MachineInteger+1);
	tw << "[";
	display(rep(v).p, type, rep(v).n::MachineInteger + 1);
	tw << "]";
	tw;
    }

}
