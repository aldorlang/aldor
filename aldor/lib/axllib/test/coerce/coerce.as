-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs
#pile
#include "axllib.as"

import from Integer

b(): Byte == 0
b(): Boolean == false

left( i: Byte , j: Boolean ): Byte == i


j0: Byte == b():: Integer
j1: Byte == false:: Integer
j2:Integer == (1+1)::Integer
j3:Integer == 0 ::DoubleFloat
j4:Integer == false ::Integer
j5:Integer == false ::SingleFloat
j6:Integer == left( 1,false)::Boolean


j7:Integer == false  
:: Integer
j8:Integer == false ::
   Integer




