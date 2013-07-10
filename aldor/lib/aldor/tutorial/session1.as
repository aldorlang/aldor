------------------------------ session1.as --------------------------------
--
-- First interactive session of "First Course on Aldor and Aldorlib"
--


-- basic include for all aldorlib clients
#include "aldor"

-- include the following only when using an interactive session
#include "aldorinterp"

"hello"

1

import from Integer

1

123456789*98765432

-- The normal precedence rules apply for infix operators:
4 + 3 * 2

-- Parentheses are used for grouping
2*(3+4)

-- Function calls can be written with parentheses around the arguments
next(10)

-- Where there is a single argument, the parentheses are optional, and
-- aguments associate to the right.  
next next 10

-- Application has higher precedence than all arithmetic operators
next 2 * next 3

-- Blocks are sequences of expressions enclosed in braces.  Normally
-- each expression is evaluated in order with the last value returned.
{2; 3; 4; 5}

-- A "fat arrow" expression can cause an early abnormal exit.
{2 < 3 => 4; 5}

-- The expression "a => b" means "if a then exit (with value) b".
(2 < 1 => 4; 5)

-- You can also write the previous two expressions as "conditionals".
if 2 < 3 then 4 else 5
if 2 < 1 then 4 else 5

-- You can store values into a local variable using infix :=.
x := 2 + 2

-- The value of the local variable can be used in other expressions.
y := {2 < x => 7; 11}

-- There are practically no restrictions as to what kinds of 
-- expressions can be nested within others.
(y := 2) + (2 < 3 => 7; 11) * (if 2 < 1 then (w := 4) else 5)

-- Printing is done using "stdout" with the << operator.
stdout << "The value of x + y is " << x + y

-- Printing "newline" causes printing to begin on the next line.
stdout << "The value is " << (if x < y then 2+2 else 2+3) << newline

-- Functions are created using infix +->.
-- You must supply the parameter and target types.
increment := (x: Integer): Integer +-> x + 1

-- A function can be stored in a local variable like any other value.
increment 2

-- The arguments supplied to a function must have the correct type.
increment "hello"

-- The usual way to define a function is using an ==.
inc(x: Integer): Integer == x + 1
inc 2

-- The == signifies that "inc" is a constant (and should not be changed).
-- Reply 'n' when asked whether to redefine inc.
inc(x: Integer): Integer == x + 2

-- Remember that function application has higher precedence
-- than arithmetic operations.
inc 2*3

-- Define the factorial function.
fact(n: Integer): Integer == {
	n < 2 => 1;
	n * fact (n - 1)
}
fact 30

incr(x: Integer, amount: Integer == 1): Integer == x + amount;
incr(12)
incr(13,7)

-- Now for some fun with lists.
import from List Integer

-- The bracket operation ('[' and ']') creates a list from its
-- elements.  The operation is imported from List, not a builtin
-- function.  
[1, 3, 5, 7, 9, 11]

-- Another way to create the same list is to use a loop.
[i for i in 1..11 by 2]

-- Loops can count backwards as well.
[2*i for i in 11..1 by -2]

-- Another way is to use a "such that" clause (introduced by '|').
[2*i for i in 11..1 by -1 | odd? i]

-- Let us bring machine integers and lists of them in scope too,
-- this will create some ambiguity for integer constants!
import from MachineInteger, List MachineInteger;

-- Since both MachineInteger and Integer are in now scope
-- the expression -1 could mean two different things.
k := -1

-- You can specify which value you mean using a declaration.
k: MachineInteger := -1

-- Another way to specify is to restrict an expression (using '@')
-- to a particular type.
m := (-1 @ Integer)

-- A while construct tests before a value is collected (ignore the warning)
u := [(k := k + 2) while k < 11]

-- You can also iterate over a list-valued expression.
v := [z for z in u | z < 9]

-- To extract an element of a list, apply the list to an index.
ww := [u.i for i in 1..5@MachineInteger by 2]

-- the "@MachineInteger" indicates that the integer 5 should be 
-- treated as a MachineInteger.
-- This can be avoided with declaring i to have a default type
default i: MachineInteger

-- Two for-constructors can be given in parallel.
[i*j for i in 1..10 for j in u]

-- In general, any number of constructs can be given in parallel.
-- The following runs over the 1, 3, 5, 7, 9 in parallel with
-- the values in u, collecting the products p of the
-- corresponding elements until p exceeds 24.
[p for i in 1..10 | odd? i for j in u while (p := i*j) < 24]

-- Constructs may be arbitrarily nested but you must import
-- what you need.
import from List List MachineInteger

-- Now we can form lists of lists of integers.
[[i*j for i in 1..2] for j in u]

-- Display the even-numbered integers between -5 and +5
for i in -5..5 | even? i repeat stdout << i << space

-- To go on to the next iteration, use "iterate"
for i in -5..5 repeat {
	odd? i => iterate;
	stdout << i << space
}

-- To exit a loop early, use "break"
for i in -5.. repeat {
	odd? i => iterate;
	i > 5 => break;
	stdout << i << space
}

-- As with collections, iterators can use while
{ x:= 0;
	while x < 10 repeat { stdout << x << " "; x := x+1 }
	stdout << newline }

-- without any guards, repeat will iterate forever
repeat { stdout << "Hit Ctrl-C to stop " << newline }

l: List MachineInteger := [i for i in 1..10];
for i in l repeat { stdout << i << space }

import from Array MachineInteger;
a: Array MachineInteger := [i for i in 1..10];
for i in a repeat { stdout << i << space }

both(l1:List Integer, l2:List Integer):Generator Integer == {
	generate {
		for s1 in l1 for s2 in l2 repeat {
			yield s1;
			yield s2;
		}
	}
}

-- Something simple:
for z in both([1,2,3,4,5],[5,4,3,2,1]) repeat { stdout << z << space }

-- we can build the combined list using a collection:
[s for s in both([1, 2], [10, 3])]

filter(f: MachineInteger -> Boolean,
	g: Generator MachineInteger): Generator MachineInteger == {
		generate {
			for z in g repeat { if f(z) then yield z }
		}
}

for z in filter(odd?, i*i for i in -10..10) repeat {
	stdout << z << space }

-- Produces low, low+1,...,high - 1, high,
--          high-1, ..., low + 1, low, low + 1, etc...
upAndDown(low: MachineInteger,
	hi: MachineInteger): Generator MachineInteger == generate {
		repeat {
			for z in low..hi repeat yield z;
			for z in (hi-1)..(low+1) by -1 repeat yield z;
		}
}

for i in 1..10 for z in filter(even?, upAndDown(1,10)) repeat {
	stdout << z << space }

-- Declaring a variable to be of a type T automatically imports from T
evens:Stream Integer := [n for n in 1@Integer .. | even? n]

-- Let's check the 6th positive even integer
evens.5

-- See all the elements of evens that have been computed so far
evens

-- Given that s is a stream storing the Fibonacci numbers up to index n-1,
-- this function computes the Fibonacci number of index n
genfib(n:MachineInteger, s:Stream Integer):Integer == {
        n = 0 or n = 1 => 1;
        s(n-1) + s(n-2);
}

-- This creates the stream of all the Fibonacci numbers
fib := stream genfib;

-- Let's check the 15-th Fibonnaci number
fib 14

-- See all the elements of fib that have been computed so far
fib

-- Streams can be iterated like any other data structure
-- but make sure to add another clause to avoid infinite loops!
for n in fib while n < 100 repeat stdout << n << space

