%start ExternalDeclaration

%include-enum "cscan.h" ctokTag

%left     CTOK_Comma
%right    CTOK_Asst CTOK_StarAsst CTOK_DivAsst CTOK_ModAsst CTOK_PlusAsst CTOK_MinusAsst CTOK_UShAsst  CTOK_DShAsst  CTOK_AndAsst CTOK_XorAsst CTOK_OrAsst
%right    CTOK_Question CTOK_Colon
%left     CTOK_LOr
%left     CTOK_LAnd
%left     CTOK_Or
%left     CTOK_Xor
%left     CTOK_And 
%left     CTOK_EQ CTOK_NE 
%left     CTOK_LT CTOK_LE  CTOK_GT CTOK_GE 
%left     CTOK_USh CTOK_DSh 
%left     CTOK_Plus CTOK_Minus 
%left     CTOK_Star CTOK_Div CTOK_Mod 
%nonassoc PREF
%nonassoc POST
%{
#include <stdlib.h>
#include "cparse.h"
#include "ccode.h"

#define YYSTYPE CCode

YYSTYPE	yytree;
%}

%%

/* TranslationUnit : seq(ExternalDeclaration) ; */

ExternalDeclaration
: FunctionDefinition
	{ yytree = $$ = $1; YYACCEPT; }
| Declaration
	{ yytree = $$ = $1; YYACCEPT; }
| CTOK_EOF	
	{ yytree = $$ = $1; YYACCEPT; }
;

FunctionDefinition
:                           Declarator optseq(Declaration) CompoundStatement 
	{ $$ = ccNewFDef(0,  0, $1, $2, $3); }
| seq(DeclarationSpecifier) Declarator optseq(Declaration) CompoundStatement 
	{ $$ = ccNewFDef(0, $1, $2, $3, $4); }
;

Declaration
: seq(DeclarationSpecifier) optlist(InitDeclarator) CTOK_Semi
	{ $$ = ccDoTypedefs(ccNewDecl(0, $1, $2, $3)); }
;

DeclarationSpecifier
: StorageClassSpecifier
| TypeSpecifier
| TypeQualifier
;

StorageClassSpecifier
: CTOK_Auto			{ $$ = ccNewStoClass(CTOK_Auto,    $1); }
| CTOK_Register		 	{ $$ = ccNewStoClass(CTOK_Register,$1); }
| CTOK_Static			{ $$ = ccNewStoClass(CTOK_Static,  $1); }
| CTOK_Extern			{ $$ = ccNewStoClass(CTOK_Extern,  $1); }
| CTOK_Typedef			{ $$ = ccNewStoClass(CTOK_Typedef, $1); }
;

TypeQualifier
: CTOK_Const			{ $$ = ccNewTypeQual(CTOK_Const,   $1); }
| CTOK_Volatile		 	{ $$ = ccNewTypeQual(CTOK_Volatile,$1); }
;

TypeSpecifier
: CTOK_Void			{ $$ = ccNewTypeSpec(CTOK_Void,    $1); }
| CTOK_Char			{ $$ = ccNewTypeSpec(CTOK_Char,    $1); }
| CTOK_Short			{ $$ = ccNewTypeSpec(CTOK_Short,   $1); }
| CTOK_Int			{ $$ = ccNewTypeSpec(CTOK_Int,     $1); }
| CTOK_Long			{ $$ = ccNewTypeSpec(CTOK_Long,    $1); }
| CTOK_Float			{ $$ = ccNewTypeSpec(CTOK_Float,   $1); }
| CTOK_Double			{ $$ = ccNewTypeSpec(CTOK_Double,  $1); }
| CTOK_Signed			{ $$ = ccNewTypeSpec(CTOK_Signed,  $1); }
| CTOK_Unsigned		 	{ $$ = ccNewTypeSpec(CTOK_Unsigned,$1); }

| CTOK_Struct opt(Name) CTOK_OCurly seq(StructDeclaration) CTOK_CCurly
				{ $$ = ccNewSDef(CTOK_Struct,$1,$2,$3,$4,$5); }
| CTOK_Struct Name              { $$ = ccNewSRef(CTOK_Struct,$1,$2); }

| CTOK_Union opt(Name) CTOK_OCurly seq(StructDeclaration) CTOK_CCurly
				{ $$ = ccNewSDef(CTOK_Union,$1,$2,$3,$4,$5); }
| CTOK_Union Name               { $$ = ccNewSRef(CTOK_Union,$1,$2); }

| CTOK_Enum opt(Name) CTOK_OCurly list(Enumerator) CTOK_CCurly
				{ $$ = ccNewSDef(CTOK_Enum,$1,$2,$3,$4,$5); }
| CTOK_Enum Name                { $$ = ccNewSRef(CTOK_Enum,$1,$2); }

| CTOK_TypedefName
;

InitDeclarator
: Declarator
| Declarator CTOK_Asst Initializer
	{ $$ = ccNewInfix(CTOK_Asst,     $1,$2,$3); }
;

StructDeclaration
: seq(SpecifierQualifier) list(StructDeclarator) CTOK_Semi
	{ $$ = ccNewDecl(0, $1, $2, $3); }
;

SpecifierQualifier
: TypeSpecifier
| TypeQualifier
;

StructDeclarator
: Declarator
| Declarator CTOK_Colon Expression
	{ $$ = ccNewBitField(0, $1, $2, $3); }
;

Enumerator
: Identifier
| Identifier CTOK_Asst Expression
	{ $$ = ccNewInfix(CTOK_Asst,     $1,$2,$3); }
;

Declarator
: DirectDeclarator
| Pointer DirectDeclarator
	{ $$ = ccNewPtrDecl(0, $1, $2); }
;

DirectDeclarator
: Identifier
|                  CTOK_OParen  Declarator          CTOK_CParen
	{ $$ = ccNewParen(0, $1, $2, $3); }
| DirectDeclarator CTOK_OBrack opt(Expression)      CTOK_CBrack
	{ $$ = ccNewARef(0, $1, $2, $3, $4); }
| DirectDeclarator CTOK_OParen  ParameterTypeList   CTOK_CParen
	{ $$ = ccNewFCall(0, $1, $2, $3, $4); }
| DirectDeclarator CTOK_OParen  optlist(Identifier) CTOK_CParen
	{ $$ = ccNewFCall(0, $1, $2, $3, $4); }
;

Pointer
: CTOK_Star optseq(TypeQualifier)
	{ $$ = ccNewPtrStar(0, $1, $2,  0); }
| CTOK_Star optseq(TypeQualifier) Pointer
	{ $$ = ccNewPtrStar(0, $1, $2, $3); }
;

ParameterTypeList
: list(ParameterDeclaration)
	{ $$ = ccNewParam(0,          $1, 0, 0); }
| list(ParameterDeclaration) CTOK_Comma CTOK_DDDot
	{ $$ = ccNewParam(CTOK_DDDot, $1, $2, $3); }
;

ParameterDeclaration
: seq(DeclarationSpecifier) Declarator
	{ $$ = ccNewDecl(0, $1, $2, 0); }
| seq(DeclarationSpecifier) opt(AbstractDeclarator)
	{ $$ = ccNewDecl(0, $1, $2, 0); }
;

Initializer
: AssignmentExpression
| CTOK_OCurly list(Initializer) CTOK_CCurly
	{ $$ = ccNewInit(0, $1, $2, 0, $3); }
| CTOK_OCurly list(Initializer) CTOK_Comma CTOK_CCurly
	{ $$ = ccNewInit(0, $1, $2, $3, $4); }
;

TypeName
: seq(SpecifierQualifier) opt(AbstractDeclarator)
	{ $$ = ccNewType(0, $1, $2); }
;

AbstractDeclarator
: Pointer
| Pointer DirectAbstractDeclarator
	{ $$ = ccNewPtrDecl(0, $1, $2); }
|         DirectAbstractDeclarator
;

DirectAbstractDeclarator
:                          CTOK_OParen  AbstractDeclarator      CTOK_CParen
	{ $$ = ccNewParen(0, $1, $2, $3); }

|                          CTOK_OBrack opt(Expression) CTOK_CBrack
	{ $$ = ccNewARef(0, 0, $1, $2, $3); }
| DirectAbstractDeclarator CTOK_OBrack opt(Expression) CTOK_CBrack
	{ $$ = ccNewARef(0,$1, $2, $3, $4); }

|                          CTOK_OParen  opt(ParameterTypeList)  CTOK_CParen
	{ $$ = ccNewFCall(0, 0, $1, $2, $3); }
| DirectAbstractDeclarator CTOK_OParen  opt(ParameterTypeList)  CTOK_CParen
	{ $$ = ccNewFCall(0,$1, $2, $3, $4); }
;

Statement
: CompoundStatement
| opt(Expression) CTOK_Semi
	{ $$ = ccNewStat(0, $1, $2); }
| Identifier CTOK_Colon Statement
	{ $$ = ccNewLabeled(0, $1, $2, $3); }
| CTOK_Case Expression CTOK_Colon Statement
	{ $$ = ccNewCase(0, $1, $2, $3, $4); }
| CTOK_Default CTOK_Colon Statement
	{ $$ = ccNewDefault(0, $1, $2, $3); }
| CTOK_Switch CTOK_OParen Expression CTOK_CParen Statement
	{ $$ = ccNewSwitch(0, $1, $2, $3, $4, $5); }
| CTOK_If CTOK_OParen Expression CTOK_CParen Statement
	{ $$ = ccNewIf(0, $1, $2, $3, $4, $5, 0, 0); }
| CTOK_If CTOK_OParen Expression CTOK_CParen Statement
    CTOK_Else Statement
	{ $$ = ccNewIf(0, $1, $2, $3, $4, $5, $6, $7); }
| CTOK_While CTOK_OParen Expression CTOK_CParen
    Statement
	{ $$ = ccNewWhile(0, $1, $2, $3, $4, $5); }
| CTOK_Do Statement
    CTOK_While CTOK_OParen Expression CTOK_CParen CTOK_Semi
	{ $$ = ccNewDo(0, $1, $2, $3, $4, $5, $6, $7); }
| CTOK_For
    CTOK_OParen
      opt(Expression) CTOK_Semi
      opt(Expression) CTOK_Semi
      opt(Expression)
    CTOK_CParen
      Statement
	{ $$ = ccNewFor(0, $1, $2, $3, $4, $5, $6, $7, $8, $9); }
| CTOK_Goto Identifier CTOK_Semi
	{ $$ = ccNewGoto(0, $1, $2, $3); }
| CTOK_Continue CTOK_Semi
	{ $$ = ccNewContinue(0, $1, $2); }
| CTOK_Break CTOK_Semi
	{ $$ = ccNewBreak(0, $1, $2); }
| CTOK_Return opt(Expression) CTOK_Semi
	{ $$ = ccNewReturn(0, $1, $2, $3); }
;

CompoundStatement
: CTOK_OCurly optseq(Declaration) optseq(Statement) CTOK_CCurly
	{ $$ = ccNewCompound(0, $1, $2, $3, $4); }
;

Expression
: AssignmentExpression
| Expression CTOK_Comma AssignmentExpression
				   { $$ = ccNewInfix(CTOK_Comma,    $1,$2,$3); }
;

AssignmentExpression:	E;

E
: CastExpression
| E CTOK_Asst E 		   { $$ = ccNewInfix(CTOK_Asst,     $1,$2,$3); }
| E CTOK_StarAsst  E 		   { $$ = ccNewInfix(CTOK_StarAsst, $1,$2,$3); }
| E CTOK_DivAsst   E 		   { $$ = ccNewInfix(CTOK_DivAsst,  $1,$2,$3); }
| E CTOK_ModAsst   E 		   { $$ = ccNewInfix(CTOK_ModAsst,  $1,$2,$3); }
| E CTOK_PlusAsst  E 		   { $$ = ccNewInfix(CTOK_PlusAsst, $1,$2,$3); }
| E CTOK_MinusAsst E 		   { $$ = ccNewInfix(CTOK_MinusAsst,$1,$2,$3); }
| E CTOK_UShAsst   E 		   { $$ = ccNewInfix(CTOK_UShAsst,  $1,$2,$3); }
| E CTOK_DShAsst   E 		   { $$ = ccNewInfix(CTOK_DShAsst,  $1,$2,$3); }
| E CTOK_AndAsst   E 		   { $$ = ccNewInfix(CTOK_AndAsst,  $1,$2,$3); }
| E CTOK_XorAsst   E 		   { $$ = ccNewInfix(CTOK_XorAsst,  $1,$2,$3); }
| E CTOK_OrAsst    E 		   { $$ = ccNewInfix(CTOK_OrAsst,   $1,$2,$3); }
| E CTOK_Question Expression CTOK_Colon E
				   { $$ = ccNewQuest(0,       $1,$2,$3,$4,$5); }
| E CTOK_LOr  E  		   { $$ = ccNewInfix(CTOK_LOr,      $1,$2,$3); }
| E CTOK_LAnd E  		   { $$ = ccNewInfix(CTOK_LAnd,     $1,$2,$3); }
| E CTOK_Or   E  		   { $$ = ccNewInfix(CTOK_Or,       $1,$2,$3); }
| E CTOK_Xor  E  		   { $$ = ccNewInfix(CTOK_Xor,      $1,$2,$3); }
| E CTOK_And  E  		   { $$ = ccNewInfix(CTOK_And,      $1,$2,$3); }
| E CTOK_EQ E    		   { $$ = ccNewInfix(CTOK_EQ,       $1,$2,$3); }
| E CTOK_NE E    		   { $$ = ccNewInfix(CTOK_NE,       $1,$2,$3); }
| E CTOK_LT E    		   { $$ = ccNewInfix(CTOK_LT,       $1,$2,$3); }
| E CTOK_LE E    		   { $$ = ccNewInfix(CTOK_LE,       $1,$2,$3); }
| E CTOK_GT E    		   { $$ = ccNewInfix(CTOK_GT,       $1,$2,$3); }
| E CTOK_GE E    		   { $$ = ccNewInfix(CTOK_GE,       $1,$2,$3); }
| E CTOK_USh E   		   { $$ = ccNewInfix(CTOK_USh,      $1,$2,$3); }
| E CTOK_DSh E   		   { $$ = ccNewInfix(CTOK_DSh,      $1,$2,$3); }
| E CTOK_Plus E  		   { $$ = ccNewInfix(CTOK_Plus,     $1,$2,$3); }
| E CTOK_Minus E 		   { $$ = ccNewInfix(CTOK_Minus,    $1,$2,$3); }
| E CTOK_Star E  		   { $$ = ccNewInfix(CTOK_Star,     $1,$2,$3); }
| E CTOK_Div  E  		   { $$ = ccNewInfix(CTOK_Div,      $1,$2,$3); }
| E CTOK_Mod  E  		   { $$ = ccNewInfix(CTOK_Mod,      $1,$2,$3); }
;

CastExpression
: UnaryExpression
| CTOK_OParen TypeName CTOK_CParen CastExpression
				   { $$ = ccNewCast(0, $1, $2, $3, $4); }
;

UnaryExpression
: PostfixExpression
| CTOK_Inc UnaryExpression	   { $$ = ccNewPrefix(CTOK_Inc,   $1, $2); }
| CTOK_Dec UnaryExpression	   { $$ = ccNewPrefix(CTOK_Dec,   $1, $2); }
| CTOK_And CastExpression	   { $$ = ccNewPrefix(CTOK_And,   $1, $2); }
| CTOK_Star CastExpression	   { $$ = ccNewPrefix(CTOK_Star,  $1, $2); }
| CTOK_Plus CastExpression	   { $$ = ccNewPrefix(CTOK_Plus,  $1, $2); }
| CTOK_Minus CastExpression	   { $$ = ccNewPrefix(CTOK_Minus, $1, $2); }
| CTOK_Not CastExpression	   { $$ = ccNewPrefix(CTOK_Not,   $1, $2); }
| CTOK_LNot CastExpression	   { $$ = ccNewPrefix(CTOK_LNot,  $1, $2); }
| CTOK_Sizeof UnaryExpression	   { $$ = ccNewPrefix(CTOK_Sizeof,$1, $2); }
| CTOK_Sizeof CTOK_OParen TypeName CTOK_CParen
				   { $$ = ccNewPrefix(CTOK_Sizeof,$1,
					              ccNewParen(0,$2,$3,$4)); }
;

PostfixExpression
: PrimaryExpression
| PostfixExpression CTOK_OBrack Expression CTOK_CBrack
	                            { $$ = ccNewARef (0, $1,$2,$3,$4); }
| PostfixExpression CTOK_OParen optlist(AssignmentExpression) CTOK_CParen
	                            { $$ = ccNewFCall(0, $1,$2,$3,$4); }
| PostfixExpression CTOK_Dot      Identifier
	                            { $$ = ccNewInfix(CTOK_Dot,     $1,$2,$3); }
| PostfixExpression CTOK_PointsTo Identifier
	                            { $$ = ccNewInfix(CTOK_PointsTo,$1,$2,$3); }
| PostfixExpression CTOK_Inc       { $$ = ccNewPostfix(CTOK_Inc,   $1,$2); }
| PostfixExpression CTOK_Dec       { $$ = ccNewPostfix(CTOK_Dec,   $1,$2); }
;

PrimaryExpression
: Identifier
| Constant
| String
| CTOK_OParen Expression CTOK_CParen
	                            { $$ = ccNewParen(0, $1, $2, $3); }
;

Constant
: CTOK_IntegerConstant
| CTOK_CharacterConstant
| CTOK_FloatingConstant
| CTOK_EnumerationConstant
;

String
: CTOK_String
;

Identifier
: CTOK_Identifier
;

Name
: CTOK_Identifier
| CTOK_EnumerationConstant
| CTOK_TypedefName
;

opt(E)		: { $$ = 0; } | E ;

optlist(E) 	: { $$ = 0; } | list(E) ;

optseq(E) 	: { $$ = 0; } | seq(E) ;

list(E)
: E                     
| list(E) CTOK_Comma E
	{ $$ = ccNewInfix(CTOK_Comma, $1, $2, $3); }
;

seq(E)	
: E                     
| seq(E) E
	{ $$ = ccNewSeq(0, $1, $2); }
;

%%

#include <stdio.h>

static struct ctok *RecentToken = 0;
int
yylex()
{
	struct ctok	*tk;

	tk = cscan();
	if (!tk) return -1;
	RecentToken = tk;

	yylval = ccNewToken(tk->kind, tk->neutral, tk->string);
	return tk->kind;
}

void
yyerror(s)
	char	*s;
{
	fprintf(stderr, "File \"%s\", line %d: %s\n",
		cscanFileName, cscanLineNumber, s);
	if (RecentToken) 
		fprintf(stderr, "At or near token \"%s\" of kind %d\n",
			RecentToken->string, RecentToken->kind);
	exit(3);
}
