Internal structure
==================

The Aldor compiler has a layered design:

- port:		Portability layer, abstraction of OS-specific code.
- gen:		General data structures such as lists and tables.
- struct:	Compiler-specific data structures such as AST and IR.
- phase:	All other compiler code, such as parsing and optimisation.
- test:		Unit tests for public interfaces.
