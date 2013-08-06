--* Received: from uk.ac.nsfnet-relay by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA05327; Thu, 30 Nov 95 20:47:55 GMT
--* Received: from bernina-rz-fddi.ethz.ch by sun3.nsfnet-relay.ac.uk 
--*           with Internet SMTP id <sg.22369-0@sun3.nsfnet-relay.ac.uk>;
--*           Thu, 30 Nov 1995 20:43:26 +0000
--* Received: from vinci.inf.ethz.ch by bernina.ethz.ch with SMTP inbound;
--*           Thu, 30 Nov 1995 21:42:41 +0100
--* Received: (bronstei@localhost) by vinci.inf.ethz.ch (8.6.8/8.6.6) id VAA21241 
--*           for ax-bugs@nag.co.uk; Thu, 30 Nov 1995 21:42:40 +0100
--* Date: Thu, 30 Nov 1995 21:42:40 +0100
--* From: bronstei <bronstei@ch.ethz.inf>
--* Message-Id: <199511302042.VAA21241@vinci.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [2] another optimizer problem (-Q1 ok, -Q2 bombs)

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Q2 optbug.as
-- Version: 1.1.5
-- Original bug file name: optbug.as

-------------------------- optbug.as -------------------------
--
-- % axiomxl -Q1 optbug.as   -- works perfectly within a few seconds
--
-- % axiomxl -Q2 optbug.as   -- runs out of memory after a loooong time
--

#include "axllib"

macro Z == Integer;

Fib(n:Z):(Z, Z) == {
	n < 0 => error "Fib: expects a nonnegative input";
	zero? n => (1, 1);
	(anminus1, an) := Fib prev n;
	(an, anminus1 + an);
}

-- returns (g, s, t) s.t. g = gcd(u, v) = s u + v t
EEA(u:Z, v:Z):(Z, Z, Z) == {
	a:Z := d:Z := 1;	-- u_i = a u + b v
	b:Z := c:Z := 0;	-- v_i = c u + d v
	if abs u < abs v then {	-- swap u and v
		(u, v) := (v, u);
		(a, b, c, d) := (c, d, a, b);
	}
	while v ~= 0 repeat {
		(q, r) := divide(u, v);
		(u, v) := (v, r);
		(a, b, c, d) := (c, d, a - q * c, b - q * d);
	}
	(u, a, b);
}

check(k:SingleInteger, algo:(Z, Z) -> (Z, Z, Z)):Boolean == {
	import from Z;
	(u, v) := Fib(prev(k)::Z);
	(g, x, y) := algo(u, v);
	g = 1 and x * u + y * v = g;
}

checkAlgo(algo:(Z, Z) -> (Z, Z, Z)):() == {
	import from SingleInteger;
	for k in 1..5 repeat {
		print << "k = " << k << ": " << check(1000 * k, algo);
		print << newline;
	}
}

print << "Checking the Extended Euclidean Algorithm:" << newline;
checkAlgo EEA;

double(n:Z):Z == { import from SingleInteger; shift(n,  1); }
halve(n:Z):Z  == { import from SingleInteger; shift(n, -1); }

-- x = a u + b v is even, u and v are not both even
-- return(y, a', b') s.t. y = x/2 = a' u + b' v
halve(x:Z, a:Z, b:Z, u:Z, v:Z):(Z, Z, Z) == {
	x := halve x;
	even? a and even? b => (x, halve a, halve b);
	(x, halve(a + v), halve(b - u));
}


binary(u:Z, v:Z):(Z, Z, Z) == {
	import from SingleInteger;
	even? u and even? v => error "binary: both inputs are even.";
	a:Z := d:Z := 1;	-- u_i = a u + b v
	b:Z := c:Z := 0;	-- v_i = c u + d v
	u0 := u; v0 := v;
	if u < 0 then { u := -u; a := -a; }
	if v < 0 then { v := -v; d := -d; }
	if u < v then {
		(u, v) := (v, u);
		(a, b, c, d) := (c, d, a, b);
	}
	while v > 0 repeat {
		if even? u then (u, a, b) := halve(u, a, b, u0, v0);
		else {
			if even? v then (v, c, d) := halve(v, c, d, u0, v0);
			else {
				(n, x, y) := halve(u - v, a - c, b - d, u0, v0);
				(u, v) := (v, n);
				(a, b, c, d) := (c, d, x, y);
			}
		}
		if u < v then {
			(u, v) := (v, u);
			(a, b, c, d) := (c, d, a, b);
		}
	}
	(u, a, b);
}

print << "Checking the Extended Binary Gcd:" << newline;
checkAlgo binary;
