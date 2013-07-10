-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--  libdup0	-- using the same symbol in two #library commands is an error
--> testerrs
#pile

#library AxlLib "axllib"

#library Basic "basic.ao"

import from Basic

#library Basic0 "basic.ao"
#library Basic0 "basic.ao"

import from Basic0
