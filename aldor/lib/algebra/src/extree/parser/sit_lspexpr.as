----------------------- sit_lspexpr.as ---------------------------------
--
-- Lisp expression parser
--
-- Copyright (c) Manuel Bronstein 1995
-- Copyright (c) INRIA 1999, Version 0.1.12
-- Logiciel Sum^it ©INRIA 1999, dans sa version 0.1.12
-- Copyright (c) Swiss Federal Polytechnic Institute Zurich, 1995-97
--
-- grammar: (EBNF notation)
--   Expr       := Leaf | "(" Prefix ")" | "(" List ")"
--   Prefix     := op Expr List
--   List	:= "" | Expr List
-----------------------------------------------------------------------------

#include "algebra"
#include "algebrauid"

#include "alg_tokens"

macro {
	SI	== MachineInteger;
	OP	== ExpressionTreeOperator;
	PTREE	== Partial ExpressionTree;
	TEXT	== TextReader;
}

#if ALDOC
\thistype{LispExpressionParser}
\History{Manuel Bronstein}{25/09/96}{created}
\Usage{import from \this}
\Descr{\this~implements lisp expression parsers.}
\begin{exports}
\category{\astype{ParserReader}}\\
\end{exports}
#endif
 
LispExpressionParser: ParserReader == add {
	macro {
		Rep       == Record(sreader: TEXT, errorCode: SI);
		ExprList  == List ExpressionTree;	
	}
	
	import from SI, Rep;

	parser(reader:TEXT):%	== per [reader, 0];
	eof?(p:%):Boolean		== lastError p = PARSER__EOF;
	lastError(p:%):SI		== rep(p).errorCode;
	local reader(p:%):TEXT	== rep(p).sreader;
	local eof?(t:Token):Boolean	== special? t and (special t =TOK__EOF);
	local setError!(p:%, n:SI):%	== { rep(p).errorCode := n; p; }

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
	
	local nextToken!(r:%):Token == {
		import from TEXT, Scanner;
		ctoken := scan! reader r;
#if TRACE
		import from TextWriter, Character, String;
                if eoexpr? ctoken then stderr << "eoexpr";
                else if eof? ctoken then stderr << "eof";
                else if operator? ctoken then {
                        if assign? ctoken then stderr << ":="
                        else printOperator (ctoken);
                }
                else if leaf? ctoken then stderr << "leaf";
                else if special? ctoken then {
                        stderr << "special";
                        if comma? ctoken then stderr << ",";
                        else if leftParen? ctoken then stderr << "(";
                        else if rightParen? ctoken then stderr << ")";
                }
                else if prefix? ctoken then stderr << "prefix";
                else stderr << "????";
                stderr << newline;
#endif
		ctoken;
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
		
	local prefix? (t:Token):Boolean == {
		assert(operator? t);
		uniqueId$(operator t) = UID__PREFIX;
	}	

	local prefix!(parser:%, op:OP):PTREE == {
		import from Token, ExpressionTree, ExprList;
		failed?(uexpr := parse! parser) => return failed;
		args:ExprList := [retract uexpr];
		repeat {
			ctoken := nextToken! parser;
			special?(ctoken) and rightParen?(ctoken) =>
				return [op reverse! args];
			failed?(uexpr := parse!(parser, ctoken)) =>
				return failed;
			args := cons(retract uexpr, args);
		}
		never;
	}

	local list!(parser:%, ctoken:Token):PTREE == {
		import from Token, ExpressionTree, ExprList;
		args:ExprList := empty;
		repeat {
			special?(ctoken) and rightParen?(ctoken) =>
				return [ExpressionTreeLispList reverse! args];
			failed?(uexpr := parse!(parser, ctoken)) =>
				return failed;
			args := cons(retract uexpr, args);
			ctoken := nextToken! parser;
		}
		never;
	}

	parse!(parser:%):PTREE == {
		ctoken := nextToken! parser;
		parse!(parser, ctoken);
	}

	local parse!(parser:%, ctoken:Token):PTREE == {
		import from Boolean, ExpressionTreeLeaf, ExpressionTree;
		eof? ctoken => { setError!(parser, PARSER__EOF); failed; }
		setError!(parser, PARSER__NOERROR);
		leaf? ctoken => [extree leaf ctoken];
		~((special? ctoken) and (leftParen? ctoken)) => {
			setError!(parser, PARSER__LPAREN__MISSING);
			failed;
		}
		ctoken := nextToken! parser;
		eof? ctoken => { setError!(parser, PARSER__EOF); failed; }
		operator? ctoken => prefix!(parser, operator ctoken);
		list!(parser, ctoken);
	}
}
