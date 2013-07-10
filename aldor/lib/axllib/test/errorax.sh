#!/bin/sh
#
# This script tests error reporting for compiling abstract syntax files (.ax).

cd $TMPDIR

cat > wrongo.ax <<++ThatsAll++
(Sequence
  (Export
    (Sequence
      (Declare Type Type)
      (Declare Tuple (Apply -> Type Type))
      (Declare
        ->
        (Apply -> (Comma (Apply Tuple Type) (Apply Tuple Type)) Type))
      (Declare Literal Type)
      (Declare String Type)
      (Declare SingleInteger Type)
      (Declare TextWriter Type))
    ()
    ())
  (Foreign
    (Sequence
      (Declare stdoutFile (Apply -> (Comma) TextWriter))
      (Declare printSInt (Apply -> (Comma TextWriter SingleInteger) (Comma)))
      (Declare printString (Apply -> (Comma TextWriter String) (Comma)))
      (Declare printNewLine (Apply -> TextWriter (Comma))))
    ())
  (Builtin (Declare ArrToSInt (Apply -> String SingleInteger)))
  (Define
    (Declare integer (Apply -> (Declare s Literal) SingleInteger))
    (Lambda
      (Comma (Declare s Literal))
      SingleInteger
      (Label integer (Apply ArrToSInt (PretendTo s String)))))
  (Define
    (Declare string (Apply -> (Declare s Literal) String))
    (Lambda
      (Comma (Declare s Literal))
      String
      (Label string (PretendTo s String))))
  (Assign out (Apply stdoutFile))
  (Apply printSInt out (LitInteger "42"))
  (Apply printString out (LitString " Skidoo"))
  (Apply printNewLine out))
++ThatsAll++
aldor -Mno-emax -M2 wrongo.ax
rm wrongo.ax
