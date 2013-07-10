-------------------------------- algebrauid.as -----------------------------
--
-- Definition of constants used by operators, parsers and the cutoff function
--
-- Copyright (c) Manuel Bronstein 1994-2002
-- Copyright (c) INRIA 1997-2002, Version 1.0.1
-- Logiciel Algebra (c) INRIA 1997-2002, dans sa version 1.0.1
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1994-1997
-----------------------------------------------------------------------------

macro {
	UID__PLUS	== 100;
	UID__TIMES	== 200;
	UID__EXPT	== 300;
	UID__FACTORIAL	== 310;
	UID__DIVIDE	== 400;
	UID__MINUS	== 500;
	UID__MATRIX	== 1000;
	UID__VECTOR	== 1010;
	UID__LIST	== 1020;
	UID__LLIST	== 1021;
	UID__COMPLEX	== 1030;
	UID__EQUAL	== 1100;
	UID__NOTEQUAL	== 1110;
	UID__AND	== 1120;
	UID__OR		== 1121;
	UID__NOT	== 1122;
	UID__IF		== 1130;
	UID__CASE	== 1140;
	UID__LESSEQUAL	== 1150;
	UID__LESSTHAN	== 1160;
	UID__MOREEQUAL	== 1170;
	UID__MORETHAN	== 1180;
	UID__BIGO	== 1190;
	UID__PREFIX	== 10000;
	UID__ASSIGN	== 10010;
	UID__SUBSCRIPT	== 10020;

	TEXPREC__PLUS	== 1000;
	TEXPREC__TIMES	== 2000;
	TEXPREC__DIVIDE	== 2500;
	TEXPREC__MINUS	== 2700;
	TEXPREC__EXPT	== 3000;

	CUTOFF__KARAMULT== 0;
	CUTOFF__FFTMULT	== 1;
	CUTOFF__QUOTBY	== 10;
	CUTOFF__DIVIDEBY== 11;
	CUTOFF__DIVIDE	== 12;

	INFIX2(f,p,l,n) == { f(p,first l); p << n; f(p,first rest l)};
}
