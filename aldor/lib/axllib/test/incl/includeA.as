-- Start file "test/incl/includeA.as"

-- This should include include0.as from this directory, not the test directory.
#include "include0"

-- This should not reinclude
#include "include0"

-- This should reinclude
#reinclude "include0.as"

-- This should include relative to the directory of this file.
#reinclude "Z/includeZ"

-- End   file "test/incl/includeA.as"
