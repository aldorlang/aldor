-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--
-- numeral2.as
--
-- This file is used by ar6.sh.
#pile

#include "axllib.as"
#library ZeroLib "../numeral0/numeral0.ao"
#library OneLib  "../numeral1/numeral1.ao"

import from ZeroLib
import from OneLib

export Two: with
	0: %
	1: %
 == add
	0: % == Zero pretend %
	1: % == One  pretend %
