#include "foamlib"
#pile

import { foo: MachineInteger -> () } from Foreign C("foo.h")

import from MachineInteger
foo(2) pretend MachineInteger
