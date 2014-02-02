Hierarchy
=========

The following lists the directory hierarchy below the `aldor` directory.

- aldor:		The Aldor compiler and runtime library.
  - contrib:		Contributed compiler code not part of the main branch.
    - gmp:		GMP-based runtime library.
  - lib:		Java and Aldor parts of the runtime library.
    - java:		Support library for generated Java code.
    - libfoam:		Aldor part of the runtime library (Java and C).
    - libfoamlib:	Stable but minimal standard library for libfoam.
  - src:		The Aldor compiler and testsuite sources.
    - java:		Java code generator (`javagen`).
    - test:		Unit test sources.
  - subcmd:		Supporting tools that are installed with the compiler.
  - test:		Simple tests only relying on the runtime library.
  - tools:		Other tools, not installed.
    - monitor:		Sandbox code used on the Try Aldor page.
    - testaldor:	Test driver for the Aldor compiler.
    - unix:		Various utilities.
- aldorug:		The Aldor User Guide LaTeX sources.
- lib:			Secondary Aldor libraries (stdlib and others).
  - aldor:		Aldor standard library.
  - algebra:		Mathematical domains for Aldor.
  - ax0:		Aldor side of the Axiom-Aldor bridge.
  - axldem:		AxiomXL (old Aldor) demo library.
  - axllib:		AxiomXL standard library.
  - debuglib:		Debugger support library.
- m4:			Extra `m4(1)` functions used in `configure`.
