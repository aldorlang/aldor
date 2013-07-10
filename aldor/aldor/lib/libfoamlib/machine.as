-----------------------------------------------------------------------------
----
---- machine.as: Interface to FOAM -- loose operations for basic types.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#assert AssertConfig
#assert LibraryLanguage

#include "foamlib"


Byte ==> XByte;  --!! Needed to disambiguate

+++ `Machine' provides machine-level types and operations.
Machine: with {

	-- Types
	Bool:       Type;
	Byte:       Type;
	HInt:       Type;
	SInt:       Type;
	BInt:       Type;
	Char:       Type;
	SFlo:       Type;
	DFlo:       Type;
	Nil:        Type;
	Arr:        Type;
	Rec:        Type;
	Ptr:        Type;
	Word:       Type;

        -- Specific values
        true:       Bool;
        false:      Bool;

        space:      Char;
        newline:    Char;
        tab:	    Char;

        nil:        Ptr;

        0:          Byte;
        0:          SInt;
        0:          HInt;
        0:          BInt;
        0:          SFlo;
        0:          DFlo;

        1:          Byte;
        1:          BInt;
        1:          DFlo;
        1:          HInt;
        1:          SFlo;
        1:          SInt;

        max:        Char;
        max:        Byte;
        max:        HInt;
        max:        SInt;
        max:        SFlo;
        max:        DFlo;

        min:        Char;
        min:        Byte;
        min:        HInt;
        min:        SInt;
        min:        SFlo;
        min:        DFlo;

        epsilon:    SFlo;
        epsilon:    DFlo;

        -- Comparisons
        =:          (Bool, Bool) ->       Bool;
        =:          (Char, Char) ->       Bool;
        =:          (SInt, SInt) ->       Bool;
        =:          (BInt, BInt) ->       Bool;
        =:          (SFlo, SFlo) ->       Bool;
        =:          (DFlo, DFlo) ->       Bool;
        =:          (Ptr,  Ptr)  ->       Bool;

        ~=:         (Bool, Bool) ->       Bool;
        ~=:         (Char, Char) ->       Bool;
        ~=:         (SInt, SInt) ->       Bool;
        ~=:         (BInt, BInt) ->       Bool;
        ~=:         (SFlo, SFlo) ->       Bool;
        ~=:         (DFlo, DFlo) ->       Bool;
        ~=:         (Ptr,  Ptr)  ->       Bool;

        <:          (Char, Char) ->       Bool;
        <:          (SInt, SInt) ->       Bool;
        <:          (BInt, BInt) ->       Bool;
        <:          (SFlo, SFlo) ->       Bool;
        <:          (DFlo, DFlo) ->       Bool;

        <=:         (Char, Char) ->       Bool;
        <=:         (SInt, SInt) ->       Bool;
        <=:         (BInt, BInt) ->       Bool;
        <=:         (SFlo, SFlo) ->       Bool;
        <=:         (DFlo, DFlo) ->       Bool;

        -- Comparisons against specific values
        zero?:      (SInt) ->             Bool;
        zero?:      (BInt) ->             Bool;
        zero?:      (SFlo) ->             Bool;
        zero?:      (DFlo) ->             Bool;

        positive?:  (SInt) ->             Bool;
        positive?:  (BInt) ->             Bool;
        positive?:  (SFlo) ->             Bool;
        positive?:  (DFlo) ->             Bool;

        negative?:  (SInt) ->             Bool;
        negative?:  (BInt) ->             Bool;
        negative?:  (SFlo) ->             Bool;
        negative?:  (DFlo) ->             Bool;

        even?:      (BInt) ->             Bool;
        even?:      (SInt) ->             Bool;

        odd?:       (BInt) ->             Bool;
        odd?:       (SInt) ->             Bool;

        single?:    (BInt) ->             Bool;
        nil?:       (Ptr) ->              Bool;
        digit?:     (Char) ->             Bool;
        letter?:    (Char) ->             Bool;

        -- Logic
        ~:          (Bool) ->             Bool;
        ~:          (SInt) ->             SInt;
        /\:         (Bool, Bool) ->       Bool;
        /\:         (SInt, SInt) ->       SInt;
        \/:         (Bool, Bool) ->       Bool;
        \/:         (SInt, SInt) ->       SInt;
	xor: 	    (SInt, SInt) -> 	  SInt;

        -- Arithmetic
        +:          (SInt, SInt) ->       SInt;
        +:          (BInt, BInt) ->       BInt;
        +:          (SFlo, SFlo) ->       SFlo;
        +:          (DFlo, DFlo) ->       DFlo;
        -:          (SInt) ->             SInt;
        -:          (BInt) ->             BInt;
        -:          (SFlo) ->             SFlo;
        -:          (DFlo) ->             DFlo;
        -:          (SInt, SInt) ->       SInt;
        -:          (BInt, BInt) ->       BInt;
        -:          (SFlo, SFlo) ->       SFlo;
        -:          (DFlo, DFlo) ->       DFlo;
        *:          (SInt, SInt) ->       SInt;
        *:          (BInt, BInt) ->       BInt;
        *:          (SFlo, SFlo) ->       SFlo;
        *:          (DFlo, DFlo) ->       DFlo;
        _*_+:       (SInt, SInt, SInt) -> SInt; ++ `_*_+(a,b,c) = a*b + c'
        _*_+:       (BInt, BInt, BInt) -> BInt; ++ `_*_+(a,b,c) = a*b + c'
        _*_+:       (SFlo, SFlo, SFlo) -> SFlo; ++ `_*_+(a,b,c) = a*b + c'
        _*_+:       (DFlo, DFlo, DFlo) -> DFlo; ++ `_*_+(a,b,c) = a*b + c'
        /:          (SFlo, SFlo) ->       SFlo;
        /:          (DFlo, DFlo) ->       DFlo;
        ^:          (BInt, SInt) ->       BInt;
        ^:          (BInt, BInt) ->       BInt;
	mod_^:      (BInt, BInt, BInt) -> BInt;

        -- Integer operations
        length:     (SInt) ->             SInt;
        length:     (BInt) ->             SInt;
        bit:        (SInt, SInt) ->       Bool;
        bit:        (BInt, SInt) ->       Bool;
        shiftUp:    (SInt, SInt) ->       SInt;
        shiftUp:    (BInt, SInt) ->       BInt;
        shiftDown:  (SInt, SInt) ->       SInt;
        shiftDown:  (BInt, SInt) ->       BInt;

        next:       (SInt) ->             SInt;
        next:       (BInt) ->             BInt;
        prev:       (SInt) ->             SInt;
        prev:       (BInt) ->             BInt;

        quo:        (SInt, SInt) ->       SInt;
        quo:        (BInt, BInt) ->       BInt;
        rem:        (SInt, SInt) ->       SInt;
        rem:        (BInt, BInt) ->       BInt;
        mod:        (SInt, SInt) ->       SInt;
        mod:        (BInt, BInt) ->       BInt;
        divide:     (SInt, SInt) ->       (SInt, SInt);
        divide:     (BInt, BInt) ->       (BInt, BInt);
        gcd:        (SInt, SInt) ->       SInt;
        gcd:        (BInt, BInt) ->       BInt;

        mod_+:      (SInt, SInt, SInt) -> SInt;
        mod_-:      (SInt, SInt, SInt) -> SInt;
        mod_*:      (SInt, SInt, SInt) -> SInt;
        mod_*inv:  (SInt, SInt, SInt,DFlo) -> SInt;

	double_*:     (Word, Word) ->	    (Word, Word);
	doubleDivide: (Word, Word, Word) -> (Word, Word, Word);
	plusStep:     (Word, Word, Word) -> (Word, Word);
	timesStep:    (Word, Word, Word, Word) -> (Word, Word);

        -- Character operations
        upper:      (Char) ->             Char;
        lower:      (Char) ->             Char;
        char:       (SInt) ->             Char;
        ord:        (Char) ->             SInt;

        -- Floating point operations
        assemble:   (Bool, SInt, Word) ->       SFlo;
        assemble:   (Bool, SInt, Word, Word) -> DFlo;
        dissemble:  (SFlo) ->		  (Bool, SInt, Word);
        dissemble:  (DFlo) ->		  (Bool, SInt, Word, Word);

        next:       (SFlo) ->             SFlo;
        next:       (DFlo) ->             DFlo;
        prev:       (SFlo) ->             SFlo;
        prev:       (DFlo) ->             DFlo;

	
	-- Rounded arithmetic
	Bzero:    () -> SInt;
	Bnearest: () -> SInt;
	Bup:   	 () -> SInt;
	Bdown:    () -> SInt;
	Bany:	 () -> SInt;
	
	round:	    (SFlo, SInt)       -> BInt;
	round:	    (DFlo, SInt)       -> BInt;
	round_+:    (SFlo, SFlo, SInt) -> SFlo;
	round_+:    (DFlo, DFlo, SInt) -> DFlo;
	round_-:    (SFlo, SFlo, SInt) -> SFlo;
	round_-:    (DFlo, DFlo, SInt) -> DFlo;
	round_*:    (SFlo, SFlo, SInt) -> SFlo;
	round_*:    (DFlo, DFlo, SInt) -> DFlo;
	round_*_+:  (SFlo, SFlo, SFlo, SInt) -> SFlo;
	round_*_+:  (DFlo, DFlo, DFlo, SInt) -> DFlo;
	round_/:    (SFlo, SFlo, SInt) -> SFlo;
	round_/:    (DFlo, DFlo, SInt) -> DFlo;

	truncate:   (SFlo) ->		  BInt;
	truncate:   (DFlo) ->		  BInt;
	fraction:   (SFlo) ->		  SFlo;
	fraction:   (DFlo) ->		  DFlo;

	-- Basic conversions
	convert:    (SFlo) ->             DFlo;
	convert:    (DFlo) ->             SFlo;
	convert:    (Byte) ->             SInt;
	convert:    (SInt) ->             Byte;
	convert:    (HInt) ->             SInt;
	convert:    (SInt) ->             HInt;
	convert:    (SInt) ->             BInt;
	convert:    (BInt) ->             SInt;
	convert:    (SInt) ->             SFlo;
	convert:    (SInt) ->             DFlo;
	convert:    (BInt) ->             SFlo;
	convert:    (BInt) ->             DFlo;
	convert:    (Ptr)  ->             SInt;
	convert:    (SInt) ->             Ptr;

        convert:    (Arr) ->              SFlo;
        convert:    (Arr) ->              DFlo;
        convert:    (Arr) ->              SInt;
        convert:    (Arr) ->              BInt;

        -- Text conversion: the second argument to each of these
	-- functions MUST be writable. String literals are usually
	-- stored in read-only memory so they must NOT be used.
	-- A duplicate copy of a string literal will be okay.
        format:     (SInt, Arr, SInt) ->  SInt;
        format:     (BInt, Arr, SInt) ->  SInt;
        format:     (SFlo, Arr, SInt) ->  SInt;
        format:     (DFlo, Arr, SInt) ->  SInt;

        scan:       (Arr, SInt) ->        (SFlo, SInt);
        scan:       (Arr, SInt) ->        (DFlo, SInt);
        scan:       (Arr, SInt) ->        (SInt, SInt);
        scan:       (Arr, SInt) ->        (BInt, SInt);

	-- Environment
        RTE:        () ->                 SInt;
        OS:         () ->                 SInt;
        halt:       (SInt) ->             Exit;

	-- Array operations
        array:      (E: Type) -> (E,   SInt) ->    Arr;
        get:        (E: Type) -> (Arr, SInt) ->    E;
        set!:       (E: Type) -> (Arr, SInt, E) -> E;

	-- Wanton destruction
        dispose!:   (Arr)     -> ();
	dispose!:   (BInt)    -> ();

} == add {
	Bool:  Type == add;
	Byte:  Type == add;
	HInt:  Type == add;
	SInt:  Type == add;
	BInt:  Type == add;

	Char:  Type == add;

	SFlo:  Type == add;
	DFlo:  Type == add;

	Nil:   Type == add;
	Arr:   Type == add;
	Rec:   Type == add;
	Ptr:   Type == add;

	Word:  Type == add;

	import {
		BoolFalse:     () ->                      Bool;
		BoolTrue:      () ->                      Bool;
		BoolNot:       (Bool) ->                  Bool;
		BoolAnd:       (Bool, Bool) ->            Bool;
		BoolOr:        (Bool, Bool) ->            Bool;
		BoolEQ:        (Bool, Bool) ->            Bool;
		BoolNE:        (Bool, Bool) ->            Bool;

		CharSpace:     () ->                      Char;
		CharNewline:   () ->                      Char;
		CharTab:       () ->                      Char;
		CharMin:       () ->                      Char;
		CharMax:       () ->                      Char;
		CharIsDigit:   (Char) ->                  Bool;
		CharIsLetter:  (Char) ->                  Bool;
		CharEQ:        (Char,Char) ->             Bool;
		CharNE:        (Char,Char) ->             Bool;
		CharLT:        (Char,Char) ->             Bool;
		CharLE:        (Char,Char) ->             Bool;
		CharLower:     (Char) ->                  Char;
		CharUpper:     (Char) ->                  Char;
		CharOrd:       (Char) ->                  SInt;
		CharNum:       (SInt) ->                  Char;

		RoundZero:	() ->		  	  SInt;
		RoundNearest:	() ->		  	  SInt;
		RoundUp:	() ->		  	  SInt;
		RoundDown:	() ->		  	  SInt;
		RoundDontCare:	() ->		  	  SInt;

		SFlo0:         () ->                      SFlo;
		SFlo1:         () ->                      SFlo;
		SFloMin:       () ->                      SFlo;
		SFloMax:       () ->                      SFlo;
		SFloEpsilon:   () ->                      SFlo;
		SFloIsZero:    (SFlo) ->                  Bool;
		SFloIsNeg:     (SFlo) ->                  Bool;
		SFloIsPos:     (SFlo) ->                  Bool;
		SFloEQ:        (SFlo,SFlo) ->             Bool;
		SFloNE:        (SFlo,SFlo) ->             Bool;
		SFloLT:        (SFlo,SFlo) ->             Bool;
		SFloLE:        (SFlo,SFlo) ->             Bool;
		SFloNegate:    (SFlo) ->                  SFlo;
		SFloPrev:      (SFlo) ->		  SFlo;
		SFloNext:      (SFlo) ->                  SFlo;
		SFloPlus:      (SFlo,SFlo) ->             SFlo;
		SFloMinus:     (SFlo,SFlo) ->             SFlo;
		SFloTimes:     (SFlo,SFlo) ->             SFlo;
		SFloTimesPlus: (SFlo,SFlo,SFlo) ->        SFlo;
		SFloDivide:    (SFlo,SFlo) ->             SFlo;
		SFloRPlus:     (SFlo,SFlo,SInt) ->	  SFlo;
		SFloRMinus:    (SFlo,SFlo,SInt) ->	  SFlo;
		SFloRTimes:    (SFlo,SFlo,SInt) ->	  SFlo;
		SFloRTimesPlus:(SFlo,SFlo,SFlo,SInt) ->	  SFlo;
		SFloRDivide:   (SFlo,SFlo,SInt) ->	  SFlo;
		SFloTruncate:  (SFlo) ->		  BInt;
		SFloFraction:  (SFlo) ->		  SFlo;
		SFloRound:     (SFlo, SInt) ->		  BInt;
		SFloDissemble: (SFlo) ->		  (Bool, SInt, Word);
		SFloAssemble:  (Bool, SInt, Word) ->      SFlo;

		DFlo0:         () ->                      DFlo;
		DFlo1:         () ->                      DFlo;
		DFloMin:       () ->                      DFlo;
		DFloMax:       () ->                      DFlo;
		DFloEpsilon:   () ->                      DFlo;
		DFloIsZero:    (DFlo) ->                  Bool;
		DFloIsNeg:     (DFlo) ->                  Bool;
		DFloIsPos:     (DFlo) ->                  Bool;
		DFloEQ:        (DFlo,DFlo) ->             Bool;
		DFloNE:        (DFlo,DFlo) ->             Bool;
		DFloLT:        (DFlo,DFlo) ->             Bool;
		DFloLE:        (DFlo,DFlo) ->             Bool;
		DFloNegate:    (DFlo) ->                  DFlo;
		DFloPrev:      (DFlo) ->		  DFlo;
		DFloNext:      (DFlo) ->                  DFlo;
		DFloPlus:      (DFlo,DFlo) ->             DFlo;
		DFloMinus:     (DFlo,DFlo) ->             DFlo;
		DFloTimes:     (DFlo,DFlo) ->             DFlo;
		DFloTimesPlus: (DFlo,DFlo,DFlo) ->        DFlo;
		DFloDivide:    (DFlo,DFlo) ->             DFlo;
		DFloRPlus:     (DFlo,DFlo,SInt) ->	  DFlo;
		DFloRMinus:    (DFlo,DFlo,SInt) ->	  DFlo;
		DFloRTimes:    (DFlo,DFlo,SInt) ->	  DFlo;
		DFloRTimesPlus:(DFlo,DFlo,DFlo,SInt) ->	  DFlo;
		DFloRDivide:   (DFlo,DFlo,SInt) ->	  DFlo;
		DFloTruncate:  (DFlo) ->		  BInt;
		DFloFraction:  (DFlo) ->		  DFlo;
		DFloRound:     (DFlo, SInt) ->		  BInt;
		DFloDissemble: (DFlo) ->		  (Bool,SInt,Word,Word);
		DFloAssemble:  (Bool, SInt, Word, Word) ->DFlo;

		Byte0:         () ->                      Byte;
		Byte1:         () ->                      Byte;
		ByteMin:       () ->                      Byte;
		ByteMax:       () ->                      Byte;

		HInt0:         () ->                      HInt;
		HInt1:         () ->                      HInt;
		HIntMin:       () ->                      HInt;
		HIntMax:       () ->                      HInt;

		SInt0:         () ->                      SInt;
		SInt1:         () ->                      SInt;
		SIntMin:       () ->                      SInt;
		SIntMax:       () ->                      SInt;
		SIntIsZero:    (SInt) ->                  Bool;
		SIntIsNeg:     (SInt) ->                  Bool;
		SIntIsPos:     (SInt) ->                  Bool;
		SIntIsEven:    (SInt) ->                  Bool;
		SIntIsOdd:     (SInt) ->                  Bool;
		SIntEQ:        (SInt, SInt) ->            Bool;
		SIntNE:        (SInt, SInt) ->            Bool;
		SIntLT:        (SInt, SInt) ->            Bool;
		SIntLE:        (SInt, SInt) ->            Bool;
		SIntNegate:    (SInt) ->                  SInt;
		SIntPrev:      (SInt) ->                  SInt;
		SIntNext:      (SInt) ->                  SInt;
		SIntPlus:      (SInt, SInt) ->            SInt;
		SIntMinus:     (SInt, SInt) ->            SInt;
		SIntTimes:     (SInt, SInt) ->            SInt;
		SIntTimesPlus: (SInt, SInt, SInt) ->      SInt;
		SIntMod:       (SInt, SInt) ->            SInt;
		SIntQuo:       (SInt, SInt) ->            SInt;
		SIntRem:       (SInt, SInt) ->            SInt;
		SIntDivide:    (SInt, SInt) ->            (SInt, SInt);
		SIntGcd:       (SInt, SInt) ->            SInt;
		SIntPlusMod:   (SInt, SInt, SInt) ->      SInt;
		SIntMinusMod:  (SInt, SInt, SInt) ->      SInt;
		SIntTimesMod:  (SInt, SInt, SInt) ->      SInt;
		SIntTimesModInv:(SInt, SInt, SInt, DFlo)->SInt;
		SIntLength:    (SInt) ->                  SInt;
		SIntShiftUp:   (SInt, SInt) ->            SInt;
		SIntShiftDn:   (SInt, SInt) ->            SInt;
		SIntBit:       (SInt, SInt) ->            Bool;
		SIntNot:       (SInt) ->                  SInt;
		SIntAnd:       (SInt, SInt) ->            SInt;
		SIntOr:        (SInt, SInt) ->            SInt;
		SIntXOr:       (SInt, SInt) ->            SInt;

		WordTimesDouble:  (Word, Word) ->	 (Word, Word);
		WordDivideDouble: (Word, Word, Word) ->  (Word, Word, Word);
		WordPlusStep:	  (Word, Word, Word) ->	 (Word, Word);
		WordTimesStep:	  (Word, Word, Word, Word) -> (Word, Word);

		BInt0:         () ->                      BInt;
		BInt1:         () ->                      BInt;
		BIntIsZero:    (BInt) ->                  Bool;
		BIntIsNeg:     (BInt) ->                  Bool;
		BIntIsPos:     (BInt) ->                  Bool;
		BIntIsEven:    (BInt) ->                  Bool;
		BIntIsOdd:     (BInt) ->                  Bool;
		BIntIsSingle:  (BInt) ->                  Bool;
		BIntEQ:        (BInt, BInt) ->            Bool;
		BIntNE:        (BInt, BInt) ->            Bool;
		BIntLT:        (BInt, BInt) ->            Bool;
		BIntLE:        (BInt, BInt) ->            Bool;
		BIntNegate:    (BInt) ->                  BInt;
		BIntPrev:      (BInt) ->                  BInt;
		BIntNext:      (BInt) ->                  BInt;
		BIntPlus:      (BInt, BInt) ->            BInt;
		BIntMinus:     (BInt, BInt) ->            BInt;
		BIntTimes:     (BInt, BInt) ->            BInt;
		BIntTimesPlus: (BInt, BInt, BInt) ->      BInt;
		BIntMod:       (BInt, BInt) ->            BInt;
		BIntQuo:       (BInt, BInt) ->            BInt;
		BIntRem:       (BInt, BInt) ->            BInt;
		BIntDivide:    (BInt, BInt) ->            (BInt, BInt);
		BIntGcd:       (BInt, BInt) ->            BInt;
		BIntSIPower:   (BInt, SInt) ->            BInt;
		BIntBIPower:   (BInt, BInt) ->            BInt;
		BIntPowerMod:  (BInt, BInt, BInt) ->      BInt;
		BIntLength:    (BInt) ->                  SInt;
		BIntShiftUp:   (BInt, SInt) ->            BInt;
		BIntShiftDn:   (BInt, SInt) ->            BInt;
		BIntBit:       (BInt, SInt) ->            Bool;
		BIntDispose:   (BInt) ->                  ();

		PtrNil:        () ->                      Ptr;
		PtrIsNil:      (Ptr) ->                   Bool;
		PtrEQ:         (Ptr, Ptr) ->              Bool;
		PtrNE:         (Ptr, Ptr) ->              Bool;

		ArrDispose:    (Arr) ->                   ();

		FormatSFlo:    (SFlo,Arr,SInt) ->         SInt;
		FormatDFlo:    (DFlo,Arr,SInt) ->         SInt;
		FormatSInt:    (SInt,Arr,SInt) ->         SInt;
		FormatBInt:    (BInt,Arr,SInt) ->         SInt;

		ScanSFlo:      (Arr, SInt) ->             (SFlo, SInt);
		ScanDFlo:      (Arr, SInt) ->             (DFlo, SInt);
		ScanSInt:      (Arr, SInt) ->             (SInt, SInt);
		ScanBInt:      (Arr, SInt) ->             (BInt, SInt);

		SFloToDFlo:    (SFlo) ->                  DFlo;
		DFloToSFlo:    (DFlo) ->                  SFlo; -- except OFLOW

		ByteToSInt:    (Byte) ->                  SInt;
		SIntToByte:    (SInt) ->                  Byte; -- except OFLOW

		HIntToSInt:    (HInt) ->                  SInt;
		SIntToHInt:    (SInt) ->                  HInt; -- except OFLOW

		SIntToBInt:    (SInt) ->                  BInt;
		BIntToSInt:    (BInt) ->                  SInt; -- except OFLOW

		SIntToSFlo:    (SInt) ->                  SFlo;
		SIntToDFlo:    (SInt) ->                  DFlo;

		BIntToSFlo:    (BInt) ->                  SFlo; -- except OFLOW
		BIntToDFlo:    (BInt) ->                  DFlo; -- except OFLOW

		PtrToSInt:     (Ptr)  ->                  SInt;
		SIntToPtr:     (SInt) ->                  Ptr;

		ArrToSFlo:     (Arr) ->                   SFlo;
		ArrToDFlo:     (Arr) ->                   DFlo;
		ArrToSInt:     (Arr) ->                   SInt;
		ArrToBInt:     (Arr) ->                   BInt;

		PlatformRTE:   () ->                      SInt;
		PlatformOS:    () ->                      SInt;

		Halt:          (SInt) ->                  Exit;
	} from Builtin;


        -- Specific values
        false:      Bool ==                        BoolFalse();
        true:       Bool ==                        BoolTrue();

        space:      Char ==                        CharSpace();
        newline:    Char ==                        CharNewline();
        tab:	    Char ==                        CharTab();

        nil:        Ptr ==                         PtrNil();

        0:          Byte ==                        Byte0();
        0:          SInt ==                        SInt0();
        0:          HInt ==                        HInt0();
        0:          BInt ==                        BInt0();
        0:          SFlo ==                        SFlo0();
        0:          DFlo ==                        DFlo0();

        1:          Byte ==                        Byte1();
        1:          BInt ==                        BInt1();
        1:          DFlo ==                        DFlo1();
        1:          HInt ==                        HInt1();
        1:          SFlo ==                        SFlo1();
        1:          SInt ==                        SInt1();

        max:        Char ==                        CharMax();
        max:        Byte ==                        ByteMax();
        max:        HInt ==                        HIntMax();
        max:        SInt ==                        SIntMax();
        max:        SFlo ==                        SFloMax();
        max:        DFlo ==                        DFloMax();

        min:        Char ==                        CharMin();
        min:        Byte ==                        ByteMin();
        min:        HInt ==                        HIntMin();
        min:        SInt ==                        SIntMin();
        min:        SFlo ==                        SFloMin();
        min:        DFlo ==                        DFloMin();

        epsilon:    SFlo ==                        SFloEpsilon();
        epsilon:    DFlo ==                        DFloEpsilon();

        -- Comparisons
        (a: Bool) = (b: Bool):  Bool ==            BoolEQ(a, b);
        (a: Char) = (b: Char):  Bool ==            CharEQ(a, b);
        (a: SInt) = (b: SInt):  Bool ==            SIntEQ(a, b);
        (a: BInt) = (b: BInt):  Bool ==            BIntEQ(a, b);
        (a: SFlo) = (b: SFlo):  Bool ==            SFloEQ(a, b);
        (a: DFlo) = (b: DFlo):  Bool ==            DFloEQ(a, b);
        (a: Ptr)  = (b: Ptr):   Bool ==            PtrEQ (a, b);

        (a: Bool) ~= (b: Bool): Bool ==            BoolNE(a, b);
        (a: Char) ~= (b: Char): Bool ==            CharNE(a, b);
        (a: SInt) ~= (b: SInt): Bool ==            SIntNE(a, b);
        (a: BInt) ~= (b: BInt): Bool ==            BIntNE(a, b);
        (a: SFlo) ~= (b: SFlo): Bool ==            SFloNE(a, b);
        (a: DFlo) ~= (b: DFlo): Bool ==            DFloNE(a, b);
        (a: Ptr)  ~= (b: Ptr):  Bool ==            PtrNE (a, b);

        (a: Char) < (b: Char):  Bool ==            CharLT(a, b);
        (a: SInt) < (b: SInt):  Bool ==            SIntLT(a, b);
        (a: BInt) < (b: BInt):  Bool ==            BIntLT(a, b);
        (a: SFlo) < (b: SFlo):  Bool ==            SFloLT(a, b);
        (a: DFlo) < (b: DFlo):  Bool ==            DFloLT(a, b);

        (a: Char) <= (b: Char): Bool ==            CharLE(a, b);
        (a: SInt) <= (b: SInt): Bool ==            SIntLE(a, b);
        (a: BInt) <= (b: BInt): Bool ==            BIntLE(a, b);
        (a: SFlo) <= (b: SFlo): Bool ==            SFloLE(a, b);
        (a: DFlo) <= (b: DFlo): Bool ==            DFloLE(a, b);

        -- Comparisons against specific values
        zero?    (a: SInt): Bool ==                SIntIsZero a;
        zero?    (a: BInt): Bool ==                BIntIsZero a;
        zero?    (a: SFlo): Bool ==                SFloIsZero a;
        zero?    (a: DFlo): Bool ==                DFloIsZero a;

        positive?(a: SInt): Bool ==                SIntIsPos  a;
        positive?(a: BInt): Bool ==                BIntIsPos  a;
        positive?(a: SFlo): Bool ==                SFloIsPos  a;
        positive?(a: DFlo): Bool ==                DFloIsPos  a;

        negative?(a: SInt): Bool ==                SIntIsNeg  a;
        negative?(a: BInt): Bool ==                BIntIsNeg  a;
        negative?(a: SFlo): Bool ==                SFloIsNeg  a;
        negative?(a: DFlo): Bool ==                DFloIsNeg  a;

        even?    (a: BInt): Bool ==                BIntIsEven a;
        even?    (a: SInt): Bool ==                SIntIsEven a;

        odd?     (a: BInt): Bool ==                BIntIsOdd  a;
        odd?     (a: SInt): Bool ==                SIntIsOdd  a;

        single?  (a: BInt): Bool ==                BIntIsSingle a;
        nil?     (a: Ptr) : Bool ==                PtrIsNil     a;
        digit?   (a: Char): Bool ==                CharIsDigit  a;
        letter?  (a: Char): Bool ==                CharIsLetter a;


        -- Logic
        ~(a: Bool): Bool ==                        BoolNot a;
        ~(a: SInt): SInt ==                        SIntNot a;

        (a: Bool) /\ (b: Bool): Bool ==            BoolAnd(a, b);
        (a: SInt) /\ (b: SInt): SInt ==            SIntAnd(a, b);

        (a: Bool) \/ (b: Bool): Bool ==            BoolOr (a, b);
        (a: SInt) \/ (b: SInt): SInt ==            SIntOr (a, b);

        xor(a: SInt,  b: SInt): SInt ==            SIntXOr (a, b);

        -- Arithmetic
        (a: SInt) + (b: SInt): SInt ==             SIntPlus   (a, b);
        (a: BInt) + (b: BInt): BInt ==             BIntPlus   (a, b);
        (a: SFlo) + (b: SFlo): SFlo ==             SFloPlus   (a, b);
        (a: DFlo) + (b: DFlo): DFlo ==             DFloPlus   (a, b);

        - (a: SInt): SInt ==                       SIntNegate a;
        - (a: BInt): BInt ==                       BIntNegate a;
        - (a: SFlo): SFlo ==                       SFloNegate a;
        - (a: DFlo): DFlo ==                       DFloNegate a;

        (a: SInt) - (b: SInt): SInt ==             SIntMinus  (a, b);
        (a: BInt) - (b: BInt): BInt ==             BIntMinus  (a, b);
        (a: SFlo) - (b: SFlo): SFlo ==             SFloMinus  (a, b);
        (a: DFlo) - (b: DFlo): DFlo ==             DFloMinus  (a, b);

        (a: SInt) * (b: SInt): SInt ==             SIntTimes  (a, b);
        (a: BInt) * (b: BInt): BInt ==             BIntTimes  (a, b);
        (a: SFlo) * (b: SFlo): SFlo ==             SFloTimes  (a, b);
        (a: DFlo) * (b: DFlo): DFlo ==             DFloTimes  (a, b);

        _*_+(a: SInt, b: SInt, c: SInt):SInt ==    SIntTimesPlus(a, b, c);
        _*_+(a: BInt, b: BInt, c: BInt):BInt ==    BIntTimesPlus(a, b, c);
        _*_+(a: SFlo, b: SFlo, c: SFlo):SFlo ==    SFloTimesPlus(a, b, c);
        _*_+(a: DFlo, b: DFlo, c: DFlo):DFlo ==    DFloTimesPlus(a, b, c);

        (a: SFlo) / (b: SFlo): SFlo ==             SFloDivide (a, b);
        (a: DFlo) / (b: DFlo): DFlo ==             DFloDivide (a, b);

        (a: BInt) ^ (b: SInt): BInt ==             BIntSIPower(a, b);
        (a: BInt) ^ (b: BInt): BInt ==             BIntBIPower(a, b);
	mod_^(a:BInt, b:BInt, c:BInt):BInt   ==    BIntPowerMod(a, b, c);

        -- Integer operations
        length(a: SInt): SInt ==                   SIntLength a;
        length(a: BInt): SInt ==                   BIntLength a;

        bit   (a: SInt, n: SInt): Bool ==          SIntBit(a, n);
        bit   (a: BInt, n: SInt): Bool ==          BIntBit(a, n);

        shiftUp  (a: SInt, n: SInt): SInt ==       SIntShiftUp(a, n);
        shiftUp  (a: BInt, n: SInt): BInt ==       BIntShiftUp(a, n);

        shiftDown(a: SInt, n: SInt): SInt ==       SIntShiftDn(a, n);
        shiftDown(a: BInt, n: SInt): BInt ==       BIntShiftDn(a, n);

        next  (a: SInt): SInt ==                   SIntNext a;
        next  (a: BInt): BInt ==                   BIntNext a;

        prev  (a: SInt): SInt ==                   SIntPrev a;
        prev  (a: BInt): BInt ==                   BIntPrev a;

        (a: SInt) quo (b: SInt): SInt ==           SIntQuo(a, b);
        (a: BInt) quo (b: BInt): BInt ==           BIntQuo(a, b);

        (a: SInt) rem (b: SInt): SInt ==           SIntRem(a, b);
        (a: BInt) rem (b: BInt): BInt ==           BIntRem(a, b);

        (a: SInt) mod (n: SInt): SInt ==           SIntMod(a, n);
        (a: BInt) mod (n: BInt): BInt ==           BIntMod(a, n);

        divide(a: SInt, b: SInt): (SInt, SInt) ==  SIntDivide(a, b);
        divide(a: BInt, b: BInt): (BInt, BInt) ==  BIntDivide(a, b);

        gcd   (a: SInt, b: SInt): SInt ==          SIntGcd(a, b);
        gcd   (a: BInt, b: BInt): BInt ==          BIntGcd(a, b);

        mod_+ (a: SInt, b: SInt, n: SInt): SInt == SIntPlusMod (a, b, n);
        mod_- (a: SInt, b: SInt, n: SInt): SInt == SIntMinusMod(a, b, n);
        mod_* (a: SInt, b: SInt, n: SInt): SInt == SIntTimesMod(a, b, n);
        mod_*inv(a:SInt,b:SInt,n:SInt,ni:DFlo): SInt ==
						   SIntTimesModInv(a,b,n,ni);

	double_*(a: Word, b: Word): (hi: Word, lo: Word) ==
						   WordTimesDouble(a,b);

	doubleDivide(nh:Word, nl:Word, d:Word): (qhi:Word, qlo:Word, r:Word) ==
						   WordDivideDouble(nh,nl,d);

	plusStep(a:Word, b:Word, kin:Word): (kout:Word, r:Word) ==
						   WordPlusStep(a,b,kin);

	timesStep(a:Word, b:Word, c:Word, kin:Word): (kout:Word, r:Word) ==
						   WordTimesStep(a,b,c,kin);

        -- Character operations
        upper   (c: Char): Char ==                 CharUpper c;
        lower   (c: Char): Char ==                 CharLower c;
        char    (n: SInt): Char ==                 CharNum   n;
        ord     (c: Char): SInt ==                 CharOrd   c;

        -- Floating point operations
        assemble(s:Bool, e:SInt, b: Word): SFlo     == SFloAssemble(s,e,b);
        assemble(s:Bool,e:SInt,bh:Word,bl:Word):DFlo== DFloAssemble(s,e,bh,bl);

        dissemble(x: SFlo): (Bool, SInt, Word)      == SFloDissemble(x);
        dissemble(x: DFlo): (Bool, SInt, Word, Word)== DFloDissemble(x);

        next(x: SFlo): SFlo == SFloNext x;
        next(x: DFlo): DFlo == DFloNext x;

        prev(x: SFlo): SFlo == SFloPrev x;
        prev(x: DFlo): DFlo == DFloPrev x;

	-- Rounded arithmetic
	Bzero()	  :SInt == RoundZero();
	Bnearest():SInt == RoundNearest();
	Bup()	  :SInt == RoundUp();
	Bdown()	  :SInt == RoundDown();
	Bany()    :SInt == RoundDontCare();

	round(a:SFlo, d:SInt):BInt == SFloRound(a,d);
	round(a:DFlo, d:SInt):BInt == DFloRound(a,d);
	
	round_+(a:SFlo, b:SFlo, d:SInt): SFlo ==   SFloRPlus(a,b,d);
	round_+(a:DFlo, b:DFlo, d:SInt): DFlo ==   DFloRPlus(a,b,d);

	round_-(a:SFlo, b:SFlo, d:SInt): SFlo ==   SFloRMinus(a,b,d);
	round_-(a:DFlo, b:DFlo, d:SInt): DFlo ==   DFloRMinus(a,b,d);

	round_*(a:SFlo, b:SFlo, d:SInt): SFlo ==   SFloRTimes(a,b,d);
	round_*(a:DFlo, b:DFlo, d:SInt): DFlo ==   DFloRTimes(a,b,d);

	round_*_+(a:SFlo, b:SFlo, c:SFlo, d:SInt): SFlo ==
						   SFloRTimesPlus(a,b,c,d);
	round_*_+(a:DFlo, b:DFlo, c:DFlo, d:SInt): DFlo ==
						   DFloRTimesPlus(a,b,c,d);

	round_/(a:SFlo, b:SFlo, d:SInt): SFlo ==   SFloRDivide(a,b,d);
	round_/(a:DFlo, b:DFlo, d:SInt): DFlo ==   DFloRDivide(a,b,d);

	truncate(a:SFlo):BInt == SFloTruncate(a);
	truncate(a:DFlo):BInt == DFloTruncate(a);
	fraction(a:SFlo):SFlo == SFloFraction(a);
	fraction(a:DFlo):DFlo == DFloFraction(a);

        -- Basic conversions
	convert(a: SFlo): DFlo ==                  SFloToDFlo a;
	convert(a: DFlo): SFlo ==                  DFloToSFlo a;
	convert(a: Byte): SInt ==                  ByteToSInt a;
	convert(a: SInt): Byte ==                  SIntToByte a;
	convert(a: HInt): SInt ==                  HIntToSInt a;
	convert(a: SInt): HInt ==                  SIntToHInt a;
	convert(a: SInt): BInt ==                  SIntToBInt a;
	convert(a: BInt): SInt ==                  BIntToSInt a;
	convert(a: SInt): SFlo ==                  SIntToSFlo a;
	convert(a: SInt): DFlo ==                  SIntToDFlo a;
	convert(a: BInt): SFlo ==                  BIntToSFlo a;
	convert(a: BInt): DFlo ==                  BIntToDFlo a;
	convert(a: Ptr) : SInt ==                  PtrToSInt  a;
	convert(a: SInt): Ptr  ==                  SIntToPtr  a;

        convert(a: Arr) : SFlo ==                  ArrToSFlo  a;
        convert(a: Arr) : DFlo ==                  ArrToDFlo  a;
        convert(a: Arr) : SInt ==                  ArrToSInt  a;
        convert(a: Arr) : BInt ==                  ArrToBInt  a;

        -- Text conversion
        format(a: SInt, s: Arr, n: SInt): SInt ==  FormatSInt(a, s, n);
        format(a: BInt, s: Arr, n: SInt): SInt ==  FormatBInt(a, s, n);
        format(a: SFlo, s: Arr, n: SInt): SInt ==  FormatSFlo(a, s, n);
        format(a: DFlo, s: Arr, n: SInt): SInt ==  FormatDFlo(a, s, n);

        scan  (s: Arr, n: SInt): (SFlo, SInt) ==   ScanSFlo(s, n);
        scan  (s: Arr, n: SInt): (DFlo, SInt) ==   ScanDFlo(s, n);
        scan  (s: Arr, n: SInt): (SInt, SInt) ==   ScanSInt(s, n);
        scan  (s: Arr, n: SInt): (BInt, SInt) ==   ScanBInt(s, n);

        -- Environment
        RTE (): SInt ==                            PlatformRTE();
        OS  (): SInt ==                            PlatformOS();
        halt(rc: SInt): Exit ==                    Halt rc;

        -- Array operations
        array(E: Type)(e: E,  n: SInt): Arr == {
		import { ArrNew: (E, SInt) -> Arr; } from Builtin;
		ArrNew(e, n);
	}
        get (E: Type)(a: Arr, n: SInt): E == {
		import { ArrElt: (Arr, SInt) -> E; } from Builtin;
		ArrElt(a, n);
	}
        set!(E: Type)(a: Arr, n: SInt, e: E): E == {
		import { ArrSet: (Arr, SInt, E) -> E; } from Builtin;
		ArrSet(a, n, e);
	}

	-- Deposing
        dispose!(a: Arr):  () == ArrDispose  a;
	dispose!(b: BInt): () == BIntDispose b;
}


