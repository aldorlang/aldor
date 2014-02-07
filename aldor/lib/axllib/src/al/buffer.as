#include "axllib"

-- This is really inefficient, but it just needs to work.
Buffer: with {
   new: () -> %;
   coerce: % -> TextWriter;
   string: % -> String;
} == add {
   Rep ==> Record(str: String);
   import from Rep;
   import from TextWriter;
   import from SingleInteger;

   coerce(self: %): TextWriter == writer( (c: Character): () +-> add!(self, c),
                                          (s: String, i1: SingleInteger,
                                                      i2: SingleInteger): SingleInteger +-> append!(self, s, i1, i2));

   add!(self: %, c: Character): () == add!(self, new(1, c));
   add!(self: %, s: String): () == {
        rep(self).str := concat(rep(self).str, s);
   }
   append!(self: %, s: String, i1: SingleInteger, i2: SingleInteger): SingleInteger == {
        if (i1 = 0 and i2 = 0) then
           add!(self, s);
        else if (i2 = 0) then
           add!(self, substring(s, i1, #s - i1))
        else
           add!(self, substring(s, i1, min(#s-i1, i2-i1)));
        0
   }

   string(self: %): String == rep(self).str;
   new(): % == per [""]
}

