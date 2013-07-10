------------------------------- sit_halfgcd.as --------------------------------
-- TO BE INCLUDED IN sit_modpgcd.as
-- Case of prime smaller than half-word, use * and rem for product

	local halfgcd!(a:ARR SI,n:SI,b:ARR SI,m:SI,lc:SI,p:SI):(ARR SI,SI,SI)=={
		macro PRODUCT(aa, bb, cc)  == (((aa) * (bb)) rem (cc));
		macro QUOTIENT(aa, bb, cc) == mod_/(aa, bb, cc);
		macro REMAINDER == halfrem!;
		import from String;
		s := 0; t := 0;
		while m >= 0 repeat {
			-- replace a by a rem b, b unchanged
			(n, s) := REMAINDER(a, s, n, b, t, m, p);
			-- replace (a, b) by (b, a rem b)
			(a, s, n, b, t, m) := (b, t, m, a, s, n);
		}
		prt("gcd!:gcd", a, n, s);
		if (a.s ~= lc) then {		-- normalize lcoeff to lc
			c := QUOTIENT(lc, a.s, p);
			a.s := lc;
			for i in 1..n repeat {
				ai := a(s+i);
				if ai ~= 0 then a(s+i) := PRODUCT(ai, c, p);
			}
		}
		prt("gcd!:gcd normalized", a, n, s);
		(a, n, s);
	}

	-- given 2 polys in a and b of degrees n and m respectively,
	-- a the modulus p, compute a rem b in F_p[x]
	-- requires the polys to be stored leading coeff first
	-- returns the degree of the remainder and its starting index in a
	-- returns (-1, arbitrary) if the remainder is 0
	-- s = starting index of a in its array (i.e. first nonzero coeff)
	-- t = starting index of b in its array (i.e. first nonzero coeff)
	local halfrem!(a:ARR SI,s:SI,n:SI,b:ARR SI,t:SI,m:SI,p:SI):(SI, SI) == {
		macro PRODUCT(aa, bb, cc) == (((aa) * (bb)) rem (cc));
		macro INVERSE(aa, cc)     == modInverse(aa, cc);
		invlcb := INVERSE(b.t, p);	-- lc(b)^{-1} mod p
		while n >= m repeat {
			lca := a.s;		-- leading coefficient of a
			lcq := PRODUCT(lca, invlcb, p);	-- lc(a)/lc(b) mod p
			-- we know the first coeff of the new a must become 0
			allzero?:Boolean := true;
			olddeg := n; n := prev n;
			local c:SI;
			for i in 1..m repeat {
				ca := a(s+i); cb := b(t+i);
				--substract lc(a)/lc(b) x^{deg(a)-deg(b)} b in a
				if zero? cb then c := ca;
				else a(s+i) := c := _
						mod_-(ca, PRODUCT(lcq,cb,p), p);
				-- figure out new degree of a
				if allzero? then {
					if (allzero? := zero? c) then n:=prev n;
				}
			}
			-- if a is 0 until now, figure out its actual degree
			if allzero? then {
				i := m+1;
				while i <= olddeg and zero? a(s+i) repeat {
					n := prev n;
					i := next i;
				}
			}
			s := s + olddeg - n;
		}
		(n, s);
	}

	-- version which normalizes the polys every k additions of products
	local halfgcd!(a:ARR SI,n:SI,b:ARR SI,m:SI,lc:SI,k:SI,
		p:SI):(ARR SI,SI,SI)=={
		macro PRODUCT(aa, bb, cc) == (((aa) * (bb)) rem (cc));
		macro REMAINDER == halfrem!;
		import from String;
		s := 0; t := 0;
		assert(k > 0);
		while m >= 0 repeat {
			-- replace a by a rem b, b unchanged
			(n, s) := REMAINDER(a, s, n, b, t, m, k, p);
			-- replace (a, b) by (b, a rem b)
			(a, s, n, b, t, m) := (b, t, m, a, s, n);
		}
		prt("gcd!:gcd", a, n, s);
		if (a.s ~= lc) then {		-- normalize lcoeff to lc
			c := mod_/(lc, a.s, p);
			a.s := lc;
			for i in 1..n repeat {
				ai := a(s+i);
				if ai ~= 0 then a(s+i) := PRODUCT(ai, c, p);
			}
		}
		prt("gcd!:gcd normalized", a, n, s);
		(a, n, s);
	}

	-- version which normalizes the polys every k additions of products
	-- only the leading coeff of the result is normalized
	-- given 2 polys in a and b of degrees n and m respectively,
	-- a the modulus p, compute a rem b in F_p[x]
	-- requires the polys to be stored leading coeff first
	-- both a and b must be normalized on entry, result is normalized
	-- returns the degree of the remainder and its starting index in a
	-- returns (-1, arbitrary) if the remainder is 0
	-- s = starting index of a in its array (i.e. first nonzero coeff)
	-- t = starting index of b in its array (i.e. first nonzero coeff)
	local halfrem!(a:ARR SI,s:SI,n:SI,b:ARR SI,t:SI,m:SI,k:SI,
		p:SI):(SI,SI) == {
		macro REDUCE(aa, bb) == ((aa) rem (bb));
		macro PRODUCT(aa, bb, cc) == (((aa) * (bb)) rem (cc));
		import from String;
		prt("gcd!:halfrem!, a", a, n, s);
		prt("gcd!:halfrem!, b", b, m, t);
		TRACE("gcd!:halfrem!, k = ", k);
		TRACE("gcd!:halfrem!, p = ", p);
		TRACE("gcd!:halfrem!, n = ", n);
		TRACE("gcd!:halfrem!, m = ", m);
		TRACE("gcd!:halfrem!, s = ", s);
		TRACE("gcd!:halfrem!, t = ", t);
		invlcb := mod_/(prev p, b.t, p);	-- -lc(b)^{-1} mod p
		TRACE("gcd!:halfrem!, invlcb = ", invlcb);
		loops:SI := 0;
		while n >= m repeat {
			if loops = k then {	-- normalize a
				for i in 1..m repeat a(s+i) := REDUCE(a(s+i),p);
				loops := 0;
			}
			lca := a.s;	-- leading coefficient of a (normalized)
			lcq := PRODUCT(lca, invlcb, p);	-- -lc(a)/lc(b) mod p
			-- we know the first coeff of the new a must become 0
			allzero?:Boolean := true;
			olddeg := n; n := prev n;
			for i in 1..m repeat {
				ca := a(s+i); cb := b(t+i);
				-- add -lc(a)/lc(b) x^{deg(a)-deg(b)} b in a
				c := { zero? cb => ca; ca + lcq * cb }	-- lazy
				-- figure out new degree of a
				if allzero? then {
					c := REDUCE(c, p);
					if (allzero? := zero? c) then n:=prev n;
				}
				a(s + i) := c;
			}
			-- if a is 0 until now, figure out its actual degree
			if allzero? then {
				i := m+1;
				while i <= olddeg and
					zero?(a(s+i):=REDUCE(a(s+i),p)) repeat {
						n := prev n;
						i := next i;
				}
			}
			s := s + olddeg - n;
			prt("gcd!:halfrem!, a", a, n, s);
			loops := next loops;
		}
		-- normalize a (lcoeff is already normalized)
		for j in 1..n repeat a(s+j) := REDUCE(a(s+j),p);
		(n, s);
	}

