-- standard include file for using axiom with libax0, rather than libaxiom

#library AxiomLib "ax0"

import from AxiomLib;
inline from AxiomLib;

macro {
        rep x == x @ % pretend Rep;
        per r == r @ Rep pretend %;
}

--import from Integer, PositiveInteger, NonNegativeInteger, SingleInteger;
--import from Float, DoubleFloat;

import { true: %; false: % } from Boolean;
import {
	string:		Literal -> %;
} from String;
