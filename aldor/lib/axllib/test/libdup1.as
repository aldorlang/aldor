-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  libdup1	-- using the same symbol in two #library commands is an error
--> testerrs
#pile

#library AxlLib "axllib"

#library Basic "basic.ao"

import from Basic

#library Complex "complex.ao"
#library Complex "basic.ao"

import from Complex
