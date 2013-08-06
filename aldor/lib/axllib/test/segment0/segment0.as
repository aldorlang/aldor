-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp -Q0

#library AxlLib "axllib"
import from AxlLib;

Foo(S: Steppable): Category == with {
        kzip: (S,S) -> ClosedSegment S;
}

extend Segment(S: Steppable): with {
	if S has Finite then Foo S;
}
== add {
        if S has Finite then {
                kzip(x: S, y: S): ClosedSegment S == {
                        import from SingleInteger;
                        (S pretend SingleInteger)+1;
                        x..y
                }
        }
}
