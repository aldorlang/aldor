#include "axllib"

-- the five IEEE 754 exceptions
IeeeException ==> Enumeration(inexact,dividebyzero,underflow,overflow,invalid);

+++ This is essentially a bit-vector of length 5.
BasicIeeeExceptionSet: 
	Logic with {
	on:		IeeeException 		-> % -> %;
		++ `on.exc x' turns exception exc on in x
	on?:		(IeeeException,%) 	-> Boolean;
		++ `on?(exc,x)' returns true if exception exc is on in x
	off:		IeeeException 		-> % -> %;
		++ `off.exc x' turns exception exc off in x
	off?:		(IeeeException,%) 	-> Boolean;
		++ `off?(exc,x)' returns true if exception exc is off in x
	empty:		() 			-> %;
		++ `empty()' returns a new exception set with all exceptions off.
	export from IeeeException;
	} == add {
 	Rep == Record(s:SingleInteger);
	import from Rep;

	-- our coding of the five cases
	INEXACT		==> 1;
	DIVIDEBYZERO	==> 2;
	UNDERFLOW	==> 4;
	OVERFLOW	==> 8;
	INVALID		==> 16;

	empty():% == per [0];
	on(f:IeeeException)(x:%): % == {
		flag:= {
			f = inexact => INEXACT;
			f = dividebyzero => DIVIDEBYZERO;
			f = underflow => UNDERFLOW;
			f = overflow => OVERFLOW;
			f = invalid => INVALID;
			0
			}
		rep(x).s := rep(x).s \/ flag;
		x;
		}
	on?(f:IeeeException,x:%):Boolean == {
		f = inexact => not zero? (rep(x).s /\ INEXACT);
		f = dividebyzero => not zero? (rep(x).s /\ DIVIDEBYZERO);
		f = underflow => not zero? (rep(x).s /\ UNDERFLOW);
		f = overflow => not zero? (rep(x).s /\ OVERFLOW);
		f = invalid => not zero? (rep(x).s /\ INVALID);
		false;
		}
	off?(f:IeeeException,x:%):Boolean == not on?(f,x);
		

	off(f:IeeeException)(x:%): % == {
		flag:= {
			f = inexact => INEXACT;
			f = dividebyzero => DIVIDEBYZERO;
			f = underflow => UNDERFLOW;
			f = overflow => OVERFLOW;
			f = invalid => INVALID;
			0
			}
		rep(x).s := rep(x).s /\ ~flag;
		x;
		}
	sample:% == per[11];
	(x:%) = (y:%) :Boolean == rep(x).s = rep(y).s;	
	~ (x:%):% == {
		y:=empty();
		rep(y).s :=  INEXACT+DIVIDEBYZERO+UNDERFLOW+OVERFLOW+INVALID - rep(x).s;
		y
		}
	(x:%) /\ (y:%) :% == {
		z:=empty();
		rep(z).s := rep(x).s /\ rep(y).s;
		z
		}
	(p:TextWriter) << (x:%) :TextWriter == {
		import from IeeeException;
		p << "[";
		if on?(inexact,x) then p << "inexact ";
		if on?(dividebyzero,x) then p << "dividebyzero ";
		if on?(overflow,x) then p << "overflow ";
		if on?(underflow,x) then p << "underflow ";
		if on?(invalid,x) then p << "invalid ";
		p << "]"
		}
	test (x:%) : Boolean == rep(x).s ~= 0;

}

BasicIeeeControlPackage : with {
	initialiseFpu:	()->();
	getRoundingMode: 	() -> RoundingMode;
		++ `getRoundingMode()' returns the current rounding mode
	setRoundingMode: 	RoundingMode -> RoundingMode;
		++ `setRoundingMode(mode)' sets the rounding mode to mode and returns 
		++ the old one
	setDefaultRoundingMode:	() -> RoundingMode;
		++ `setDefaultRoundingMode()' sets the rounding mode to the default value
		++ and returns the old one
	getEnabledExceptions: 	() -> BasicIeeeExceptionSet;
		++ `getEnabledExceptions()' returns the currently trapped exceptions
	setEnabledExceptions: 	BasicIeeeExceptionSet -> BasicIeeeExceptionSet;
		++ `setEnabledExceptions(set)' enables trapping of the exceptions 
		++ turned on in set, disables trapping of the exceptions turned off 
		++ and returns the previously trapped exceptions.
	getExceptionFlags:		() -> BasicIeeeExceptionSet;
		++ `getExceptionFlags()' returns the current exception flags (sticky bits)
	setExceptionFlags:		(BasicIeeeExceptionSet) ->BasicIeeeExceptionSet; 
		++ `setExceptionFlags(set)' sets the exception flags (sticky bits) turned
		++ on in set, clears the ones turned off and returns the previous 
		++ exception flags
	} == add {
	import {
		fiInitialiseFpu:		()->();
		fiIeeeGetRoundingMode:		()->RoundingMode;
		fiIeeeSetRoundingMode:		RoundingMode->RoundingMode;
		fiIeeeGetEnabledExceptions: 	()-> SingleInteger;
		fiIeeeSetEnabledExceptions: 	SingleInteger -> SingleInteger;
		fiIeeeGetExceptionStatus:	()-> SingleInteger;
		fiIeeeSetExceptionStatus:	SingleInteger -> SingleInteger;
		} from Foreign;
	import from Record(s:SingleInteger);

	getRoundingMode():RoundingMode == fiIeeeGetRoundingMode();
	setRoundingMode(r:RoundingMode):RoundingMode ==
		fiIeeeSetRoundingMode(r);
	setDefaultRoundingMode():RoundingMode == setRoundingMode nearest();

	getEnabledExceptions():BasicIeeeExceptionSet == 
		[fiIeeeGetEnabledExceptions()] pretend  BasicIeeeExceptionSet;
	setEnabledExceptions(x:BasicIeeeExceptionSet):BasicIeeeExceptionSet == {
		[fiIeeeSetEnabledExceptions( (x pretend Record (s:SingleInteger)).s )] 
			pretend BasicIeeeExceptionSet;
		}

	getExceptionFlags():BasicIeeeExceptionSet == 
		[fiIeeeGetExceptionStatus()] pretend  BasicIeeeExceptionSet;
	setExceptionFlags(x:BasicIeeeExceptionSet):BasicIeeeExceptionSet ==
		[fiIeeeSetExceptionStatus( (x pretend Record (s:SingleInteger)).s )] 
			pretend BasicIeeeExceptionSet;
	initialiseFpu():() == fiInitialiseFpu();
}


BasicDoubleHexPrint : with {
	<<:	DoubleFloat -> TextWriter ->TextWriter;
		++ `(<< f) t' prints a hexadecimal representation of f on t
		++ and returns the new t;
	hex:    DoubleFloat -> String;
		++ `hex(f)' returns a string containing a hexadecimal representation
		++ of f
	} == add {
	import {
		fiDoubleHexPrintToString: DoubleFloat -> String;
		} from Foreign;

	(<<)(f: DoubleFloat)(t: TextWriter): TextWriter == {
		s:= fiDoubleHexPrintToString(f);
		write!(t,s);
		t;
		}
		
	hex(f:DoubleFloat):String == fiDoubleHexPrintToString(f);
}
	

#if TEST
sinTaylorSeries(x:DoubleFloat):DoubleFloat == {
	-- x -1/3! x^3 + 1/5! x^5 - 1/7! x^7
	f:=x;
	power:=x*x*x;
	factorial:=-1.0/6.0;
	index:=3.0;
	repeat {
		f:= f + power* factorial;
		power:=power*x*x;		
		factorial:= - factorial / ((index+1.) *(index+2.));
		(index:=index+2.0) > 101.0 => break;
		}
	f
}		

sinTaylorSeriesKahanSummation(x:DoubleFloat):DoubleFloat == {
	-- x -1/3! x^3 + 1/5! x^5 - 1/7! x^7
	f:=x;
	power:=x*x*x;
	factorial:=-1.0/6.0;
	index:=3.0;
	C:=0.0;
	repeat {
		Y:= power *factorial - C;
		T:=f+Y;
		C:=(T-f)-Y;
		f:=T;
		power:=power*x*x;		
		factorial:= - factorial / ((index+1.0) *(index+2.0));
		(index:=index+2.0) > 101.0 => break;
		}
	f
}		

test():() == {
	
	H==>BasicDoubleHexPrint;
	DF==>DoubleFloat;

	import from BasicDoubleHexPrint;

	import from BasicIeeeControlPackage,BasicIeeeExceptionSet;

	import from FormattedOutput,Integer,String;

	local r:RoundingMode ;
	local r1:RoundingMode ;
	local f,g:DoubleFloat;

	f:=1.0;
	g:=2.^(-54);

	sf:="3ff00000 00000000";
	sg:="3c900000 00000000";
	s1:="3cb00000 00000000";
	s2:="3ca00000 00000000";
	s3:="3c900000 00000000";
	s4:="80000000 00000000";
	s5:="00000000 00000000";

	r:= up();
	r1:=setRoundingMode(r);
	r:=getRoundingMode();
	print."was ~a is ~a~n"(<< r1, << r);
	print."f=~t~a ~a~ng=~t~a ~a~nf+g-f=~t~a ~a~ng-f+f=~t~a ~a~nf-f+g=~t~a ~a~n"(_
		<<$H(f),     << (hex(f)$H     = sf),_
		<<$H(g),     << (hex(g)$H     = sg),_
		<<$H(f+g-f), << (hex(f+g-f)$H = s1),_
		<<$H(g-f+f), << (hex(g-f+f)$H = s2),_
		<<$H(f-f+g), << (hex(f-f+g)$H = s3));
	

	r:=down();
	r1:=setRoundingMode(r);
	r:=getRoundingMode();
	print."was ~a is ~a~n"(<< r1, << r);
	print."f=~t~a ~a~ng=~t~a ~a~nf+g-f=~t~a ~a~ng-f+f=~t~a ~a~nf-f+g=~t~a ~a~n"(_
		<<$H(f),     << (hex(f)$H     = sf),_
		<<$H(g),     << (hex(g)$H     = sg),_
		<<$H(f+g-f), << (hex(f+g-f)$H = s4),_
		<<$H(g-f+f), << (hex(g-f+f)$H = s4),_
		<<$H(f-f+g), << (hex(f-f+g)$H = s3));

	r:= nearest();
	r1:=setRoundingMode(r);
	r:=getRoundingMode();
	print."was ~a is ~a~n"(<< r1, << r);
	print."f=~t~a ~a~ng=~t~a ~a~nf+g-f=~t~a ~a~ng-f+f=~t~a ~a~nf-f+g=~t~a ~a~n"(_
		<<$H(f),     << (hex(f)$H     = sf),_
		<<$H(g),     << (hex(g)$H     = sg),_
		<<$H(f+g-f), << (hex(f+g-f)$H = s5),_
		<<$H(g-f+f), << (hex(g-f+f)$H = s5),_
		<<$H(f-f+g), << (hex(f-f+g)$H = s3));

	r:=zero();
	r1:=setRoundingMode(r);
	r:=getRoundingMode();
	print."was ~a is ~a~n"(<< r1, << r);
	print."f=~t~a ~a~ng=~t~a ~a~nf+g-f=~t~a ~a~ng-f+f=~t~a ~a~nf-f+g=~t~a ~a~n"(_
		<<$H(f),     << (hex(f)$H     = sf),_
		<<$H(g),     << (hex(g)$H     = sg),_
		<<$H(f+g-f), << (hex(f+g-f)$H = s5),_
		<<$H(g-f+f), << (hex(g-f+f)$H = s2),_
		<<$H(f-f+g), << (hex(f-f+g)$H = s3));

	r1:=setDefaultRoundingMode();
	r:=getRoundingMode();
	print."was ~a is ~a~n"(<< r1, << r);
	print."f=~t~a ~a~ng=~t~a ~a~nf+g-f=~t~a ~a~ng-f+f=~t~a ~a~nf-f+g=~t~a ~a~n"(_
		<<$H(f),     << (hex(f)$H     = sf),_
		<<$H(g),     << (hex(g)$H     = sg),_
		<<$H(f+g-f), << (hex(f+g-f)$H = s5),_
		<<$H(g-f+f), << (hex(g-f+f)$H = s5),_
		<<$H(f-f+g), << (hex(f-f+g)$H = s3));


  -- test taylor series
	pi:=3.1415926535897932385;
	print."pi = ~a~n".(<<$DF pi);
	print."pi = ~a~n".(<<$H pi);

	setRoundingMode(down());	
	lo:=sinTaylorSeries(pi);
	setRoundingMode(up());	
	hi:=sinTaylorSeries(pi);
	print."sin(pi) by Taylor series         = [ ~a , ~a ]~n".(<<$DF lo, <<$DF hi);
	print."sin(pi) by Taylor series         = [ ~a , ~a ]~n".(<<$H lo, <<$H hi); 

	setRoundingMode(down());	
	lo:= sinTaylorSeriesKahanSummation(pi);
	setRoundingMode(up());	
	hi:= sinTaylorSeriesKahanSummation(pi);
	print."sin(pi) by Taylor series (Kahan) = [ ~a , ~a ]~n".(<<$DF lo, <<$DF hi);
	print."sin(pi) by Taylor series (Kahan) = [ ~a , ~a ]~n".(<<$H lo, <<$H hi);

	sinpi:=sin(pi)$DoubleFloatElementaryFunctions;
	print."sin(pi) = ~a ~a~n".(<<$DF sinpi, <<$H sinpi);

  -- test traps
	--     first get sticky stuff
	print."getExceptionFlags() = ~a~n".(<< getExceptionFlags());
	setExceptionFlags(empty());
	print."cleared exceptions~ngetExceptionFlags() = ~a~n".(<< getExceptionFlags());
	set:=getEnabledExceptions();
	print."getEnabledExceptions = ~a~n".(<< set);	
	on.overflow on.invalid on.underflow on.dividebyzero on.inexact set;
	print."setEnabledExceptions(~a) = ~a~n".(<< set, << setEnabledExceptions(set));
	print."getEnabledExceptions = ~a~n".(<< set);	
	-- turn them off;
	off.overflow off.invalid off.underflow off.dividebyzero off.inexact set;
	print."setEnabledExceptions(~a) = ~a~n".(<< set, << setEnabledExceptions(set));
	print."getEnabledExceptions = ~a~n".(<< set);	
  -- test sticky bits
	print."setExceptionFlags(~a) = ~a~n".(<< empty(), << setExceptionFlags(empty()));
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());
	f:=1.0/0.0;
	print."1/0 = ~a~n".(<<$DF f);	
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());

	print."setExceptionFlags(~a) = ~a~n".(<< empty(), << setExceptionFlags(empty()));
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());
	f:=f-f;
	print."Inf-Inf = ~a~n".(<<$DF f);	
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());

	print."setExceptionFlags(~a) = ~a~n".(<< empty(), << setExceptionFlags(empty()));
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());
	f:=1.0/3.0;
	print."1/3 = ~a~n".(<<$DF f);	
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());

	print."setExceptionFlags(~a) = ~a~n".(<< empty(), << setExceptionFlags(empty()));
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());
	f:=1.E-220 * 1.E-220;
	print."small*small = ~a~n".(<<$DF f);	
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());

	print."setExceptionFlags(~a) = ~a~n".(<< empty(), << setExceptionFlags(empty()));
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());
	f:=1.E220 * 1.E220;
	print."big*big = ~a~n".(<<$DF f);	
	print."getExceptionFlags() = ~a~n".( << getExceptionFlags());


	}
test(); 

#endif

