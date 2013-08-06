-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile
#include "axllib"
#library FormatLib "fmtout.ao"
import from FormatLib;

import from FormattedOutput, String, FileName, SingleInteger, DoubleFloat;

print."This is my~ntry~n"()
print."This is my ~1-th try~n"(<<99)
print."This is my ~2-th ~1. it feels like ~a or ~a ~~ ~n"(<<"TRY",<<100,<<1000)

s := string."This is a funny string containing ~~ and `~1'!!"(<<"stuff")
print."The string contains _"~a_"~n"(<<s)

n := count.">~a<"(<<100)
print.">~a< has ~a characters~n"(<<100, <<n)


#if DoFilesToo
file: OutFile := open parse "Here"
fout:= file::TextWriter
print("The numbers are ~{10}a and ~{2}a ~n", fout) (
       <<1.0,<<2.2,<<3,<<4,<<5,<<6,<<7,<<8,<<9,<<10,<<11,<<12
 );
close file
#endif
