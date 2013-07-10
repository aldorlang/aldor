
#include "dbg_defs"


SI ==> SingleInteger;


+++ Identifier is the category of variables, constants, parameters etc.
define Identifier:Category == SetCategory with
{
	new: (String, Type, SI) -> %;
		++ Creation.

	nil: () -> %;
		++ Special variable: only nil? can be applied to it.

	nil?: % -> Boolean;
		++ True if the variable is nil.

	name: % -> String;
		++ Name of the identifier.

	type: % -> Type;
		++ Type of the identifier.

	number: % -> SI;
		++ ID number of the identifier.

	value: (%, T:Type) -> T;
		++ Value viewed as a member of type T.

	update: (%, T:Type, v:T) -> %;
		++ Return a NEW value representing the updated identifier.
		++ The identifier passed as the first argument is not
		++ changed in any way.
}


+++ Parameter is a domain used to represent function parameters.
Parameter:Identifier with
{
}
== add
{
	Rep == Record(nm:String, Typ:Type, val:Typ, no:SI);
	import from Rep, SingleInteger;

        --local address(s:%):Pointer              == s pretend Pointer;
                                                                                                                              
        hash(s:%): MachineInteger  == { import from String; hash ((rep s).nm) * ((rep s).no)}

	sample:% == nil();
	(a:%) = (b:%):Boolean ==
	{
		-- Deal with the special values.
		(nil? a) => nil? b;
		(nil? b) => false;


		-- Check names and numbers.
		not((name a) = (name b)) => false;
		not((number a) = (number b)) => false;


		-- Pointer equality: note that the pointer will change
		-- as the result of an update call.
		import from Pointer;
		((rep a) pretend Pointer) = ((rep b) pretend Pointer);
	}

	(tw:TextWriter) << (x:%):TextWriter ==
	{
		-- name:Type == value
		import from DomainName, TypePackage;

		-- Do we have a variable?
		(nil? x) => tw << "???:??? == ???";


		-- Do we have a known type?
		zero?((type x) pretend SI) =>
		{
			-- Display something.
			tw << (name x) << ":??? == ";
			tw << "{" << value(x, SI) << "}";
		}


		-- Looking good: display the name and type.
		tw << (name x) << ":" << typeName(type x);
		tw << " == ";


		-- Monkey business to get around limitations of
		-- the compiler with types as values.
		foo(tw, type x, value(x, type x)) where
		{
			foo(tw:TextWriter, T:Type, t:T):() ==
			{
				if (T has BasicType) then
					tw << t;
				else
					tw << "{" << (t pretend SI) << "}";
			}
		}


		-- Return the modified textwriter.
		tw;
	}

	-- Local constructor.
	local new(x:String, T:Type, v:T, n:SingleInteger):% ==
		per [x, T, v, n];

	new(x:String, T:Type, n:SingleInteger):% ==
		new(x, T, 0 pretend T, n);
	nil():% == 0 pretend %;
	nil?(x:%):Boolean == zero?(x pretend SingleInteger);
	name(x:%):String == (rep x).nm;
	type(x:%):Type == (rep x).Typ;
	number(x:%):SingleInteger == (rep x).no;
	value(x:%, T:Type):T == (rep x).val pretend T;

	update(x:%, T:Type, v:T):% ==
	{
		-- Strictly speaking we ought to ignore T and
		-- use where/pretend to get v:type(x).
		new(name x, T, v, number x);
	}
}

