-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testgen c
#pile

#include "axllib.as"

local z00: Boolean                         -- zb
local z00: Integer                         -- zi

z00: Boolean == false
z00: Integer == 0


local left00: (Integer, Boolean) -> Integer   -- lib-i
local left00: (Boolean, Boolean) -> Boolean   -- lbb-b
local left00: (Boolean, Integer) -> Boolean   -- lbi-b

left00(i: Integer, b: Boolean): Integer == i
left00(b1:Boolean, b2:Boolean): Boolean == b1
left00(b: Boolean, i: Integer): Boolean == b


local f00: (Integer, Boolean) -> Integer

f00(i: Integer, b: Boolean): Integer ==
	b := left00(z00, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left00(z00, b)		   -- same but choose i
	b := left00(left00(z00, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left00(left00(z00, b), b)     -- same but choose i

local z01: Boolean                         -- zb
local z01: Integer                         -- zi

z01: Boolean == false
z01: Integer == 0


local left01: (Integer, Boolean) -> Integer   -- lib-i
local left01: (Boolean, Boolean) -> Boolean   -- lbb-b
local left01: (Boolean, Integer) -> Boolean   -- lbi-b

left01(i: Integer, b: Boolean): Integer == i
left01(b1:Boolean, b2:Boolean): Boolean == b1
left01(b: Boolean, i: Integer): Boolean == b


local f01: (Integer,Boolean) -> Integer

f01(i: Integer, b: Boolean): Integer ==
	b := left01(z01, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left01(z01, b)		   -- same but choose i
	b := left01(left01(z01, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left01(left01(z01, b), b)     -- same but choose i

local z02: Boolean                         -- zb
local z02: Integer                         -- zi

z02: Boolean == false
z02: Integer == 0


local left02: (Integer, Boolean) -> Integer   -- lib-i
local left02: (Boolean, Boolean) -> Boolean   -- lbb-b
local left02: (Boolean, Integer) -> Boolean   -- lbi-b

left02(i: Integer, b: Boolean): Integer == i
left02(b1:Boolean, b2:Boolean): Boolean == b1
left02(b: Boolean, i: Integer): Boolean == b


local f02: (Integer,Boolean) -> Integer

f02(i: Integer, b: Boolean): Integer ==
	b := left02(z02, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left02(z02, b)		   -- same but choose i
	b := left02(left02(z02, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left02(left02(z02, b), b)     -- same but choose i

local z03: Boolean                         -- zb
local z03: Integer                         -- zi

z03: Boolean == false
z03: Integer == 0


local left03: (Integer, Boolean) -> Integer   -- lib-i
local left03: (Boolean, Boolean) -> Boolean   -- lbb-b
local left03: (Boolean, Integer) -> Boolean   -- lbi-b

left03(i: Integer, b: Boolean): Integer == i
left03(b1:Boolean, b2:Boolean): Boolean == b1
left03(b: Boolean, i: Integer): Boolean == b


local f03: (Integer,Boolean) -> Integer

f03(i: Integer, b: Boolean): Integer ==
	b := left03(z03, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left03(z03, b)		   -- same but choose i
	b := left03(left03(z03, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left03(left03(z03, b), b)     -- same but choose i

local z04: Boolean                         -- zb
local z04: Integer                         -- zi

z04: Boolean == false
z04: Integer == 0


local left04: (Integer, Boolean) -> Integer   -- lib-i
local left04: (Boolean, Boolean) -> Boolean   -- lbb-b
local left04: (Boolean, Integer) -> Boolean   -- lbi-b

left04(i: Integer, b: Boolean): Integer == i
left04(b1:Boolean, b2:Boolean): Boolean == b1
left04(b: Boolean, i: Integer): Boolean == b


local f04: (Integer,Boolean) -> Integer

f04(i: Integer, b: Boolean): Integer ==
	b := left04(z04, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left04(z04, b)		   -- same but choose i
	b := left04(left04(z04, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left04(left04(z04, b), b)     -- same but choose i

local z05: Boolean                         -- zb
local z05: Integer                         -- zi

z05: Boolean == false
z05: Integer == 0


local left05: (Integer, Boolean) -> Integer   -- lib-i
local left05: (Boolean, Boolean) -> Boolean   -- lbb-b
local left05: (Boolean, Integer) -> Boolean   -- lbi-b

left05(i: Integer, b: Boolean): Integer == i
left05(b1:Boolean, b2:Boolean): Boolean == b1
left05(b: Boolean, i: Integer): Boolean == b


local f05: (Integer,Boolean) -> Integer

f05(i: Integer, b: Boolean): Integer ==
	b := left05(z05, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left05(z05, b)		   -- same but choose i
	b := left05(left05(z05, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left05(left05(z05, b), b)     -- same but choose i

local z06: Boolean                         -- zb
local z06: Integer                         -- zi

z06: Boolean == false
z06: Integer == 0


local left06: (Integer, Boolean) -> Integer   -- lib-i
local left06: (Boolean, Boolean) -> Boolean   -- lbb-b
local left06: (Boolean, Integer) -> Boolean   -- lbi-b

left06(i: Integer, b: Boolean): Integer == i
left06(b1:Boolean, b2:Boolean): Boolean == b1
left06(b: Boolean, i: Integer): Boolean == b


local f06: (Integer,Boolean) -> Integer

f06(i: Integer, b: Boolean): Integer ==
	b := left06(z06, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left06(z06, b)		   -- same but choose i
	b := left06(left06(z06, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left06(left06(z06, b), b)     -- same but choose i

local z07: Boolean                         -- zb
local z07: Integer                         -- zi

z07: Boolean == false
z07: Integer == 0


local left07: (Integer, Boolean) -> Integer   -- lib-i
local left07: (Boolean, Boolean) -> Boolean   -- lbb-b
local left07: (Boolean, Integer) -> Boolean   -- lbi-b

left07(i: Integer, b: Boolean): Integer == i
left07(b1:Boolean, b2:Boolean): Boolean == b1
left07(b: Boolean, i: Integer): Boolean == b


local f07: (Integer,Boolean) -> Integer

f07(i: Integer, b: Boolean): Integer ==
	b := left07(z07, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left07(z07, b)		   -- same but choose i
	b := left07(left07(z07, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left07(left07(z07, b), b)     -- same but choose i
local z08: Boolean                         -- zb
local z08: Integer                         -- zi

z08: Boolean == false
z08: Integer == 0


local left08: (Integer, Boolean) -> Integer   -- lib-i
local left08: (Boolean, Boolean) -> Boolean   -- lbb-b
local left08: (Boolean, Integer) -> Boolean   -- lbi-b

left08(i: Integer, b: Boolean): Integer == i
left08(b1:Boolean, b2:Boolean): Boolean == b1
left08(b: Boolean, i: Integer): Boolean == b


local f08: (Integer,Boolean) -> Integer

f08(i: Integer, b: Boolean): Integer ==
	b := left08(z08, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left08(z08, b)		   -- same but choose i
	b := left08(left08(z08, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left08(left08(z08, b), b)     -- same but choose i
local z09: Boolean                         -- zb
local z09: Integer                         -- zi

z09: Boolean == false
z09: Integer == 0


local left09: (Integer, Boolean) -> Integer   -- lib-i
local left09: (Boolean, Boolean) -> Boolean   -- lbb-b
local left09: (Boolean, Integer) -> Boolean   -- lbi-b

left09(i: Integer, b: Boolean): Integer == i
left09(b1:Boolean, b2:Boolean): Boolean == b1
left09(b: Boolean, i: Integer): Boolean == b


local f09: (Integer,Boolean) -> Integer

f09(i: Integer, b: Boolean): Integer ==
	b := left09(z09, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left09(z09, b)		   -- same but choose i
	b := left09(left09(z09, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left09(left09(z09, b), b)     -- same but choose i

local z10: Boolean                         -- zb
local z10: Integer                         -- zi

z10: Boolean == false
z10: Integer == 0


local left10: (Integer, Boolean) -> Integer   -- lib-i
local left10: (Boolean, Boolean) -> Boolean   -- lbb-b
local left10: (Boolean, Integer) -> Boolean   -- lbi-b

left10(i: Integer, b: Boolean): Integer == i
left10(b1:Boolean, b2:Boolean): Boolean == b1
left10(b: Boolean, i: Integer): Boolean == b


local f10: (Integer,Boolean) -> Integer

f10(i: Integer, b: Boolean): Integer ==
	b := left10(z10, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left10(z10, b)		   -- same but choose i
	b := left10(left10(z10, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left10(left10(z10, b), b)     -- same but choose i

local z11: Boolean                         -- zb
local z11: Integer                         -- zi

z11: Boolean == false
z11: Integer == 0


local left11: (Integer, Boolean) -> Integer   -- lib-i
local left11: (Boolean, Boolean) -> Boolean   -- lbb-b
local left11: (Boolean, Integer) -> Boolean   -- lbi-b

left11(i: Integer, b: Boolean): Integer == i
left11(b1:Boolean, b2:Boolean): Boolean == b1
left11(b: Boolean, i: Integer): Boolean == b


local f11: (Integer,Boolean) -> Integer

f11(i: Integer, b: Boolean): Integer ==
	b := left11(z11, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left11(z11, b)		   -- same but choose i
	b := left11(left11(z11, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left11(left11(z11, b), b)     -- same but choose i

local z12: Boolean                         -- zb
local z12: Integer                         -- zi

z12: Boolean == false
z12: Integer == 0


local left12: (Integer, Boolean) -> Integer   -- lib-i
local left12: (Boolean, Boolean) -> Boolean   -- lbb-b
local left12: (Boolean, Integer) -> Boolean   -- lbi-b

left12(i: Integer, b: Boolean): Integer == i
left12(b1:Boolean, b2:Boolean): Boolean == b1
left12(b: Boolean, i: Integer): Boolean == b


local f12: (Integer,Boolean) -> Integer

f12(i: Integer, b: Boolean): Integer ==
	b := left12(z12, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left12(z12, b)		   -- same but choose i
	b := left12(left12(z12, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left12(left12(z12, b), b)     -- same but choose i

local z13: Boolean                         -- zb
local z13: Integer                         -- zi

z13: Boolean == false
z13: Integer == 0


local left13: (Integer, Boolean) -> Integer   -- lib-i
local left13: (Boolean, Boolean) -> Boolean   -- lbb-b
local left13: (Boolean, Integer) -> Boolean   -- lbi-b

left13(i: Integer, b: Boolean): Integer == i
left13(b1:Boolean, b2:Boolean): Boolean == b1
left13(b: Boolean, i: Integer): Boolean == b


local f13: (Integer,Boolean) -> Integer

f13(i: Integer, b: Boolean): Integer ==
	b := left13(z13, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left13(z13, b)		   -- same but choose i
	b := left13(left13(z13, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left13(left13(z13, b), b)     -- same but choose i

local z14: Boolean                         -- zb
local z14: Integer                         -- zi

z14: Boolean == false
z14: Integer == 0


local left14: (Integer, Boolean) -> Integer   -- lib-i
local left14: (Boolean, Boolean) -> Boolean   -- lbb-b
local left14: (Boolean, Integer) -> Boolean   -- lbi-b

left14(i: Integer, b: Boolean): Integer == i
left14(b1:Boolean, b2:Boolean): Boolean == b1
left14(b: Boolean, i: Integer): Boolean == b


local f14: (Integer,Boolean) -> Integer

f14(i: Integer, b: Boolean): Integer ==
	b := left14(z14, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left14(z14, b)		   -- same but choose i
	b := left14(left14(z14, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left14(left14(z14, b), b)     -- same but choose i

local z15: Boolean                         -- zb
local z15: Integer                         -- zi

z15: Boolean == false
z15: Integer == 0


local left15: (Integer, Boolean) -> Integer   -- lib-i
local left15: (Boolean, Boolean) -> Boolean   -- lbb-b
local left15: (Boolean, Integer) -> Boolean   -- lbi-b

left15(i: Integer, b: Boolean): Integer == i
left15(b1:Boolean, b2:Boolean): Boolean == b1
left15(b: Boolean, i: Integer): Boolean == b


local f15: (Integer,Boolean) -> Integer

f15(i: Integer, b: Boolean): Integer ==
	b := left15(z15, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left15(z15, b)		   -- same but choose i
	b := left15(left15(z15, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left15(left15(z15, b), b)     -- same but choose i

local z16: Boolean                         -- zb
local z16: Integer                         -- zi

z16: Boolean == false
z16: Integer == 0


local left16: (Integer, Boolean) -> Integer   -- lib-i
local left16: (Boolean, Boolean) -> Boolean   -- lbb-b
local left16: (Boolean, Integer) -> Boolean   -- lbi-b

left16(i: Integer, b: Boolean): Integer == i
left16(b1:Boolean, b2:Boolean): Boolean == b1
left16(b: Boolean, i: Integer): Boolean == b


local f16: (Integer,Boolean) -> Integer

f16(i: Integer, b: Boolean): Integer ==
	b := left16(z16, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left16(z16, b)		   -- same but choose i
	b := left16(left16(z16, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left16(left16(z16, b), b)     -- same but choose i

local z17: Boolean                         -- zb
local z17: Integer                         -- zi

z17: Boolean == false
z17: Integer == 0


local left17: (Integer, Boolean) -> Integer   -- lib-i
local left17: (Boolean, Boolean) -> Boolean   -- lbb-b
local left17: (Boolean, Integer) -> Boolean   -- lbi-b

left17(i: Integer, b: Boolean): Integer == i
left17(b1:Boolean, b2:Boolean): Boolean == b1
left17(b: Boolean, i: Integer): Boolean == b


local f17: (Integer,Boolean) -> Integer

f17(i: Integer, b: Boolean): Integer ==
	b := left17(z17, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left17(z17, b)		   -- same but choose i
	b := left17(left17(z17, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left17(left17(z17, b), b)     -- same but choose i
local z18: Boolean                         -- zb
local z18: Integer                         -- zi

z18: Boolean == false
z18: Integer == 0


local left18: (Integer, Boolean) -> Integer   -- lib-i
local left18: (Boolean, Boolean) -> Boolean   -- lbb-b
local left18: (Boolean, Integer) -> Boolean   -- lbi-b

left18(i: Integer, b: Boolean): Integer == i
left18(b1:Boolean, b2:Boolean): Boolean == b1
left18(b: Boolean, i: Integer): Boolean == b


local f18: (Integer,Boolean) -> Integer

f18(i: Integer, b: Boolean): Integer ==
	b := left18(z18, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left18(z18, b)		   -- same but choose i
	b := left18(left18(z18, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left18(left18(z18, b), b)     -- same but choose i
local z19: Boolean                         -- zb
local z19: Integer                         -- zi

z19: Boolean == false
z19: Integer == 0


local left19: (Integer, Boolean) -> Integer   -- lib-i
local left19: (Boolean, Boolean) -> Boolean   -- lbb-b
local left19: (Boolean, Integer) -> Boolean   -- lbi-b

left19(i: Integer, b: Boolean): Integer == i
left19(b1:Boolean, b2:Boolean): Boolean == b1
left19(b: Boolean, i: Integer): Boolean == b


local f19: (Integer,Boolean) -> Integer

f19(i: Integer, b: Boolean): Integer ==
	b := left19(z19, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left19(z19, b)		   -- same but choose i
	b := left19(left19(z19, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left19(left19(z19, b), b)     -- same but choose i
local z20: Boolean                         -- zb
local z20: Integer                         -- zi

z20: Boolean == false
z20: Integer == 0


local left20: (Integer, Boolean) -> Integer   -- lib-i
local left20: (Boolean, Boolean) -> Boolean   -- lbb-b
local left20: (Boolean, Integer) -> Boolean   -- lbi-b

left20(i: Integer, b: Boolean): Integer == i
left20(b1:Boolean, b2:Boolean): Boolean == b1
left20(b: Boolean, i: Integer): Boolean == b


local f20: (Integer,Boolean) -> Integer

f20(i: Integer, b: Boolean): Integer ==
	b := left20(z20, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left20(z20, b)		   -- same but choose i
	b := left20(left20(z20, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left20(left20(z20, b), b)     -- same but choose i

local z21: Boolean                         -- zb
local z21: Integer                         -- zi

z21: Boolean == false
z21: Integer == 0


local left21: (Integer, Boolean) -> Integer   -- lib-i
local left21: (Boolean, Boolean) -> Boolean   -- lbb-b
local left21: (Boolean, Integer) -> Boolean   -- lbi-b

left21(i: Integer, b: Boolean): Integer == i
left21(b1:Boolean, b2:Boolean): Boolean == b1
left21(b: Boolean, i: Integer): Boolean == b


local f21: (Integer,Boolean) -> Integer

f21(i: Integer, b: Boolean): Integer ==
	b := left21(z21, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left21(z21, b)		   -- same but choose i
	b := left21(left21(z21, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left21(left21(z21, b), b)     -- same but choose i

local z22: Boolean                         -- zb
local z22: Integer                         -- zi

z22: Boolean == false
z22: Integer == 0


local left22: (Integer, Boolean) -> Integer   -- lib-i
local left22: (Boolean, Boolean) -> Boolean   -- lbb-b
local left22: (Boolean, Integer) -> Boolean   -- lbi-b

left22(i: Integer, b: Boolean): Integer == i
left22(b1:Boolean, b2:Boolean): Boolean == b1
left22(b: Boolean, i: Integer): Boolean == b


local f22: (Integer,Boolean) -> Integer

f22(i: Integer, b: Boolean): Integer ==
	b := left22(z22, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left22(z22, b)		   -- same but choose i
	b := left22(left22(z22, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left22(left22(z22, b), b)     -- same but choose i

local z23: Boolean                         -- zb
local z23: Integer                         -- zi

z23: Boolean == false
z23: Integer == 0


local left23: (Integer, Boolean) -> Integer   -- lib-i
local left23: (Boolean, Boolean) -> Boolean   -- lbb-b
local left23: (Boolean, Integer) -> Boolean   -- lbi-b

left23(i: Integer, b: Boolean): Integer == i
left23(b1:Boolean, b2:Boolean): Boolean == b1
left23(b: Boolean, i: Integer): Boolean == b


local f23: (Integer,Boolean) -> Integer

f23(i: Integer, b: Boolean): Integer ==
	b := left23(z23, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left23(z23, b)		   -- same but choose i
	b := left23(left23(z23, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left23(left23(z23, b), b)     -- same but choose i

local z24: Boolean                         -- zb
local z24: Integer                         -- zi

z24: Boolean == false
z24: Integer == 0


local left24: (Integer, Boolean) -> Integer   -- lib-i
local left24: (Boolean, Boolean) -> Boolean   -- lbb-b
local left24: (Boolean, Integer) -> Boolean   -- lbi-b

left24(i: Integer, b: Boolean): Integer == i
left24(b1:Boolean, b2:Boolean): Boolean == b1
left24(b: Boolean, i: Integer): Boolean == b


local f24: (Integer,Boolean) -> Integer

f24(i: Integer, b: Boolean): Integer ==
	b := left24(z24, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left24(z24, b)		   -- same but choose i
	b := left24(left24(z24, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left24(left24(z24, b), b)     -- same but choose i

local z25: Boolean                         -- zb
local z25: Integer                         -- zi

z25: Boolean == false
z25: Integer == 0


local left25: (Integer, Boolean) -> Integer   -- lib-i
local left25: (Boolean, Boolean) -> Boolean   -- lbb-b
local left25: (Boolean, Integer) -> Boolean   -- lbi-b

left25(i: Integer, b: Boolean): Integer == i
left25(b1:Boolean, b2:Boolean): Boolean == b1
left25(b: Boolean, i: Integer): Boolean == b


local f25: (Integer,Boolean) -> Integer

f25(i: Integer, b: Boolean): Integer ==
	b := left25(z25, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left25(z25, b)		   -- same but choose i
	b := left25(left25(z25, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left25(left25(z25, b), b)     -- same but choose i

local z26: Boolean                         -- zb
local z26: Integer                         -- zi

z26: Boolean == false
z26: Integer == 0


local left26: (Integer, Boolean) -> Integer   -- lib-i
local left26: (Boolean, Boolean) -> Boolean   -- lbb-b
local left26: (Boolean, Integer) -> Boolean   -- lbi-b

left26(i: Integer, b: Boolean): Integer == i
left26(b1:Boolean, b2:Boolean): Boolean == b1
left26(b: Boolean, i: Integer): Boolean == b


local f26: (Integer,Boolean) -> Integer

f26(i: Integer, b: Boolean): Integer ==
	b := left26(z26, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left26(z26, b)		   -- same but choose i
	b := left26(left26(z26, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left26(left26(z26, b), b)     -- same but choose i

local z27: Boolean                         -- zb
local z27: Integer                         -- zi

z27: Boolean == false
z27: Integer == 0


local left27: (Integer, Boolean) -> Integer   -- lib-i
local left27: (Boolean, Boolean) -> Boolean   -- lbb-b
local left27: (Boolean, Integer) -> Boolean   -- lbi-b

left27(i: Integer, b: Boolean): Integer == i
left27(b1:Boolean, b2:Boolean): Boolean == b1
left27(b: Boolean, i: Integer): Boolean == b


local f27: (Integer,Boolean) -> Integer

f27(i: Integer, b: Boolean): Integer ==
	b := left27(z27, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left27(z27, b)		   -- same but choose i
	b := left27(left27(z27, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left27(left27(z27, b), b)     -- same but choose i
local z28: Boolean                         -- zb
local z28: Integer                         -- zi

z28: Boolean == false
z28: Integer == 0


local left28: (Integer, Boolean) -> Integer   -- lib-i
local left28: (Boolean, Boolean) -> Boolean   -- lbb-b
local left28: (Boolean, Integer) -> Boolean   -- lbi-b

left28(i: Integer, b: Boolean): Integer == i
left28(b1:Boolean, b2:Boolean): Boolean == b1
left28(b: Boolean, i: Integer): Boolean == b


local f28: (Integer,Boolean) -> Integer

f28(i: Integer, b: Boolean): Integer ==
	b := left28(z28, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left28(z28, b)		   -- same but choose i
	b := left28(left28(z28, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left28(left28(z28, b), b)     -- same but choose i
local z29: Boolean                         -- zb
local z29: Integer                         -- zi

z29: Boolean == false
z29: Integer == 0


local left29: (Integer, Boolean) -> Integer   -- lib-i
local left29: (Boolean, Boolean) -> Boolean   -- lbb-b
local left29: (Boolean, Integer) -> Boolean   -- lbi-b

left29(i: Integer, b: Boolean): Integer == i
left29(b1:Boolean, b2:Boolean): Boolean == b1
left29(b: Boolean, i: Integer): Boolean == b


local f29: (Integer,Boolean) -> Integer

f29(i: Integer, b: Boolean): Integer ==
	b := left29(z29, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left29(z29, b)		   -- same but choose i
	b := left29(left29(z29, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left29(left29(z29, b), b)     -- same but choose i
local z30: Boolean                         -- zb
local z30: Integer                         -- zi

z30: Boolean == false
z30: Integer == 0


local left30: (Integer, Boolean) -> Integer   -- lib-i
local left30: (Boolean, Boolean) -> Boolean   -- lbb-b
local left30: (Boolean, Integer) -> Boolean   -- lbi-b

left30(i: Integer, b: Boolean): Integer == i
left30(b1:Boolean, b2:Boolean): Boolean == b1
left30(b: Boolean, i: Integer): Boolean == b


local f30: (Integer,Boolean) -> Integer

f30(i: Integer, b: Boolean): Integer ==
	b := left30(z30, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left30(z30, b)		   -- same but choose i
	b := left30(left30(z30, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left30(left30(z30, b), b)     -- same but choose i

local z31: Boolean                         -- zb
local z31: Integer                         -- zi

z31: Boolean == false
z31: Integer == 0


local left31: (Integer, Boolean) -> Integer   -- lib-i
local left31: (Boolean, Boolean) -> Boolean   -- lbb-b
local left31: (Boolean, Integer) -> Boolean   -- lbi-b

left31(i: Integer, b: Boolean): Integer == i
left31(b1:Boolean, b2:Boolean): Boolean == b1
left31(b: Boolean, i: Integer): Boolean == b


local f31: (Integer,Boolean) -> Integer

f31(i: Integer, b: Boolean): Integer ==
	b := left31(z31, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left31(z31, b)		   -- same but choose i
	b := left31(left31(z31, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left31(left31(z31, b), b)     -- same but choose i

local z32: Boolean                         -- zb
local z32: Integer                         -- zi

z32: Boolean == false
z32: Integer == 0


local left32: (Integer, Boolean) -> Integer   -- lib-i
local left32: (Boolean, Boolean) -> Boolean   -- lbb-b
local left32: (Boolean, Integer) -> Boolean   -- lbi-b

left32(i: Integer, b: Boolean): Integer == i
left32(b1:Boolean, b2:Boolean): Boolean == b1
left32(b: Boolean, i: Integer): Boolean == b


local f32: (Integer,Boolean) -> Integer

f32(i: Integer, b: Boolean): Integer ==
	b := left32(z32, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left32(z32, b)		   -- same but choose i
	b := left32(left32(z32, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left32(left32(z32, b), b)     -- same but choose i

local z33: Boolean                         -- zb
local z33: Integer                         -- zi

z33: Boolean == false
z33: Integer == 0


local left33: (Integer, Boolean) -> Integer   -- lib-i
local left33: (Boolean, Boolean) -> Boolean   -- lbb-b
local left33: (Boolean, Integer) -> Boolean   -- lbi-b

left33(i: Integer, b: Boolean): Integer == i
left33(b1:Boolean, b2:Boolean): Boolean == b1
left33(b: Boolean, i: Integer): Boolean == b


local f33: (Integer,Boolean) -> Integer

f33(i: Integer, b: Boolean): Integer ==
	b := left33(z33, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left33(z33, b)		   -- same but choose i
	b := left33(left33(z33, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left33(left33(z33, b), b)     -- same but choose i

local z34: Boolean                         -- zb
local z34: Integer                         -- zi

z34: Boolean == false
z34: Integer == 0


local left34: (Integer, Boolean) -> Integer   -- lib-i
local left34: (Boolean, Boolean) -> Boolean   -- lbb-b
local left34: (Boolean, Integer) -> Boolean   -- lbi-b

left34(i: Integer, b: Boolean): Integer == i
left34(b1:Boolean, b2:Boolean): Boolean == b1
left34(b: Boolean, i: Integer): Boolean == b


local f34: (Integer,Boolean) -> Integer

f34(i: Integer, b: Boolean): Integer ==
	b := left34(z34, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left34(z34, b)		   -- same but choose i
	b := left34(left34(z34, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left34(left34(z34, b), b)     -- same but choose i

local z35: Boolean                         -- zb
local z35: Integer                         -- zi

z35: Boolean == false
z35: Integer == 0


local left35: (Integer, Boolean) -> Integer   -- lib-i
local left35: (Boolean, Boolean) -> Boolean   -- lbb-b
local left35: (Boolean, Integer) -> Boolean   -- lbi-b

left35(i: Integer, b: Boolean): Integer == i
left35(b1:Boolean, b2:Boolean): Boolean == b1
left35(b: Boolean, i: Integer): Boolean == b


local f35: (Integer,Boolean) -> Integer

f35(i: Integer, b: Boolean): Integer ==
	b := left35(z35, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left35(z35, b)		   -- same but choose i
	b := left35(left35(z35, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left35(left35(z35, b), b)     -- same but choose i

local z36: Boolean                         -- zb
local z36: Integer                         -- zi

z36: Boolean == false
z36: Integer == 0


local left36: (Integer, Boolean) -> Integer   -- lib-i
local left36: (Boolean, Boolean) -> Boolean   -- lbb-b
local left36: (Boolean, Integer) -> Boolean   -- lbi-b

left36(i: Integer, b: Boolean): Integer == i
left36(b1:Boolean, b2:Boolean): Boolean == b1
left36(b: Boolean, i: Integer): Boolean == b


local f36: (Integer,Boolean) -> Integer

f36(i: Integer, b: Boolean): Integer ==
	b := left36(z36, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left36(z36, b)		   -- same but choose i
	b := left36(left36(z36, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left36(left36(z36, b), b)     -- same but choose i

local z37: Boolean                         -- zb
local z37: Integer                         -- zi

z37: Boolean == false
z37: Integer == 0


local left37: (Integer, Boolean) -> Integer   -- lib-i
local left37: (Boolean, Boolean) -> Boolean   -- lbb-b
local left37: (Boolean, Integer) -> Boolean   -- lbi-b

left37(i: Integer, b: Boolean): Integer == i
left37(b1:Boolean, b2:Boolean): Boolean == b1
left37(b: Boolean, i: Integer): Boolean == b


local f37: (Integer,Boolean) -> Integer

f37(i: Integer, b: Boolean): Integer ==
	b := left37(z37, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left37(z37, b)		   -- same but choose i
	b := left37(left37(z37, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left37(left37(z37, b), b)     -- same but choose i

local z38: Boolean                         -- zb
local z38: Integer                         -- zi

z38: Boolean == false
z38: Integer == 0


local left38: (Integer, Boolean) -> Integer   -- lib-i
local left38: (Boolean, Boolean) -> Boolean   -- lbb-b
local left38: (Boolean, Integer) -> Boolean   -- lbi-b

left38(i: Integer, b: Boolean): Integer == i
left38(b1:Boolean, b2:Boolean): Boolean == b1
left38(b: Boolean, i: Integer): Boolean == b


local f38: (Integer,Boolean) -> Integer

f38(i: Integer, b: Boolean): Integer ==
	b := left38(z38, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left38(z38, b)		   -- same but choose i
	b := left38(left38(z38, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left38(left38(z38, b), b)     -- same but choose i

local z39: Boolean                         -- zb
local z39: Integer                         -- zi

z39: Boolean == false
z39: Integer == 0


local left39: (Integer, Boolean) -> Integer   -- lib-i
local left39: (Boolean, Boolean) -> Boolean   -- lbb-b
local left39: (Boolean, Integer) -> Boolean   -- lbi-b

left39(i: Integer, b: Boolean): Integer == i
left39(b1:Boolean, b2:Boolean): Boolean == b1
left39(b: Boolean, i: Integer): Boolean == b


local f39: (Integer,Boolean) -> Integer

f39(i: Integer, b: Boolean): Integer ==
	b := left39(z39, b)                -- lib-b+zi or lbb-b+zb: choose b
	i := left39(z39, b)		   -- same but choose i
	b := left39(left39(z39, b), b)     -- lib-b+zi or lbb-b+zb: choose b
	i := left39(left39(z39, b), b)     -- same but choose i
