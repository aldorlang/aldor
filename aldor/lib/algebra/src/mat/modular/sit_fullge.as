---------------------------- sit_fullge.as ------------------------------------
-- TO BE INCLUDED IN sit_modpoge.as
-- Case of prime greater than half-word, use mod_* for product

	-- stores the solution in column k of the matrix sol
	local fullBS!(a:M,ra:Z,m:Z,sol:M,k:Z,sig:A,st:A, r:Z, c:Z, p:Z):() == {
		macro PRODUCT(aa, bb, cc)  == mod_*(aa, bb, cc);
		macro QUOTIENT(aa, bb, cc) == mod_/(aa, bb, cc);
		r1 := prev r; c1 := prev c;
		sol.c1.k := 1;
		for i in r1..0 by -1 repeat {
			e := - a(sig.i)(c1);
			if e < 0 then e := e + p;
			for j in next i..r1 repeat {
				e := e - PRODUCT(a(sig.i)(st.j),sol(st.j)(k),p);
				if e < 0 then e := e + p;
			}
			sol(st.i)(k) := QUOTIENT(e, a(sig.i)(st.i), p);
		}
	}

	-- stores the solution in the matrix sl and the denoms in den
	-- the number of rows of b must be ra
	local fullBS!(a:M,ra:Z,ca:Z,sl:M,den:A,sg:A,st:A,r:Z,b:M,cb:Z,p:Z):()=={
		macro PRODUCT(aa, bb, cc)  == mod_*(aa, bb, cc);
		macro QUOTIENT(aa, bb, cc) == mod_/(aa, bb, cc);
		s := prev cb; ra1 := prev ra; r1 := prev r;
		for k in 0..s repeat {
			den.k := 1;
			for j in r..ra1 repeat {
				if b(sg.j)(k) ~= 0 then { den.k := 0; break }
			}
			if den.k ~= 0 then for i in r1..0 by -1 repeat {
				e := b(sg.i)(k);
				for j in next i..r1 repeat {
					e := e - PRODUCT(a(sg.i)(st.j),
							sl(st.j)(j), p);
					if e < 0 then e := e + p;
				}
				sl(st.i)(k) := QUOTIENT(e, a(sg.i)(st.i), p);
			}
		}
	}

	local fullRowEch!(a:M, n:Z, ca:Z, b:M, rb:Z, cb:Z, p:Z):(A,Z,A,Z)=={
		macro PRODUCT(aa, bb, cc) == mod_*(aa, bb, cc);
		macro INVERSE(aa, cc)     == modInverse(aa, cc);
		pp := identity n;
		d:Z := 1;
		st:A := new(n, next ca);
		r:Z := 0;
		ca1 := prev ca; cb1 := prev cb; n1 := prev n;
		while r < n for c in 0..ca1 repeat {
			l := r;
			while l < n and a(pp.l)(c) = 0 repeat l := next l;
			if l < n then {
				st.r := c;
				r1 := next r; c1 := next c;
				d := transpose!(pp, l, r, d);
				arcinv := INVERSE(a(pp.r)(c), p);
				for i in r1..n1 | a(pp.i)(c) ~= 0 repeat {
					f := PRODUCT(a(pp.i)(c), arcinv, p);
					for j in c1..ca1 repeat {
						pr := PRODUCT(f, a(pp.r)(j), p);
						a(pp.i)(j) :=
							mod_-(a(pp.i)(j),pr,p);
					}
					for j in 0..cb1 repeat {
						pr := PRODUCT(f, b(pp.r)(j), p);
						b(pp.i)(j) :=
							mod_-(b(pp.i)(j),pr,p);
					}
				}
				r := r1;
			}
		}
		(pp, r, st, d);
	}

	local fullDeter(a:M, n:Z, pp:A, r:Z, d:Z, p:Z):Z == {
		macro PRODUCT(aa, bb, cc) == mod_*(aa, bb, cc);
		r < n => 0;
		det:Z := { d = 1 => 1; prev p };
		for i in 0..prev r repeat
			det := PRODUCT(det, a(pp.i)(i), p);
		det;
	}

	local fullDep(gen:Generator A, n:Z, p:Z, a:M):(A, Z) == {
		macro PRODUCT(aa, bb, cc) == mod_*(aa, bb, cc);
		macro INVERSE(aa, cc)     == modInverse(aa, cc);
		n1 := prev n;
		pp := identity n;
		d:Z := 1;
		r:Z := 0;
		for v in gen repeat {
			r1 := next r;
			for i in 0..n1 repeat a.i.r := v.i;
			for j in 0..prev r repeat {
				for i in next(j)..n1 repeat {
					pr := PRODUCT(a(pp.i)(j),a(pp.j)(r),p);
					a(pp.i)(r) := mod_-(a(pp.i)(r), pr, p);
				}
			}
			l := r;
			while l < n and a(pp.l)(r) = 0 repeat l := next l;
			if l < n then {		-- still linearly independent
				d := transpose!(pp, l, r, d);
				inv := INVERSE(a(pp.r)(r), p);
				for i in r1..n1 repeat
					a(pp.i)(r):= PRODUCT(a(pp.i)(r),inv, p);
				r := r1;
			}
			else return(pp, r1);
		}
		never;		-- gen finished, all linearly independent
	}
