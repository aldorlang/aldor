-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen l
--> testrun -l axllib
#pile

#include "axllib.as"

macro SI == SingleInteger

treeIter(i: SI):Generator SI == generate
  i < 1 => yield i
  for k in treeIter(i quo 2) repeat yield k
  yield i
  for k in treeIter(i quo 3) repeat yield k

treePrint(i: SI): () ==
  import from String
  for k in treeIter(i) repeat 
    print<<k
    print<<" "
  print<<newline

import from SI
treePrint(10)
