#!/bin/sh

if [ "`platform|sed -e "s/.*msvc.*/y/g"`" = "y" ]; then
	ALDOR=aldor.sh
	LM=
else
	ALDOR=aldor
	LM=-lm
fi

cd $TMPDIR

# Extension of OpenSegment(S) and ClosedSegment(S).
cat > segx0.as << EOF
#include "axllib"

extend OpenSegment(S:Steppable):with
{
	if ((S has Order) and (S has EuclideanDomain)) then
	{
		case: (S, %) -> Boolean;
			++ Support for exotic select statements:
			++    select i in {
			++       (-1 .. by -1)	=> "negative";
			++       10 .. by 2	=> "big and even";
			++       11 .. by 2	=> "big and odd";
			++       "small";
			++    }
	}
}
== add
{
	if ((S has Order) and (S has EuclideanDomain)) then
	{
		local inrange?(s:S, lo:S, st:S):Boolean ==
		{
			-- (lo .. by st) where st is positive.
			assert(st >= 0);


			-- Eliminate the stupid case.
			(st = 0) => (s = lo);


			-- Normal single-steps are trivial.
			(st = 1) => (s >= lo);


			-- Avoid division if possible.
			(s < lo) => false;


			-- We have a value that lies within the limits
			-- of the segment but is it in the range?
			(((s - lo) rem st) = 0);
		}


		case(s:S, x:%):Boolean ==
		{
			import from Segment S;

			-- Extract useful information.
			local lo:S == low(x::Segment(S));
			local st:S == step(x::Segment(S));


			-- Want to compare with positive steps.
			(st < 0) => inrange?(-s, -lo, -st);
			inrange?(s, lo, st);
		}
	}
}


extend ClosedSegment(S:Steppable):with
{
	if ((S has Order) and (S has EuclideanDomain)) then
	{
		case: (S, %) -> Boolean;
			++ Support for exotic select statements:
			++    select i in {
			++       1..10		=> "small";
			++       10 .. 20 by 2	=> "medium and even";
			++       11 .. 20 by 2	=> "medium and odd";
			++       "unclassified";
			++    }

	}
}
== add
{
	if ((S has Order) and (S has EuclideanDomain)) then
	{
		local inrange?(s:S, lo:S, hi:S, st:S):Boolean ==
		{
			-- (lo .. hi by st) where st is positive.
			assert(st >= 0);


			-- Eliminate the stupid case.
			(st = 0) => (s = lo);


			-- Normal single-steps are trivial.
			(st = 1) => (lo <= s <= hi);


			-- Avoid division if possible.
			((s < lo) or (s > hi)) => false;


			-- We have a value that lies within the limits
			-- of the segment but is it in the range?
			(((s - lo) rem st) = 0);
		}


		case(s:S, x:%):Boolean ==
		{
			import from Segment S;

			-- Extract useful information.
			local lo:S == low(x::Segment(S));
			local hi:S == high(x::Segment(S));
			local st:S == step(x::Segment(S));


			-- Want to compare with positive steps.
			(st < 0) => inrange?(-s, -lo, -hi, -st);
			inrange?(s, lo, hi, st);
		}
	}
}
EOF

# Extension of OpenSegment(S) and ClosedSegment(S).
cat > segx1.as << EOF
#include "axllib"

#library LL "segx0.ao"
import from LL;
inline from LL;

extend Segment(S:Steppable):with
{
	if ((S has Order) and (S has EuclideanDomain)) then
	{
		case: (S, %) -> Boolean;
			++ Support for exotic select statements:
			++    select i in {
			++       1..10		=> "small";
			++       10 .. 20 by 2	=> "medium and even";
			++       11 .. 20 by 2	=> "medium and odd";
			++       "unclassified";
			++    }

	}
}
== add
{
	if ((S has Order) and (S has EuclideanDomain)) then
	{
		case(s:S, x:%):Boolean ==
		{
			import from OpenSegment(S), ClosedSegment(S);
			open? x => case(s, x pretend OpenSegment(S));
			case(s, x pretend ClosedSegment(S));
		}
	}
}
EOF

# Test file
cat > sel0.as << EOF
#include "axllib"

#library L1 "segx0.ao"
#library L2 "segx1.ao"
import from L1, L2;
inline from L1, L2;


foo(i:SingleInteger):String ==
{
   import from Segment(SingleInteger);
   import from OpenSegment(SingleInteger);
   import from ClosedSegment(SingleInteger);

   select i in
   {
      0			=> "zero";
      1..9		=> "small";
      (-2 .. by -2)	=> "negative and even";
      (-1 .. by -2)	=> "negative and odd";
      10 .. by 2	=> "big and even";
      11 .. by 2	=> "big and odd";
      "unclassified";
   }
}


main():() ==
{
   import from SingleInteger;
   for i in -5..15 repeat
	   print << i << ": " << foo(i) << newline;
}

main();
EOF

# Compile all this together.
aldor -Fao -Q2 segx0.as
aldor -Fao -Q2 segx1.as
aldor -Fao -Q2 sel0.as


# Test under the interpreter.
aldor -Ginterp -Q2 -laxllib -Mno-ALDOR_W_CantUseArchive $LM sel0.ao segx0.ao segx1.ao

# Separate the output
echo ""
echo ""

# Test natively.
aldor -Grun -Q2 -laxllib -Mno-ALDOR_W_CantUseArchive $LM sel0.ao segx0.ao segx1.ao

