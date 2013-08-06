-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testphase abnorm
#pile

H0 == G
H1 == G with G
H2 == F(G) with (G; F(G))

H3 == with
	f: A -> B
	f: B -> A
	g: A -> B
	f: B -> A

H4 == (with (f: A -> B; g: B->A)) with
	(with (f: A->B; h: A -> B))

H5 == (with (f: A -> B; g: B->A)) with
	(with (f: A->B; h: A -> B); with (h:A->C; i: K))
