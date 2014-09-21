---------------------------- sit_halfge.as ------------------------------------
-- TO BE INCLUDED IN sit_modpoge.as
-- Case of prime smaller than half-word, use * and rem for product

	-- stores the solution in column k of the matrix sol
	-- c = column number is 1-indexed (first column is c = 1)
	-- r = number of leading columns before the c-th column
	local halfBS!(a:M,ra:Z,m:Z,sol:M,k:Z,sig:A,st:A, r:Z, c:Z, p:Z):() == {
		macro PRODUCT(aa, bb, cc)  == (((aa) * (bb)) rem (cc));
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
	local halfBS!(a:M,ra:Z,ca:Z,sl:M,den:A,sg:A,st:A,r:Z,b:M,cb:Z,p:Z):()=={
		macro PRODUCT(aa, bb, cc)  == (((aa) * (bb)) rem (cc));
		macro QUOTIENT(aa, bb, cc) == mod_/(aa, bb, cc);
		import from String;
		prt("halfge::halfBS!:a", a, ra, ca);
		prt("halfge::halfBS!:b", b, ra, cb);
		prt("halfge::halfBS!:sg = ", sg, ra);
		prt("halfge::halfBS!:st = ", st, r);
		TRACE("halfge::halfBS!:r = ", r);
		TRACE("halfge::halfBS!:p = ", p);
		s := prev cb; ra1 := prev ra; r1 := prev r;
		for k in 0..s repeat {
			TRACE("halfge::halfBS!:k = ", k);
			den.k := 1;
			for j in r..ra1 repeat {
				TRACE("halfge::halfBS!:j = ", j);
				if b(sg.j)(k) ~= 0 then { den.k := 0; break }
			}
			if den.k ~= 0 then for i in r1..0 by -1 repeat {
				TRACE("halfge::halfBS!:i = ", i);
				e := b(sg.i)(k);
				for j in next i..r1 repeat {
					TRACE("halfge::halfBS!:j = ", j);
					e := e - PRODUCT(a(sg.i)(st.j),
							sl(st.j)(k), p);
					if e < 0 then e := e + p;
				}
				sl(st.i)(k) := QUOTIENT(e, a(sg.i)(st.i), p);
			}
		}
		TRACE("halfge::halfBS! ", "exiting");
	}

	-- lazy version which normalizes only on potential overflow
	local halfRowEch!(a:M, n:Z, ca:Z, b:M, rb:Z, cb:Z, p:Z):(A,Z,A,Z)=={
		macro PRODUCT(aa, bb, cc)  == (((aa) * (bb)) rem (cc));
		macro QUOTIENT(aa, bb, cc) == mod_/(aa, bb, cc);
		macro INVERSE(aa, cc)      == modInverse(aa, cc);
		macro REMAINDER(aa, bb)    == ((aa) rem (bb));
		pp := identity n;
		d:Z := 1;
		st:A := new(n, next ca);
		r:Z := 0;
		p1 := prev p;
		ca1 := prev ca; cb1 := prev cb; n1 := prev n;
		while r < n for c in 0..ca1 repeat {
			l := r;
			while l < n and a(pp.l)(c) = 0 repeat l := next l;
			c1 := next c;
			if l < n then {
				st.r := c;
				r1 := next r;
				d := transpose!(pp, l, r, d);
				assert(a(pp.r)(c) > 0);
				assert(a(pp.r)(c) < p);
				-- alpha = - a(pp.r)(c)^{-1} mod p
				alpha := QUOTIENT(p1, a(pp.r)(c), p);
				-- value can overflow if entry > bound
				bound := maxint - alpha * p1;
				assert(bound > 0);
				-- reduce pivoting row mod p in both a and b
				for j in c1..ca1 repeat
					a(pp.r)(j) := REMAINDER(a(pp.r)(j), p);
				for j in 0..cb1 repeat
					b(pp.r)(j) := REMAINDER(b(pp.r)(j), p);
				for i in r1..n1 | a(pp.i)(c) ~= 0 repeat {
					assert(a(pp.i)(c) > 0);
					assert(a(pp.i)(c) < p);
					f := PRODUCT(a(pp.i)(c), alpha, p);
					for j in c1..ca1 repeat {
						x := a(pp.i)(j);
						if x > bound then
							x := REMAINDER(x, p);
						assert(x >= 0);
						a(pp.i)(j) := x + f*a(pp.r)(j);
					}
					for j in 0..cb1 repeat {
						x := b(pp.i)(j);
						if x > bound then
							x := REMAINDER(x, p);
						assert(x >= 0);
						b(pp.i)(j) := x + f*b(pp.r)(j);
					}
				}
				r := r1;
			}
			-- normalize column c below r
			if c1 < ca then for i in r1..n1 repeat
				a(pp.i)(c1) := REMAINDER(a(pp.i)(c1),p);
		}
		(pp, r, st, d);
	}

	-- version which normalizes the matrix every k loops
	local halfRowEch!(a:M,n:Z,ca:Z,b:M,rb:Z, cb:Z, k:Z, p:Z):(A,Z,A,Z)=={
		macro PRODUCT(aa, bb, cc)  == (((aa) * (bb)) rem (cc));
		macro QUOTIENT(aa, bb, cc) == mod_/(aa, bb, cc);
		macro REMAINDER(aa, bb)    == ((aa) rem (bb));
		assert(k > 0);
		pp := identity n;
		d:Z := 1;
		st:A := new(n, next ca);
		r:Z := 0;
		r1 := 0;
		ca1 := prev ca; cb1 := prev cb;
		n1 := prev n; p1 := prev p;
		loops:Z := 0;
		while r < n for c in 0..ca1 repeat {
			l := r;
			while l < n and a(pp.l)(c) = 0 repeat l := next l;
			c1 := next c;
			if l < n then {
				st.r := c;
				r1 := next r;
				d := transpose!(pp, l, r, d);
				assert(a(pp.r)(c) > 0);
				assert(a(pp.r)(c) < p);
				-- alpha = - a(pp.r)(c)^{-1} mod p
				alpha := QUOTIENT(p1, a(pp.r)(c), p);
				-- reduce pivoting row mod p in both a and b
				for j in c1..ca1 repeat
					a(pp.r)(j) := REMAINDER(a(pp.r)(j), p);
				for j in 0..cb1 repeat
					b(pp.r)(j) := REMAINDER(b(pp.r)(j), p);
				for i in r1..n1 | a(pp.i)(c) ~= 0 repeat {
					assert(a(pp.i)(c) > 0);
					assert(a(pp.i)(c) < p);
					f := PRODUCT(a(pp.i)(c), alpha, p);
					for j in c1..ca1 repeat {
						a(pp.i)(j) :=
							a(pp.i)(j)+f*a(pp.r)(j);
					}
					for j in 0..cb1 repeat {
						b(pp.i)(j) :=
							b(pp.i)(j)+f*b(pp.r)(j);
					}
				}
				r := r1;
				loops := next loops;
				if loops = k then {	-- normalize rest-matrix
					for ii in r1..n1 repeat {
						-- column c is normalized later
						for jj in c1..ca1 repeat
						    a(pp.ii)(jj) :=
							REMAINDER(a(pp.ii)(jj),p);
						for jj in 0..cb1 repeat
						    b(pp.ii)(jj) :=
							REMAINDER(b(pp.ii)(jj),p);
					}
					loops := 0;
				}
			}
			-- normalize column c below r
			if c1 < ca then for ii in r1..n1 repeat
				a(pp.ii)(c1) := REMAINDER(a(pp.ii)(c1), p);
		}
		(pp, r, st, d);
	}

	local halfDeter(a:M, n:Z, pp:A, r:Z, d:Z, p:Z):Z == {
		macro PRODUCT(aa, bb, cc) == (((aa) * (bb)) rem (cc));
		r < n => 0;
		det:Z := { d = 1 => 1; prev p };
		for i in 0..prev r repeat
			det := PRODUCT(det, a(pp.i)(i), p);
		det;
	}

	local halfDep(gen:Generator A, n:Z, p:Z, a:M):(A, Z) == {
		macro PRODUCT(aa, bb, cc) == (((aa) * (bb)) rem (cc));
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
