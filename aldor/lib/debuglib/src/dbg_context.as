
#include "dbg_defs"


SI  ==> SingleInteger;


+++ CallContext is a domain that records information about specific
+++ function call instances and the variables/constants declared.
CallContext:SetCategory with
{
	nothing: () -> %;
		++ The empty context.

	context: (String, Type, Type, SI, SI) -> %;
		++ context(f, t, x, d, c) creates a context associated
		++ with the function called f having type t exported
		++ by x called at depth d. The function has c parameters
		++ which will be assigned values later. The caller is
		++ responsible for setting the filename and line number.

	name: % -> String;
		++ name(c) returns the name of the function associated
		++ with the context c.

	type: % -> Type;
		++ type(c) returns the type of the function associated
		++ with the context c.

	exporter: % -> Type;
		++ type(c) returns the domain exporting the function
		++ associated with the context c.

	depth: % -> SingleInteger;
		++ depth(c) returns the call depth associated with the
		++ function context c.

	apply: (%, 'file') -> String;
		++ `c.file' returns the file name associated with the
		++ current execution position in c.

	apply: (%, 'name') -> String;
		++ `c.name' returns the prog name associated with the
		++ current execution position in c.

	apply: (%, 'line') -> SingleInteger;
		++ `c.line' returns the line number associated with the
		++ current execution position in c.

	apply: (%, 'parameters') -> Array(Parameter);
		++ `c.parameters' returns the parameters to this call.

	set!: (%, 'file', String) -> String;
		++ `c.file := myFile' is used to update the file
		++ associated with the current execution point in
		++ the context c.

	set!: (%, 'line', SingleInteger) -> SingleInteger;
		++ `c.line := myLine' is used to update the line
		++ associated with the current execution point in
		++ the context c.

	format: (%, SingleInteger) -> String;	
		++ format(tw, c, w) returns a text string containing
		++ the context c formatted into width w.

	nothing? : % -> Boolean;
		++ Returns true if this is the empty context.

	export from 'line', 'file', 'parameters', 'name';
}
== add
{
	Rep == Record
	(
		-- Constant parts.
		fname:String,
		ftype:Type,
		xtype:Type,
		cdepth:SingleInteger,
		argc:SingleInteger,
		params:Array(Parameter),


		-- Mutable parts.
		cfile:String,
		cline:SingleInteger
	);
	import from Rep, 'line', 'file', 'parameters', 'name';

	sample:% == nothing();

        local address(s:%):Pointer              == s pretend Pointer;
                                                                                                                              
        hash(s:%): MachineInteger  == { import from String; hash ((rep s).fname);}

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
	name(x:%):String == (rep x).fname;
	type(x:%):Type == (rep x).ftype;
	exporter(x:%):Type == (rep x).xtype;
	depth(x:%):SingleInteger == (rep x).cdepth;

	context(n:String, t:Type, x:Type, d:SI, c:SI):% ==
		per [n, t, x, d, c, new(c), "", 0];

	apply(x:%, dummy:'line'):SingleInteger == (rep x).cline;
	apply(x:%, dummy:'file'):String == (rep x).cfile;
	apply(x:%, dummy:'name'):String == (rep x).fname;
	apply(x:%, dummy:'parameters'):Array(Parameter) == (rep x).params;

	set!(x:%, dummy:'line', lin:SingleInteger):SingleInteger ==
		(rep x).cline := lin;

	set!(x:%, dummy:'file', fil:String):String ==
		(rep x).cfile := fil;

	(tw:TextWriter) << (con:%):TextWriter ==
	{
		import from Parameter, DomainName, TypePackage, Character;


		-- Display the context (if known).
		zero?(con pretend SI) => tw;


		-- Get the parameters.
		local fnpars:Array(Parameter) := con.parameters;


		-- Convert the parameters into an explicit generator.
		local fngen:Generator(Parameter) := generator fnpars;

		
		-- Display the function name.
		tw << newline;
		tw << name(con) << "(";

		import from Generator(Parameter);

		-- Start the generator rolling ...
		step! fngen;

		-- Display the parameters (if any).
		if (not empty? fngen) then
		{
			-- Display the first parameter.
			tw << (value fngen);


			-- Display the remaining parameters.
			for var in fngen repeat
				tw << ", " << var;
		}

		-- Closing brace.
		tw << ")";


		-- Do we have a type?
		if not zero?(type(con) pretend SI) then
			tw << ": " << typeName(type(con));


		-- Do we have an exporter?
		if not zero?(exporter(con) pretend SI) then
			tw << " from " << typeName(exporter(con));


		-- Display the current position.
		tw << " [_"";
		tw << (con.file);
		tw << "_" at line ";
		tw << (con.line);
		tw << "]";		

		-- Return the modified textwriter.
		tw;
	}


	format(con:%, width:SingleInteger):String ==
	{
		import from Character;
		import from DomainName, TypePackage;
		--import from FormattedOutput;
		--import from JustifyPackage(String);

		local fname, ftype, fexport, fpos:String;


		-- Shorthand for joining strings.
		--local (a:String) + (b:String):String == concat(a, b);


		-- Drop out if we don't have a context.
		zero?(con pretend SI) => new(width, space);


		-- Display the function name.
		--fname := string("~a")(<< name(con));
		fname := empty;
		fname::TextWriter << name(con);


		-- Get the parameters.
		local fnpars:Array(Parameter) := con.parameters;


		-- Convert the parameters into an explicit generator.
		local fngen:Generator(Parameter) := generator fnpars;


		-- Function types start with an opening brace.
		ftype := "(";


		-- Start the generator rolling ...
		step! fngen;


		-- Display the parameters (if any).
		if (not empty? fngen) then
		{
			-- Display the first parameter.
			--ftype := ftype + string("~a")(<< (value fngen));
			ftype::TextWriter << (value fngen);


			-- Display the remaining parameters.
			for var in fngen repeat
				--ftype := ftype + string(", ~a")(<< var);
				ftype::TextWriter << var;
		}


		-- Finish with the closing brace.
		ftype := ftype + ")";


		-- Do we have a return type?
		if not zero?(type(con) pretend SI) then
			--ftype := ftype + string(": ~a")(<< typeName(type(con)));
			ftype::TextWriter << typeName(type(con));


		-- Do we have an exporter?
		if not zero?(exporter(con) pretend SI) then
		{
			--fexport := string("~a")(<< typeName(exporter(con)));
			fexport::TextWriter <<  typeName(exporter(con));
			fexport := " from " + fexport;
		}
		else
			fexport := "";


		-- What is the source code position?
		fpos := "_"" +  con.file + "_" at line ";
		--fpos := string(" [~a~a]")(<< fpos, << (con.line));
		fpos::TextWriter << fpos << "," << (con.line);


		-- Create the formatted text.
		left := fname + ftype + fexport;


		-- Forget about source code position if there is no room.
		(#left > width) => substring(left, 1, width);


		-- Add source code position to the end, right justified.
		--concat(left, justify(fpos, width - #left, right));
		--left + justify(fpos, width - #left, right);
		left + fpos;
	}
}
