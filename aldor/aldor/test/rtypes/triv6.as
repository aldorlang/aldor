#library L0 "l0.ao"
#library L1 "l1.ao"
#pile
import from L0, L1
import from TT, TT2

makeTT(): ? == value@TT

inferTT(): ? == value * value
inferTT2(): ? == value + value
