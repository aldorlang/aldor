-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile

#include "axllib.as"

import { log: BDFlo -> BDFlo } from Foreign
import from DoubleFloat

print<<log(2.718::BDFlo)::DoubleFloat<<newline

f: DoubleFloat := 1.234567890123456789
print<<f<<newline
