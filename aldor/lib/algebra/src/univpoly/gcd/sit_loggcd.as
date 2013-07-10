--------------------------------- sit_loggcd.as --------------------------------
-- TO BE INCLUDED IN sit_modpgcd.as
-- Case of prime with log/exp tables, use logs for product

	-- slightly faster than mod_+ since we know that a+b cannot overflow
	local mmod_+(a:SI, b:SI, q:SI):SI == {
		(c := a + b) < q => c;
		c - q;
	}

	local gcd!(a:ARR SI, n:SI, b:ARR SI, m:SI, lc:SI, p:SI,
			log:ARR SI, exp:ARR SI, p1:SI):(ARR SI, SI, SI) == {
		macro PRODUCT(aa,bb,cc)  == exp(mmod_+(log(aa),log(bb), p1));
		macro QUOTIENT(aa,bb,cc) == exp(mod_-(log(aa), log(bb), p1));
		macro REMAINDER == rem!;
		import from String;
		assert(next p1 = p);
		s := 0; t := 0;
		while m >= 0 repeat {
			-- replace a by a rem b, b unchanged
			(n, s) := REMAINDER(a, s, n, b, t, m, p, log, exp, p1);
			-- replace (a, b) by (b, a rem b)
			(a, s, n, b, t, m) := (b, t, m, a, s, n);
		}
		prt("gcd!:gcd", a, n, s);
		if (a.s ~= lc) then {		-- normalize lcoeff to lc
			c := QUOTIENT(lc, a.s, p);
			assert(c ~= 0);
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
	local rem!(a:ARR SI, s:SI, n:SI, b:ARR SI, t:SI, m:SI, p:SI,
			log:ARR SI, exp:ARR SI, p1:SI):(SI, SI) == {
		macro PRODUCT(aa,bb,cc) == exp(mmod_+(log(aa),log(bb),p1));
		macro INVERSE(aa,cc)	== exp(p1 - log(aa));
		invlcb := INVERSE(b.t, p);	-- lc(b)^{-1} mod p
		assert(invlcb ~= 0);
		while n >= m repeat {
			lca := a.s;		-- leading coefficient of a
			assert(lca ~= 0);
			lcq := PRODUCT(lca, invlcb, p);	-- lc(a)/lc(b) mod p
			assert(lcq ~= 0);
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

