----------------------- sit_infexpr.as ---------------------------------
--
-- Infix expression parser
--
-- Copyright (c) Niklaus Mannhart 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
--
-- parser parses an expression (or assignments) and creates an expression tree.
--
-- Niklaus Mannhart, Mar 25, 1996
--   11.06.96 nm: added bracket support [..] => List (...)
--    6.01.98 nm: fixed bug: -x^2 => (^(-x) 2) instead of (-(^x 2))
--   18.03.99 nm: added empty list: [] => List()
--   22.03.99 nm: added empty argument list: f() => f()
--
-- grammar: (EBNF notation)
--   Sumit      := Expr {":=" Expr}.
--   Expr       := Term {pre0op Term}.
--   Term       := Factor {pre1op Factor}.
--   Factor     := Power {pre2op Power}.
--   Power      := [op] Leaf | [op] Prefix | "(" Expr ")" | "[" Expr "]".
--   Prefix     := PF "(" Exp { "," Expr} ")".
--
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#include "alg_tokens"

macro {
	SI	== MachineInteger;
	TEXT	== TextReader;
	TREE	== ExpressionTree;
}

#if ALDOC
\thistype{InfixExpressionParser}
\History{Niklaus Mannhart}{25/03/96}{created}
\Usage{import from \this}
\Descr{\this~implements infix expression parsers.}
\begin{exports}
\category{\altype{ParserReader}}\\
\alexp{precedences!}:
& (\%, \altype{MachineInteger}, \altype{MachineInteger}) $\to$ \% &
Set operator precedences\\
\end{exports}
#endif
 
InfixExpressionParser: ParserReader with {
	precedences!: (%, SI, SI) -> %;
#if ALDOC
\alpage{precedences!}
\Usage{\name(p, op, n)}
\Signature{(\%, \altype{MachineInteger}, \altype{MachineInteger})}{\%}
\Params{
{\em p} & \% & A parser\\
{\em op} & \altype{MachineInteger} & An operator code\\
{\em n} & \altype{MachineInteger} & Its new precedence\\
}
\Descr{Sets the precedence of op to n and returns the new parser. op must
be one of PARSER\_\_PLUS, PARSER\_\_MINUS, PARSER\_\_TIMES, PARSER\_\_DIVIDE or
PARSER\_\_POWER.}
#endif
} == add {
	macro {
		-- TOKEN     == MachineInteger;
		-- PrecTable == PrimitiveArray(SI);
		PrecTable == Array(SI);
		Rep       == Record (sreader:TEXT,prec:PrecTable,errorCode:SI);
		ExprList  == List TREE;	
	}
	
	import from SI, PrecTable, Rep, ExprList, TOKEN, Token; 

	parser(reader:TEXT):% == {
		r: Rep := [reader, new(PARSER__PREC__SIZE), PARSER__NOERROR];
		r.prec(PARSER__PLUS) := r.prec(PARSER__MINUS) := PARSER__PREC0;
		r.prec(PARSER__TIMES) := r.prec(PARSER__DIVIDE) :=PARSER__PREC1;
		r.prec(PARSER__POWER) := PARSER__PREC2;
		per r;
	}

	precedences!(self:%, op:SI, preceding:SI):% == {
		r := rep self;
		r.prec(op) := preceding;
		per r;
	}

	local ctoken: Token; -- current token accessed and changed by 
                             -- the following functions.
	local lookahead: Token; -- next token that will be stored in ctoken
	                        -- when nextToken! is called.
			     
	local printOperator(t: Token): () == {
		import from TextWriter, String;
		id := uniqueId$(operator t);
		id = UID__PLUS => stderr << "+"; 
		id = UID__MINUS => stderr << "-";
		id = UID__TIMES => stderr << "*";
		id = UID__DIVIDE => stderr << "/";
		id = UID__EXPT => stderr << "^";
		stderr << "ups ???";
	}
	
	local nextToken!(r:Rep):() == {
		import from TEXT, Scanner;
		free ctoken, lookahead;

		if r.errorCode = PARSER__LOOKAHEAD then {
			ctoken := lookahead;
		} else {
			ctoken := scan!(r.sreader);
		}
		r.errorCode := PARSER__NOERROR;
#if TRACE
		import from TextWriter, String, Character;
                if eoexpr? ctoken then stderr << "eoexpr";
                else if eof? ctoken then stderr << "eof";
                else if operator? ctoken then {
                        if assign? ctoken then stderr << ":="
                        else printOperator (ctoken);
                }
                else if leaf? ctoken then stderr << "leaf";
                else if special? ctoken then {
                        stderr << "special: " << special ctoken;
                        if comma? ctoken then stderr << ",";
                        else if leftParen? ctoken then stderr << "(";
                        else if rightParen? ctoken then stderr << ")";
                }
                else if prefix? ctoken then stderr << "prefix";
                else stderr << "????";
                stderr << newline;
#endif
	}

	lookahead!(r:Rep):() == {
		import from TEXT, Scanner;
		import from TextWriter, String, Character;
		free lookahead;
		
		if r.errorCode = PARSER__LOOKAHEAD then {
			stderr << "ups in lookahead ???" << newline;
		} else {
			r.errorCode := PARSER__LOOKAHEAD;
			lookahead := scan!(r.sreader);
		}
#if TRACE
                if eoexpr? ctoken then stderr << "eoexpr";
                else if eof? ctoken then stderr << "eof";
                else if operator? ctoken then {
                        if assign? ctoken then stderr << ":="
                        else printOperator (ctoken);
                }
                else if leaf? ctoken then stderr << "leaf";
                else if special? ctoken then {
                        stderr << "special: " << special ctoken;
                        if comma? ctoken then stderr << ",";
                        else if leftParen? ctoken then stderr << "(";
                        else if rightParen? ctoken then stderr << ")";
                }
                else if prefix? ctoken then stderr << "prefix";
                else stderr << "????";
                stderr << newline;
#endif
	}
	
	local leftParen? (t:Token):Boolean == {
		assert(special? t);
		special t = TOK__LPAREN;
	}
		
	local rightParen? (t:Token):Boolean == {
		assert(special? t);
		special t = TOK__RPAREN;
	}
	
	local leftBracket? (t:Token):Boolean ==  {
		assert(special? t);
		special t = TOK__LBRACKET;
	}
	
	local rightBracket? (t:Token):Boolean == {
		assert(special? t);
		special t = TOK__RBRACKET;
	}
		
	local comma? (t:Token):Boolean == {
		assert(special? t);
		special t = TOK__COMMA;
	}
		
	local eof? (t:Token):Boolean == {
		special? t and (special t = TOK__EOF);
	}
	
	local eoexpr? (t:Token):Boolean == {
		special? t and (special t = TOK__EOEXPR);
	}

	local assign? (t:Token):Boolean == {
		assert(operator? t);
		uniqueId$(operator t) = UID__ASSIGN;
	}
		
	local prefix? (t:Token):Boolean == {
		assert(operator? t);
		uniqueId$(operator t) = UID__PREFIX;
	}	

	local Prefix (r:Rep):Partial TREE == {
		import from TREE, Boolean;
		free ctoken;
		TRACE("Prefix", ctoken);
		arg:ExprList := empty;
		prefixOp := operator ctoken;
		nextToken! r;
		~((special? ctoken) and (leftParen? ctoken)) => {
			r.errorCode := PARSER__LPAREN__MISSING;
			failed;
		}
		Prefix0(prefixOp, r);
	}

#if OLDERVERSION
	local Prefix0(prefixOp:ExpressionTreeOperator, r:Rep):Partial TREE == {
		import from Boolean, TREE;
		nextToken! r;
		expr: Partial TREE := expression (r);
		failed? expr => failed;
		arg:ExprList := [retract expr];
		while (special? ctoken) and (comma? ctoken) 
	      	      and (~failed? expr) repeat {
			nextToken! r;
			expr := expression(r);
			if ~failed? expr then arg := append!(arg, retract expr);
			else r.errorCode := PARSER__ERROR__IN__EXPR;
		}
		failed? expr => failed;
		~rightParen? ctoken => {
			r.errorCode := PARSER__RPAREN__MISSING;
			failed;
		} 
		nextToken! r;
		[apply (prefixOp, arg)];
	}
#endif

	local Prefix0(prefixOp:ExpressionTreeOperator, r:Rep):Partial TREE == {
		import from Boolean, TREE, ExprList;
		nextToken! r;
		(special? ctoken) and (rightParen? ctoken) => {
			nextToken! r; 
			[apply (prefixOp, empty)];
		}
		expr: Partial TREE := expression (r);
		failed? expr => failed;
		arg:ExprList := [retract expr];
		while (special? ctoken) and (comma? ctoken) 
			and (~failed? expr) repeat {
				nextToken! r;
				expr := expression(r);
				if ~failed? expr then
					arg := append!(arg, retract expr);
				else r.errorCode := PARSER__ERROR__IN__EXPR;
		}
		failed? expr => failed;
		~rightParen? ctoken => {
			r.errorCode := PARSER__RPAREN__MISSING;
			failed;
		} 
		nextToken! r;
		[apply (prefixOp, arg)];
	}

	local ListOp (r:Rep): Partial TREE == {
		import from TREE, Boolean;
		free ctoken;
		TRACE("List", ctoken);
		arg:ExprList := empty;
		if (~special? ctoken) or (~rightBracket? ctoken) then {
		    expr: Partial TREE := expression(r);
		    failed? expr => failed;
		    arg := append!(arg, retract expr);
		    while (special? ctoken) and (comma? ctoken)
		          and (~failed? expr) repeat {
		        nextToken! r;
			expr := expression(r);
			if ~failed? expr then arg := append!(arg, retract expr);
			else r.errorCode := PARSER__ERROR__IN__EXPR;
		    }
		    failed? expr => failed;
		    ~rightBracket? ctoken => {
			r.errorCode := PARSER__RBRACKET__MISSING;
			failed;
		    }
		}
		nextToken! r;
		[apply (ExpressionTreeList, arg)];
	}
	
	-- Prec can be called on non-operators (e.g. ")")
	local Prec(r:Rep, t:Token):SI == {
		operator? t => PrecOp(r, t);
		-1;
	}

	local PrecOp(r:Rep, t:Token):SI == {
		assert(operator? t);
		id := uniqueId$(operator t);
		id = UID__PLUS => r.prec(PARSER__PLUS);
		id = UID__MINUS => r.prec(PARSER__MINUS);
		id = UID__TIMES => r.prec(PARSER__TIMES);
		id = UID__DIVIDE => r.prec(PARSER__DIVIDE);
		id = UID__EXPT => r.prec(PARSER__POWER);
		-1; 
	}
		
	local Power(r:Rep):Partial TREE == {
		import from TREE, ExpressionTreeLeaf, Boolean;
		TRACE("Power ", ctoken);
		free ctoken;
		leaf? ctoken => {
			TRACE("Power: leaf ", ctoken);
			symbol?(lf := leaf ctoken) => {
				nextToken! r;
				special? ctoken and leftParen? ctoken => {
				    Prefix0(ExpressionTreePrefix symbol lf, r);					
				}  
				[extree lf];
			}
			nextToken! r;
			[extree lf];
		}
		special? ctoken and leftParen? ctoken => {
			TRACE("Power: leftParen ", ctoken);
			nextToken! r;
			expr:Partial TREE := expression r;
			if ~((special? ctoken) and (rightParen? ctoken)) then {
				r.errorCode := PARSER__RPAREN__MISSING;
				failed;
			}
			else {
				nextToken! r;
				expr;
			}
		}
		special? ctoken and leftBracket? ctoken => {
			TRACE("Power: leftBracket ", ctoken);
			nextToken! r;
			expr:Partial TREE := ListOp r;
		}
		operator? ctoken => PowerOp r;
		TRACE("Power: failed ", ctoken);
		r.errorCode := PARSER__ERROR__IN__EXPR;
		failed;
	}

	local PowerOp(r:Rep):Partial TREE == {
		import from TREE, ExpressionTreeLeaf, Boolean;
		free ctoken, lookahead;
		TRACE("PowerOp ", ctoken);
		assert operator? ctoken;
		prefix? ctoken => Prefix r;
		op := operator ctoken;
		prec := Prec(r, ctoken);
		nextToken! r;
		leaf? ctoken => { 
			lookahead! r;
			if prec >= Prec(r, lookahead) then {
				expr := op [extree leaf ctoken];
				nextToken! r;
			} else {
				pexpr := Term r;
				expr := op [retract pexpr];
			}
			[expr];
		}
		operator? ctoken and prefix? ctoken => {
			pexpr := Prefix r;
			if ~failed? pexpr then failed;
			else {
				expr := op [retract pexpr];
				nextToken! r;
				[expr];
			}
		}
		special? ctoken and leftParen? ctoken => {
			nextToken! r;
			pexpr:Partial TREE := expression r;
			if ~((special? ctoken) and (rightParen? ctoken)) then {
				r.errorCode := PARSER__RPAREN__MISSING;
				failed;
			}
			else {
				expr := op [retract pexpr];
				nextToken! r;
				[expr];
			}
		}
		r.errorCode := PARSER__ERROR__IN__EXPR;
		failed;
	}

	local Factor(r:Rep):Partial TREE == {
		import from Boolean, TREE;
		TRACE("Factor ", ctoken);
		free ctoken;
		expr := Power r;
		while (~eoexpr? ctoken) and Prec(r, ctoken) = PARSER__PREC2 
		      and (~failed? expr) repeat {
			expr := FactorOp(r, retract expr);
		}
		TRACE("Factor returns ", retract expr);
		expr;
	}

	local FactorOp(r:Rep, expr:TREE):Partial TREE == {
		TRACE("FactorOp ", ctoken);
 		assert operator? ctoken;
		free ctoken;
		op := operator ctoken;
		nextToken! r;
		powerExpr := Power r;
		if failed? powerExpr then failed;
		else [op [expr, retract powerExpr]];
	}

	local Term(r:Rep):Partial TREE == {
		import from Boolean, TREE;
		TRACE("Term ", ctoken);
		free ctoken;
		expr := Factor r;
		while (~eoexpr? ctoken) and (Prec(r, ctoken) = PARSER__PREC1) 
	 	      and (~failed? expr) repeat {
			expr := TermOp(r, retract expr);
		}
		TRACE("Term returns ", retract expr);
		expr;
	}

	local TermOp(r:Rep, expr:TREE):Partial TREE == {
		assert operator? ctoken;
		free ctoken;
		op := operator ctoken;
		nextToken! r;
		factorExpr := Factor r;
		if failed? factorExpr then failed;
		else [op [expr, retract factorExpr]];
	}

	local expression(r:Rep):Partial TREE == {
		import from Boolean, TREE;
		TRACE("expression ", ctoken);
		free ctoken;
		expr := Term r;
		while (~eoexpr? ctoken) and (Prec(r, ctoken) = PARSER__PREC0) 
		      and (~failed? expr) repeat {
			expr := expressionOp(r, retract expr);
		}
		TRACE("expression returns ", retract expr);
		expr;
	}

	local expressionOp(r:Rep,expr:TREE):Partial TREE=={
		TRACE("expressionOp ", ctoken);
		assert operator? ctoken;
		free ctoken;
		op := operator ctoken;
		nextToken! r;
		termExpr := Term r;
		if failed? termExpr then failed;
		else [op [expr, retract termExpr]];
	}

	parse!(self:%):Partial TREE == {
		import from Boolean, TREE;
		free ctoken;
		r:= rep self;
		nextToken! r;
		eof? ctoken => {
			r.errorCode := PARSER__EOF;
			failed;
		}
		r.errorCode := PARSER__NOERROR;
		pexpr:Partial TREE := expression r;
		if ~failed? pexpr then expr := retract pexpr;
		while ~eoexpr? ctoken and ~eof? ctoken
			and ~failed? pexpr repeat {
				if operator? ctoken and assign? ctoken then {
					nextToken! r;
					pexpr := expression(r);
					if ~failed? pexpr then 
						expr := ExpressionTreeAssign(
							[expr, retract pexpr]);
				}
				else {
					r.errorCode := PARSER__ASSIGN__MISSING;
					pexpr := failed;
				}
		}
		failed? pexpr => {
			while ~eoexpr? ctoken and ~eof? ctoken repeat {
				nextToken! r;
			}
			failed;
		}
		[expr];

	}
	
	eof?(self:%):Boolean == {
		(rep self).errorCode = PARSER__EOF;
	}
	
	lastError(self:%): SI == {
		(rep self).errorCode;
	}
}
