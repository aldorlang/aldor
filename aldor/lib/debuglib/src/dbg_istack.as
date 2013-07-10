
#include "dbg_defs"


SI  ==> SingleInteger;
PTR ==> Pointer;

+++ IntStack is a domain that records information about current
+++ lexical enviroments of 'prog' call in FOAM code.
IntStack:SetCategory with
{
	nothing: () -> %;
		++ The empty stack.

	stack: (String, String, Array PTR, Array Pointer, Pointer) -> %;
		++ stack(u, p, l, p, e) creates a stack associated
		++ with the prog p in unit u.
  
	depth: % -> SingleInteger;
		++ depth(s) returns the stack depth associated with the
		++ stack s.

	apply: (%, 'unit') -> String;
		++ `s.unit' returns the unit name associated with the
		++ current execution position in s.

	apply: (%, 'prog') -> String;
		++ `s.prog' returns the prog name associated with the
		++ current execution position in s.

	apply: (%, 'locals') -> Array PTR;
		++ `s.locals' returns the pointer of locals in this prog.

	apply: (%, 'params') -> Array PTR;
		++ `s.params' returns the pointer of parameters in this prog.

	apply: (%, 'env') -> PTR;
		++ `s.env' returns the pointer of env in this prog.

	apply: (%, 'depth') -> SI;

	apply: (%, 'numl') -> SI;

	set!: (%, 'unit', String) -> String;
		++ `s.unit := myUnit' is used to update the unit
		++ associated with the current execution point in
		++ the stack s.

	set!: (%, 'prog', String) -> String;
		++ `c.prog := myProg' is used to update the prog
		++ associated with the current execution point in
		++ the stack s.
  
        set!: (%, 'env', PTR) -> PTR;
                ++ 's.env := myEnv' is used to update the environment
                ++ associated with current stack.

	set!: (%, 'depth', SI) -> SI;
 
        setLocals!: (%, PTR) -> Array PTR;
                ++ '..' is used to update the locals
                ++ associated with current stack.
  
        setParams!: (%, PTR) -> Array PTR;
                ++ '..' is used to update the parameters
                ++ associated with current stack.

	nothing? : % -> Boolean;
		++ Returns true if this is the empty stack.

	export from 'unit', 'prog', 'env', 'depth', 'numl', 'locals', 'params';
}
== add
{
	Rep == Record
	(
		unit:String,
		prog:String,
		numl:SingleInteger,
                locals:Array PTR,
		nump:SingleInteger,
		params:Array PTR,
                env:PTR,
		depth:SingleInteger;
	);

	import from Rep, 'unit', 'prog', 'locals', 'params', 'env', 'depth';

        local address(s:%):Pointer              == s pretend Pointer;
                                                                                                                              
        hash(s:%): MachineInteger  == { import from Pointer; address(s)::MachineInteger }

	sample:% == nothing();

	(a:%) = (b:%):Boolean ==
	{
		-- Use pointer equality: later if we store
		-- type hash codes then we could check the
		-- name, type and depth.
		import from Pointer;
		(a pretend Pointer) = (b pretend Pointer);
	}

	nothing?(x:%):Boolean == nil?(x pretend Pointer)$Pointer;
	nothing():% == (nil$Pointer) pretend %;
	unit(x:%):String == (rep x).unit;
	prog(x:%):String == (rep x).prog;
	depth(x:%):SingleInteger == (rep x).depth;

	stack(u:String, prog:String, l:Array PTR, prms:Array PTR, e:PTR):% ==
		per [u, prog, 0, l, 0, prms, e, 0];

	apply(x:%, dummy:'unit'):String == (rep x).unit;
	apply(x:%, dummy:'prog'):String == (rep x).prog;

	apply(x:%, dummy:'locals'):Array PTR == (rep x).locals;
	apply(x:%, dummy:'params'):Array PTR == (rep x).params;	
        apply(x:%, dummy:'env'):PTR == (rep x).env;
	apply(x:%, dummy:'depth'):SI == (rep x).depth;
	apply(x:%, dummy:'numl'):SI == (rep x).numl;

	set!(x:%, dummy:'unit', u:String):String ==
		(rep x).unit := u;

	set!(x:%, dummy:'prog', p:String):String ==
		(rep x).prog := p;

	set!(x:%, dummy:'depth', d:SI):SI == 
		(rep x).depth := d;

	setLocals!(x:%, l:PTR):Array PTR ==
	{
		import from Array PTR;
		local alocals: Array PTR := (rep x).locals;

		if ((rep x).numl = 0) then
		{
			alocals := new(1, l);
		}
		else
		{
			extend!(alocals, l);
		
		}

		(rep x).numl := (rep x).numl + 1;
		(rep x).locals := alocals;
	}

	setParams!(x:%, p:PTR):Array PTR ==
	{
		import from Array PTR;
		local aparams: Array PTR := (rep x).params;

		if ((rep x).nump = 0) then
		{
			aparams := new(1, p);
		}
		else
		{
			extend!(aparams, p);
		
		}

		(rep x).nump := (rep x).nump + 1;
		(rep x).params := aparams;
	}

	set!(x:%, dummy:'env', e:PTR):PTR ==
		(rep x).env := e;

	(tw:TextWriter) << (stack:%):TextWriter ==
	{
		import from Parameter, DomainName;

		-- Display the current position.
		tw << " [_"";
		tw << (stack.unit);
		tw << "_" at prog ";
		tw << (stack.prog);
		tw << "]";


		-- Return the modified textwriter.
		tw;
	}
}
