-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -M no-ALDOR_W_FunnyEscape -l axllib

#pile
#include "axllib"
#library FormatLib "fmtout.ao"
import from FormatLib;

import from
   DoubleFloat
   Float
   SingleInteger
   Integer
   NumberScanPackage DoubleFloat
   NumberScanPackage Float
   NumberScanPackage SingleInteger
   NumberScanPackage Integer
   String

df1 : DoubleFloat := 34.0
df1_1 : DoubleFloat := scanNumber " 34.0"
df1_2 : DoubleFloat := scanNumber "34.0"
df2 : DoubleFloat := scanNumber "34.3"
df3 : DoubleFloat := - 34.0
df3_1 : DoubleFloat := scanNumber "- 34.0"
df3_2 : DoubleFloat := scanNumber " - 34.0"

print << "DoubleFloat 34.0: " << df1 << newline
print << "DoubleFloat _" 34.0_": " << df1_1 << newline
print << "DoubleFloat _"34.0_": " << df1_2 << newline
print << "DoubleFloat _"34.3_": " << df2 << newline
print << "DoubleFloat - 34.0: " << df3 << newline
print << "DoubleFloat _"- 34.0_": " << df3_1 << newline
print << "DoubleFloat _" - 34.0_": " << df3_2 << newline

f1 : Float := 34.0
f1_1 : Float := scanNumber " 34.0"
f1_2 : Float := scanNumber "34.0"
f1_3 : Float := scanNumber "34.3"
f3 : Float := - 34.0
f3_1 : Float := scanNumber "- 34.0"
f3_2 : Float := scanNumber " - 34.0"

print << "Float 34.0: " << f1 << newline
print << "Float _" 34.0_": " << f1_1 << newline
print << "Float _"34.0_": " << f1_2 << newline
print << "Float _"34.3_": " << f1_3 << newline
print << "Float - 34.0: " << f3 << newline
print << "Float _"- 34_": " << f3_1 << newline
print << "Float _" - 34_": " << f3_2 << newline

si1 : SingleInteger := 24
si2_1 : SingleInteger := scanNumber " 24"
si2_2 : SingleInteger := scanNumber "24 "
si3 : SingleInteger := - 34
si3_1 : SingleInteger := scanNumber "- 34"
si3_2 : SingleInteger := scanNumber " - 34"

print << "SingleInteger 24: " << si1 << newline
print << "SingleInteger _" 24_": " << si2_1 << newline
print << "SingleInteger _"24 _": " << si2_2 << newline
print << "SingleInteger - 34: " << si3 << newline
print << "SingleInteger _"- 34_": " << si3_1 << newline
print << "SingleInteger _" - 34_": " << si3_2 << newline

i1 : Integer := 24
i2_1 : Integer := scanNumber " 24"
i2_2 : Integer := scanNumber "24 "
i3 : Integer := - 34
i3_1 : Integer := scanNumber "- 34"
i3_2 : Integer := scanNumber " - 34"

print << "Integer 24: " << i1 << newline
print << "Integer _" 24_": " << i2_1 << newline
print << "Integer _"24 _": " << i2_2 << newline
print << "Integer - 34: " << i3 << newline
print << "Integer _"- 34_": " << i3_1 << newline
print << "Integer _" - 34_": " << i3_2 << newline
