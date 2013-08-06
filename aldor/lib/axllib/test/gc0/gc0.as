
-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -Q3 -l axllib

#include "axllib"

SI ==> SingleInteger;


-- Object types are a very scare commodity. At the time this test
-- was constructed there were 32 possible types and the first 28
-- are used by the compiler. Since this program may be run from
-- the interpreter we cannot reuse an existing object type.
FooObjType ==> 31;


-- A simple domain to trace. Try setting GC_CLASSIFY before
-- running this test and compare what happens when the Aldor
-- tracer is not registered. For domains with large reps and
-- few internal pointers the difference may be significant.
Foo : with
{
   new: SI -> %;
   foo: % -> %;
   bar: % -> %;
   sho: % -> ();
}
== add
{
   -- This is a very contrived domain - a kind of linked list
   -- with a tag to mark the end of the list. If the tag is
   -- a positive integer then it is data; if it is zero or
   -- negative then it marks the end of the list.
   Rep == Record(x:SI, y:%);

   import from SI, Boolean, Rep;
   import from Machine;
   import from Record(code:SI, hasPtrs:Boolean);


   -- Grab various storage manager functions. Note that these
   -- functions may not do anything on all platforms.
   import
   {
      StoRecode:     (Rep, SI) -> %;
      StoNewObject:  (SI, Boolean) -> ();
      StoMarkObject: % -> SInt;
      StoATracer:    (SI, (SI, SI) -> SInt) -> ();
   } from Builtin;


   -- This function might be invoked by the garbage collector
   -- when it wants to mark objects of type FooObjType. All we
   -- do is call StoMarkObject on anything that might be a
   -- pointer. The penalty for missing a pointer is a painful
   -- death - the collector may recycle some of our data.
   FooTracer(p:SI, ignored:SI):SInt ==
   {
      local tmp:Rep := p pretend Rep;
      (tmp.x > 0) =>
      {
         -- Some debugging information
         -- print << "*** Marking " << ((tmp.y) pretend SI) << newline;


         -- Printing the address doesn't help the test suite ...
         print << "*** Marking " << newline;


         -- Ask the system to mark this (live) object.
         StoMarkObject(tmp.y);
      }
      0;
   }


   -- New object types MUST be registered before the tracer. We
   -- ought to create a StoInfo domain but can't be bothered.
   StoNewObject(FooObjType, true);


   -- Register FooTracer as a function for marking objects of
   -- type FooObjType. The collector may ignore this request.
   StoATracer(FooObjType, FooTracer);


   -- Create a new record and change its object type
   local newterm(a:SI, b:%):% ==
      StoRecode([a, b], FooObjType);


   -- Wacky constructor
   new(n:SI):% ==
   {
      local result:% := newterm(0, (0@SInt) pretend %);

      for i in 1..n repeat
         result := newterm(i, result);

      result;
   }


   -- Reduce the length of the list
   foo(p:%):% ==
   {
      local result:Rep := rep p;

      for i in 1..((result.x) quo 2) repeat
         result := (result.y) pretend Rep;

      per result;
   }


   -- Increase the length of the list
   bar(p:%):% ==
   {
      new(((rep p).x) * 2);
   }


   -- Display the list
   sho(p:%):() ==
   {
      local result:Rep := rep p;

      print << "{";
      while (result.x > 0) repeat
      {
         print << (result.x);
         result := (result.y) pretend Rep;
      }
      print << "}";

   }
}


tester():() ==
{
   import from Foo, SI;
   local xx:Foo := new(15);


   -- Display the possible arguments to StoShowArgs
   StoShowArgs("show");


   -- Grab various storage manager functions. Note that these
   -- functions may not do anything on all platforms.
   import
   {
      StoShow:       SI -> ();
      StoShowArgs:   String -> SI;
   } from Builtin;


   -- Quick check that everything is okay.
   print << "x(15) = "; sho(xx); print << newline;


   -- Display the current store
   -- StoShow(StoShowArgs("all"));


   -- Strange manipulations - the list length remains the
   -- same but we generate lots of store allocations.
   for ii in 1..10000 repeat
   {
      xx := foo foo bar bar xx;
   }


   -- Display the final store
   -- StoShow(StoShowArgs("all"));


   -- Show the result
   print << "x(15) = "; sho(xx); print << newline;
}


tester();


