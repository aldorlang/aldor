#!/bin/sh
#
# Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

cd ${TMPDIR-/tmp}

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	UNICL=unicl.sh
	OBJ=obj
	LM=
else
	UNICL=unicl
	OBJ=o
	LM=-lm
fi


# Generate the runtime system file.
echo "== (Generating the runtime system)"
cat > rtime.c << EOF
#include <stdio.h>

void sink(int x) { (void)printf("*** Success!\n"); }
int INIT__0_rtexns(void) { }
EOF


# Generate the Aldor test file.
echo "== (Generating the test program)"
cat > eager0.as << EOF
-- We don't need all the rubbish that comes with standard libraries.
Category:with == add;
Type:Category == with;
Tuple(T:Type):with == add;
(args:Tuple Type) -> (results:Tuple Type):with == add;


-- Basic, Machine: doesn't matter.
Basic:with { Bool:Type; } == add { Bool:Type == add; }


-- Can't get much simpler than this.
Boolean:with { true: %; } == add
{
   import from Basic;
   import { BoolTrue: () -> Bool; } from Builtin;

   true:% == BoolTrue() pretend %;
}


-- This is the domain that will be instantiated first. Provided blob()
-- uses its argument "x" in some form, we initialise initialise Foo(%)
-- before we create the exports blob and 1.
--
-- Now if Foo(%) is eager then we ask our category a question. This in
-- turn forces 1 but since we haven't got an export 1 yet the lookup
-- fails. However, if Foo(%) is lazy then we don't ask the category any
-- questions and we get to create blob and 1 in peace.
OneDom:with
{
   one: () -> %;
   1:   %;

   default { one():% == 1; }
}
== add
{
   blob(x:Foo(%)):() == foo x;
   1:% == (true\$Boolean) pretend %;
}


-- Bar is a property we can ask domains about.
define Bar:Category == with;


#if LAZY
-- Lazy: build first, ask questions later.
Foo(T:Type):with { foo: % -> (); } == add
{
   local tmp:Boolean := T has Bar;
   foo(x:%):() == { free tmp:Boolean; if tmp then {}; }
}
#elseif EAGER
-- Eager: build and ask questions immediately.
Foo(T:Type):with { foo: % -> (); } ==
{
   local tmp:Boolean := T has Bar;


   -- This may look silly but imagine that we have a select
   -- on a property of T and return a different domain.
   add {
      foo(x:%):() == { free tmp:Boolean; if tmp then {}; }
   }
}
#else
#error "Please compile with -DLAZY or -DEAGER"
#endif


-- Test function: forces OneDom which builds Foo(OneDom). This works
-- provided that Foo(OneDom) isn't eager or doesn't ask questions.
main():() ==
{
   -- Build OneDom.
   local var:OneDom := 1;


   -- Ensure that dead code elimination doesn't sweep "var" away.
   import { sink: OneDom -> (); } from Foreign C;
   sink(var);
}


-- Start the ball rolling.
main();
EOF


# Compile the runtime system.
echo "== (Compiling the runtime system)"
unicl -c rtime.c


# Test the lazy Aldor version.
echo "== (Lazy version at -Q1)"
aldor $LM -Mno-ALDOR_W_CantUseArchive -grun -Q1 -D LAZY eager0.as rtime.$OBJ

echo "== (Lazy version at -Q3)"
aldor $LM -Mno-ALDOR_W_CantUseArchive -grun -Q3 -D LAZY eager0.as rtime.$OBJ


# Test the eager Aldor version.
echo "== (Eager version at -Q1)"
aldor $LM -Mno-ALDOR_W_CantUseArchive -grun -Q1 -D EAGER eager0.as rtime.$OBJ


echo "== (Eager version at -Q3)"
aldor $LM -Mno-ALDOR_W_CantUseArchive -grun -Q3 -D EAGER eager0.as rtime.$OBJ


# Cleaning up
echo "== (Cleaning up)"
# rm -f eager0* rtime*

