#ifndef GF_EMBED_H
#define GF_EMBED_H

#include "axlobs.h"

Foam	gen0Embed(Foam val, AbSyn ab, TForm tf, AbEmbed embed);

Foam	gen0CrossToMulti	  (Foam, TForm);
Foam	gen0CrossToUnary	  (Foam, TForm);
Foam	gen0CrossToTuple	  (Foam, TForm);

Foam	gen0MultiToCross	  (Foam, TForm);
Foam	gen0MultiToTuple	  (Foam);
Foam	gen0MultiToUnary	  (Foam);

Foam	   gen0UnaryToMulti	  (Foam);
Foam	   gen0UnaryToCross	  (Foam, TForm);
Foam	   gen0UnaryToRaw	  (Foam, AbSyn);
Foam	   gen0UnaryToTuple	  (Foam);

Foam	   gen0RawToUnary	  (Foam, AbSyn);

#endif
