--execute with
--  aldor -Ginterp max.as

#include "algebra"

import from String;
import from Symbol;
import from List Symbol;
local VAR1  == (-"y1");
local VAR2  == (-"y2");
local VARS  == OrderedVariableList(reverse! [ VAR1, VAR2 ]);	

import from MachineInteger;
import from Partial VARS;
import from VARS;
local var1: VARS := retract variable ( VAR1 );
local var2: VARS := retract variable ( VAR2 );

import from TextWriter;
import from Character;
stdout << "var1              : " << var1 << newline;
stdout << "var2              : " << var2 << newline;
stdout << "var1 < var2       : " << (var1<var2) << newline;
stdout << "var1 > var2       : " << (var1>var2) << newline;
stdout << "max( var1, var2 ) : " << max(var1,var2) << newline;
