-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib

#include "axllib"

-- Test the behavior with respect to doubles:

DF ==> DoubleFloat;
SI ==> SingleInteger;

foo(): () == {
   import from Format;
   local {
	s: String;
	(df1, df2): DF;
	i:  SI;
   };

   df1 := -33213.4542545345434998;
   df2 :=  6.7000000000000002;
   s := new(100);
   i := format(df1, s, 1);

   print<<newline;
   print<<"df1 := -33213.4542545345434998"<<newline;
   print<<"unformatted output:"<<newline;
   print<<"df1 = "<<df1<<newline;
   print<<"formatted output:"<<newline;
   print<<" s  = "<<s<<" i = "<<i<<newline;
   print<<newline;

   s := new(100);
   i := format(df2, s, 1);

   print<<newline;
   print<<"df2 :=  6.7000000000000002"<<newline;
   print<<"unformatted output:"<<newline;
   print<<"df2 = "<<df2<<newline;
   print<<"formatted output:"<<newline;
   print<<" s  = "<<s<<" i = "<<i<<newline;
   print<<newline;

   s := "-33213.4542545345434998";
   (df1, i) := scan(s, 1);

   print<<newline;
   print<<"df1 := scan _"-33213.4542545345434998_""<<newline;
   print<<"unformatted output:"<<newline;
   print<<"df1 = "<<df1<<newline;
   print<<"scanned string:"<<newline;
   print<<" s  = "<<s<<" i = "<<i<<newline;
   print<<newline;
};

foo();

