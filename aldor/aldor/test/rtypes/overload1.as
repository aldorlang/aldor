#library L0 "l0.ao"
#pile
import from L0
import from TT

f(x: TT): TT == never
f(x: Boolean): Boolean == never

g(): ? == f create()