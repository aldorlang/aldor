------------------------------ session3.as --------------------------------
--
-- Third interactive session of "First Course on Aldor and Aldorlib"
--

#include "aldor"
#include "aldorinterp"

-- A record is a type, just like any other.
import from MachineInteger, Record(name: String, age: MachineInteger);

-- square brackets create a new object
newBod := ["Ethel T. Aardvark", 21];

-- record elements are referenced by function application.
-- normally, a dot is used to ensure that the call parses as expected.
newBod.name
newBod.age
newBod(age)
newBod age

-- Records can be updated too:
-- Here the left side of the := is a function application.
newBod.age := newBod.age + 1;

apply(newBod, age)
set!(newBod, age, 21)

import from Union(s: String, i: MachineInteger);

-- create one of these chaps 
u := union(12);

-- or you can use brackets
v := ["Hello"]

-- we can test which branch an object is a member of:
if u case s then stdout << "I'm a string with value: " << u.s;

-- we can destructively update the contents of a union
u.s := "A string";

-- and check what it is again:
if u case s then stdout << "I'm a string with value: " << u.s;

import from 'red, green, blue';
x := red;
if x = green then stdout << "Strewth";

import from List MachineInteger, 'first, second';

element(l: List MachineInteger, tag: 'first, second'):MachineInteger == {
	tag = first => first l;
	tag = second => first rest l;
	never;
}

-- called with:
element([1,2,3], second);

-- also, we can use the apply notation:
apply(l:List MachineInteger,tag:'first,second'):MachineInteger==element(l,tag);

-- can be called with
apply([1,2,3], second)

-- or 
[1,2,3].second

