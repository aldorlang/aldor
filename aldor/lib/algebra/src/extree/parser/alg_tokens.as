------------------------------- alg_tokens.as ----------------------------------
-- Copyright Swiss Federal Polytechnic Institute Zurich, 1995-1997
--


-- Special token codes
macro {
	TOKEN		== MachineInteger;
	TOK__UNKNOWN	== 100;
	TOK__EOF	== 200;
	TOK__EOEXPR	== 210;
	TOK__LPAREN	== 300;
	TOK__RPAREN	== 301;
	TOK__LBRACKET	== 302;
	TOK__RBRACKET	== 303;
	TOK__LCURLY	== 304;
	TOK__RCURLY	== 305;
	TOK__COMMA	== 310;
}

-- Special parser codes
macro {
	-- parser precedence table 
	PARSER__PLUS	== 0;
	PARSER__MINUS	== 1;
	PARSER__TIMES	== 2;
	PARSER__DIVIDE	== 3;
	PARSER__POWER	== 4;
	PARSER__PREC__SIZE == 5;
	
	-- parser possible precedence level 0 < level 1 < level 2, 
	-- i.e level 2 binds stronger than level 1, and so on.
	PARSER__PREC0	== 0;
	PARSER__PREC1	== 1;
	PARSER__PREC2	== 2;
	
	-- parser error codes
	PARSER__LOOKAHEAD		==  1; -- lookahead was used (int. use)
	PARSER__NOERROR			==  0; -- no error
	PARSER__EOF			== -1; -- end of file
	PARSER__ERROR__IN__EXPR		== -2; -- error in expression
	PARSER__LPAREN__MISSING		== -3; -- ( expected
	PARSER__RPAREN__MISSING		== -4; -- ) expected
	PARSER__RBRACKET__MISSING	== -5; -- ] expected
	PARSER__ASSIGN__MISSING		== -6; -- := expected
}
