
-- Type hasher for old compiler style type names which produces a hash code
-- compatible with the axiomxl compiler.  Takes a hard error if the type
-- is parameterized, but has no constructor modemap.
hashType(type) ==
	SYMBOLP type  => hashString SYMEBOL_-NAME type
	type is [dom] => hashString SYMBOL_-NAME dom
	type is ['_:, ., type2] => hashType type2
	[op, :args] := type
	hash := hashString SYMBOL_-NAME op

	op = 'Mapping =>
		hash := hashString '"->"
		[retType, :mapArgs] := args
		for arg in mapArgs repeat
			hash := hashCombine(hashType(arg), hash)
		hashCombine(hashType(retType), hash)

	op in $DomainsWithoutLisplibs =>
		for arg in args repeat
			hash := hashCombine(hashType(arg), hash)
		hash

	cmm :=   CDDAR getConstructorModemap(op)
	cosig := CDR   getConstructorProperty(op, 'coSig)
	for arg in args for c in cosig for ct in cmm repeat
		if c then
			hash := hashCombine(hashType(arg), hash)
		else
			hash := hashCombine(hashType(ct), hash)

	hash

$hashModulus := 1073741789 			-- largest 30-bit prime

-- Produce a 30-bit hash code.  This function must produce the same codes
-- as the axiomxl string hasher in src/strops.c
hashString str ==
	h := 0
	for i in 0..#str-1 repeat
		j := CHAR_-CODE char str.i
		h := LOGXOR(h, ASH(h, 8))
		h := h + j + 200041
		h := LOGAND(h, 1073741823)	-- 0x3FFFFFFF
	REM(h, $hashModulus)

-- Combine two hash codes to make a new one.  Must be the same as in
-- the hashCombine function in axllib/runtime.as in axiomxl.
hashCombine(hash1, hash2) ==
	 MOD(ASH(LOGAND(hash2, 16777215), 6) + hash1, $hashModulus)
