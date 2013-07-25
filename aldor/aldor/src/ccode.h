/*****************************************************************************
 *
 * ccoode.h: Structures for manipulating C programs.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _CCODE_H_
#define _CCODE_H_

#include "cport.h"

#include "srcpos.h"
#include "strops.h"
#include "symbol.h"

/******************************************************************************
 *
 * :: Grammar
 *
 ******************************************************************************
 *
 * This grammar is adapted from K+R 2nd Edition.
 * It shows how the ccoXxxx macros are used to represent C source code.
 *
 *  TranslationUnit
 *  : seq(ExternalDeclaration)		{ $$ = ccoUnit($1); } 
 *  ;
 *
 *  ExternalDeclaration
 *  : FunctionDefinition
 *  | Declaration
 *  | PreprocessorLine			{"Not part of K+R grammar";}
 *  ;
 *  
 *  FunctionDefinition
 *  : optseq(DeclarationSpecifier)
 *    Declarator optseq(Declaration) CompoundStatement 
 *          				{ $$ = ccoFDef($1, A, B, $4); }
 *  ;
 *  
 *  Declaration
 *  : seq(DeclarationSpecifier) optlist(InitDeclarator) ';'
 *          				{ $$ = ccoDecl($1,$2); }
 *  ;
 *  
 *  DeclarationSpecifier
 *  : StorageClassSpecifier
 *  | TypeSpecifier
 *  | TypeQualifier
 *  ;
 *  
 *  StorageClassSpecifier
 *  : 'auto'                         	{ $$ = ccoAuto(); }
 *  | 'register'                     	{ $$ = ccoRegister(); }
 *  | 'static'                       	{ $$ = ccoStatic(); }
 *  | 'extern'                       	{ $$ = ccoExtern(); }
 *  | 'typedef'                      	{ $$ = ccoTypedef(); }
 *  ;
 *  
 *  TypeQualifier
 *  : 'const'                        	{ $$ = ccoConst(); }
 *  | 'volatile'                     	{ $$ = ccoVolatile(); }
 *  ;
 *  
 *  TypeSpecifier
 *  : 'void'                         	{ $$ = ccoVoid(); }
 *  | 'char'                         	{ $$ = ccoChar(); }
 *  | 'short'                        	{ $$ = ccoShort(); }
 *  | 'int'                          	{ $$ = ccoInt(); }
 *  | 'long'                         	{ $$ = ccoLong(); }
 *  | 'float'                        	{ $$ = ccoFloat(); }
 *  | 'double'                       	{ $$ = ccoDouble(); }
 *  | 'signed'                       	{ $$ = ccoSigned(); }
 *  | 'unsigned'                     	{ $$ = ccoUnsigned(); }
 *  | 'struct' opt(Name) '{' seq(StructDeclaration) '}'
 *                                   	{ $$ = ccoStructDef($2,$4); }
 *  | 'union'  opt(Name) '{' seq(StructDeclaration) '}'
 *                                   	{ $$ = ccoUnionDef($2,$4); }
 *  | 'enum'   opt(Name) '{' list(Enumerator) '}'
 *                                   	{ $$ = ccoEnumDef($2,$4); }
 *  | 'struct' Name                  	{ $$ = ccoStructRef($2); }
 *  | 'union'  Name                  	{ $$ = ccoUnionRef($2); }
 *  | 'enum'   Name                  	{ $$ = ccoEnumRef($2); }
 *  
 *  | TYPEDEF_NAME                   	{ $$ = ccoTypedefId(ccoIdOf(yylval)); }
 *  ;
 *  
 *  InitDeclarator
 *  : Declarator
 *  | Declarator '=' Initializer	{ $$ = ccoAsst($1,$3); }
 *  ;
 *  
 *  StructDeclaration
 *  : seq(SpecifierQualifier) list(StructDeclarator) ';'
 *          				{ $$ = ccoDecl($1, $2); }
 *  ;
 *  
 *  SpecifierQualifier
 *  : TypeSpecifier
 *  | TypeQualifier
 *  ;
 *  
 *  StructDeclarator
 *  : Declarator
 *  | Declarator ':' Expression		{ $$ = ccoBitField($1, $3); }
 *  ;
 *  
 *  Enumerator
 *  : IDENTIFIER
 *  | IDENTIFIER '=' Expression		{ $$ = ccoAsst($1, $3); }
 *  ;
 *  
 *  Declarator
 *  : DirectDeclarator
 *  | Pointer(DirectDeclarator)
 *  ;
 *  
 *  DirectDeclarator
 *  : IDENTIFIER
 *  | '(' Declarator ')'   		{ $$ = $2; }
 *  | DirectDeclarator '[' opt(Expression)     ']'   
 *					{ $$ = ccoARef($1, $3); }
 *  | DirectDeclarator '(' ParameterTypeList   ')'   
 *					{ $$ = ccoFCall($1, $3); }
 *  | DirectDeclarator '(' optlist(IDENTIFIER) ')'   
 *					{ $$ = ccoFCall($1, $3); }
 *  ;
 *  
 *  Pointer(A)
 *  : '*' OptQual(A) 			{ $$ = ccoPreStar($2); }
 *  | '*' OptQual(Pointer(A)) 		{ $$ = ccoPreStar($2); }
 *  ;
 *  
 *  OptQual(A)
 *  : A
 *  | TypeSpecifier OptQual(A) 		{ $$ = ccoQual($1, $2); }
 *  ;
 *  
 *  ParameterTypeList
 *  : list(ParameterDeclaration)
 *  ;
 *  
 *  ParameterDeclaration
 *  : seq(DeclarationSpecifier) Declarator
 *          				{ $$ = ccoParam(extractID($2),$1,$2); }
 *  | seq(DeclarationSpecifier) opt(AbstractDeclarator)
 *          				{ $$ = ccoParam(extractID($2),$1,$2); }
 *  | '...'
 *          				{ $$ = ccoVAParam(); }
 *  ;
 *  
 *  Initializer
 *  : AssignmentExpression
 *  | '{' list(Initializer) '}' 	{ $$ = ccoInit($2); }
 *  | '{' list(Initializer) ',' '}' 	{ $$ = ccoInit($2); }
 *  ;
 *  
 *  TypeName
 *  : SpecifierQualifier opt(AbstractDeclarator)  
 *					{ $$ = ccoQual($1, $2); }
 *  | SpecifierQualifier TypeName    	{ $$ = ccoQual($1, $2); }
 *  ;
 *  
 *  AbstractDeclarator
 *  : Pointer(Nothing)
 *  | Pointer(DirectAbstractDeclarator)
 *  |         DirectAbstractDeclarator
 *  ;
 *  
 *  DirectAbstractDeclarator
 *  :    '(' AbstractDeclarator ')'	{ $$ = $2; }
 *  |    '[' opt(Expression)        ']'	{ $$ = ccoARef(0, $2); }
 *  |    '(' opt(ParameterTypeList) ')' { $$ = ccoFCall(0, $2); }
 *  | DirectAbstractDeclarator '[' opt(Expression) ']'
 *          				{ $$ = ccoARef($1, $3); }
 *  | DirectAbstractDeclarator '('  opt(ParameterTypeList)  ')'
 *          				{ $$ = ccoFCall($1, $3); }
 *  ;
 *  
 *  Statement
 *  : CompoundStatement
 *  | opt(Expression) ';' 		{ $$ = ccoStat($1); }
 *  | IDENTIFIER ':' Statement 		{ $$ = ccoLabel($1, $3); }
 *  | 'case' Expression ':' Statement 	{ $$ = ccoCase($2, $4); }
 *  | 'default' ':' Statement 		{ $$ = ccoDefault($3); }
 *  | 'switch' '(' Expression ')' Statement
 *					{ $$ = ccoSwitch($3, $5); }
 *  | 'if' '(' Expression ')' Statement { $$ = ccoIf($3, $5, 0); }
 *  | 'if' '(' Expression ')' Statement 'else' Statement
 *          				{ $$ = ccoIf($3, $5, $7); }
 *  | 'while' '(' Expression ')' Statement
 *          				{ $$ = ccoWhile($3, $5); }
 *  | 'do' Statement 'while' '(' Expression ')' ';'
 *          				{ $$ = ccoDo($2, $5); }
 *  | 'for' '('opt(Expression)';'opt(Expression)';'opt(Expression)')' Statement
 *          				{ $$ = ccoFor($3, $5, $7, $9); }
 *  | 'goto' IDENTIFIER ';' 		{ $$ = ccoGoto($2); }
 *  | 'continue' ';' 			{ $$ = ccoContinue(); }
 *  | 'break' ';' 			{ $$ = ccoBreak(); }
 *  | 'return' opt(Expression) ';' 	{ $$ = ccoReturn($2); }
 *  ;
 *  
 *  CompoundStatement
 *  : '{' optseq(DeclarationOrStatement) '}'
 *          				{ $$ = ccoCompound($2); }
 *  ;
 *  
 *  DeclarationOrStatement
 *  : Declaration
 *  | Statement
 *  | Comment				{"Not part of K+R grammar";}
 *  ;
 *  
 *  Expression
 *  : AssignmentExpression
 *  | Expression ',' AssignmentExpression
 *                                     { $$ = ccoComma($1,$3); }
 *  ;
 *  
 *  AssignmentExpression:   E;
 *  
 *  E
 *  : CastExpression
 *  | E '=' E                  		{ $$ = ccoAsst       ($1,$3); }
 *  | E '*='  E                		{ $$ = ccoStarAsst   ($1,$3); }
 *  | E '/='   E               		{ $$ = ccoDivAsst    ($1,$3); }
 *  | E '%='   E               		{ $$ = ccoModAsst    ($1,$3); }
 *  | E '+='  E                		{ $$ = ccoPlusAsst   ($1,$3); }
 *  | E '-=' E                 		{ $$ = ccoMinusAsst  ($1,$3); }
 *  | E '<<='   E              		{ $$ = ccoUShAsst    ($1,$3); }
 *  | E '>>='   E              		{ $$ = ccoDShAsst    ($1,$3); }
 *  | E '&='   E               		{ $$ = ccoAndAsst    ($1,$3); }
 *  | E '^='   E               		{ $$ = ccoXorAsst    ($1,$3); }
 *  | E '|='    E              		{ $$ = ccoOrAsst     ($1,$3); }
 *  | E '?' Expression ':' E   		{ $$ = ccoQuest      ($1,$3,$5); }
 *  | E '||'  E                		{ $$ = ccoLOr        ($1,$3); }
 *  | E '&&' E                 		{ $$ = ccoLAnd       ($1,$3); }
 *  | E '|'   E                		{ $$ = ccoOr         ($1,$3); }
 *  | E '^'  E                 		{ $$ = ccoXor        ($1,$3); }
 *  | E '&'  E                 		{ $$ = ccoAnd        ($1,$3); }
 *  | E '==' E                 		{ $$ = ccoEQ         ($1,$3); }
 *  | E '!=' E                 		{ $$ = ccoNE         ($1,$3); }
 *  | E '<' E                  		{ $$ = ccoLT         ($1,$3); }
 *  | E '<=' E                 		{ $$ = ccoLE         ($1,$3); }
 *  | E '>' E                  		{ $$ = ccoGT         ($1,$3); }
 *  | E '>=' E                 		{ $$ = ccoGE         ($1,$3); }
 *  | E '<<' E                 		{ $$ = ccoUSh        ($1,$3); }
 *  | E '>>' E                 		{ $$ = ccoDSh        ($1,$3); }
 *  | E '+' E                  		{ $$ = ccoPlus       ($1,$3); }
 *  | E '-' E                  		{ $$ = ccoMinus      ($1,$3); }
 *  | E '*' E                  		{ $$ = ccoStar       ($1,$3); }
 *  | E '/'  E                 		{ $$ = ccoDiv        ($1,$3); }
 *  | E '%'  E                 		{ $$ = ccoMod        ($1,$3); }
 *  ;
 *  
 *  CastExpression
 *  : UnaryExpression
 *  | '(' TypeName ')' CastExpression	{ $$ = ccoCast($2, $4); }
 *  ;
 *  
 *  UnaryExpression
 *  : PostfixExpression
 *  | '++' UnaryExpression     		{ $$ = ccoPreInc  ($2); }
 *  | '--' UnaryExpression     		{ $$ = ccoPreDec  ($2); }
 *  | '&' CastExpression       		{ $$ = ccoPreAnd  ($2); }
 *  | '*' CastExpression       		{ $$ = ccoPreStar ($2); }
 *  | '+' CastExpression       		{ $$ = ccoPrePlus ($2); }
 *  | '-' CastExpression       		{ $$ = ccoPreMinus($2); }
 *  | '~' CastExpression       		{ $$ = ccoNot     ($2); }
 *  | '!' CastExpression       		{ $$ = ccoLNot    ($2); }
 *  | 'sizeof' UnaryExpression 		{ $$ = ccoSizeof  ($2); }
 *  | 'sizeof' '(' TypeName ')'		{ $$ = ccoSizeof  ($3); }
 *  ;
 *  
 *  PostfixExpression
 *  : PrimaryExpression
 *  | PostfixExpression '[' Expression ']'
 *                                      { $$ = ccoARef    ($1,$3); }
 *  | PostfixExpression '(' optlist(AssignmentExpression) ')'
 *                                      { $$ = ccoFCall   ($1,$3); }
 *  | PostfixExpression '.'  IDENTIFIER { $$ = ccoDot     ($1,$3); }
 *  | PostfixExpression '->' IDENTIFIER { $$ = ccoPointsTo($1,$3); }
 *  | PostfixExpression '++'       	{ $$ = ccoPostInc ($1); }
 *  | PostfixExpression '--'       	{ $$ = ccoPostDec ($1); }
 *  | PostfixExpression '*'       	{ $$ = ccoPostStar($1); }
 *  ;
 *  
 *  PrimaryExpression
 *  : IDENTIFIER
 *  | STRING
 *  | Constant
 *  | '(' Expression ')' 		{ $$ = $2; }
 *  ;
 *  
 *  Constant
 *  : INTEGER_CONSTANT
 *  | CHARACTER_CONSTANT
 *  | FLOATING_CONSTANT
 *  | ENUMERATION_CONSTANT
 *  ;
 *  
 *  Name
 *  : IDENTIFIER
 *  | ENUMERATION_CONSTANT
 *  | TYPEDEF_NAME
 *  ;
 *  
 *  PreprocessorLine
 *  : PREPROCESSOR_LINE
 *  ;
 *
 *  Comment
 *  : COMMENT
 *  ;
 *
 *  Nothing         : { $$ = 0; } ;
 *  opt(E)          : Nothing | E ;
 *  optlist(E)      : Nothing | list(E) ;
 *  optseq(E)       : Nothing | seq(E) ;
 *  
 *  list(E)         : E | list(E) ',' E  { $$ = ccoMany_MakeFlat($1,$3); };
 *  seq(E)          : E | seq(E)      E  { $$ = ccoMany_MakeFlat($1,$2); };
 */


/******************************************************************************
 *
 * :: CCode Node Tags
 *
 *****************************************************************************/

enum ccodeTag {
    CCO_START,
	CCO_Unit = CCO_START,		/* file		 */

        /* Types and declaration words */
        CCO_Auto,		        /* auto           */
        CCO_Register,                   /* register       */
        CCO_Static,                     /* static         */
        CCO_Extern,                     /* extern         */
        CCO_Typedef,                    /* typedef        */

        CCO_Const,                      /* const          */
        CCO_Volatile,                   /* volatile       */

        CCO_Void,                       /* void           */
        CCO_Char,                       /* char           */
        CCO_Short,                      /* short          */
        CCO_Int,                        /* int            */
        CCO_Long,                       /* long           */
        CCO_Float,                      /* float          */
        CCO_Double,                     /* double         */
        CCO_Signed,                     /* signed         */
        CCO_Unsigned,                   /* unsigned       */
        CCO_TypedefId,                  /* id             */

        CCO_StructRef,                  /* struct a       */
        CCO_StructDef,                  /* struct {}      */
        CCO_UnionRef,                   /* union  a       */
        CCO_UnionDef,                   /* union  {}      */
        CCO_EnumRef,                    /* enum   a       */
        CCO_EnumDef,                    /* enum   {}      */

        CCO_Param,                      /* [a] int a      */
        CCO_VAParam,                    /* ...            */

        CCO_Decl,                       /* extern int a   */

        CCO_FDef,                       /* void f() {}    */
        CCO_Type,                       /* int (*)()      */    
        CCO_BitField,                   /* a : b          */
        CCO_Init,                       /* {i}            */
        CCO_Qual,                       /* * const **     */

        /* Label, Case and Default must occur within a Compound. */
        CCO_Label,                      /* i:             */
        CCO_Case,                       /* case e:        */
        CCO_Default,                    /* default:       */

        /* Executable statements */
        CCO_Compound,                   /* {a b c ...}    */
        CCO_Stat,                       /* e;             */
        CCO_Goto,                       /* goto id;       */
        CCO_Continue,                   /* continue;      */
        CCO_Break,                      /* break;         */
        CCO_Return,                     /* return e;      */
        CCO_If,                         /* if(b) t else e;*/
        CCO_Switch,                     /* switch(e) s    */
        CCO_While,                      /* while (b) s    */
        CCO_Do,                         /* do s while(e); */
        CCO_For,                        /* for(a;b;c) s   */

        /* Expression formers */
        CCO_Comma,                      /* a, b           */
        CCO_Asst,                       /* a  = b         */
        CCO_StarAsst,                   /* a *= b         */
        CCO_DivAsst,                    /* a /= b         */
        CCO_ModAsst,                    /* a %= b         */
        CCO_PlusAsst,                   /* a += b         */
        CCO_MinusAsst,                  /* a -= b         */
        CCO_UShAsst,                    /* a <<= b        */
        CCO_DShAsst,                    /* a >>= b        */
        CCO_AndAsst,                    /* a &= b         */
        CCO_XorAsst,                    /* a ^= b         */
        CCO_OrAsst,                     /* a |= b         */
        CCO_Quest,                      /* a ? b : c      */
        CCO_LOr,                        /* a || b         */
        CCO_LAnd,                       /* a && b         */
        CCO_Or,                         /* a | b          */
        CCO_Xor,                        /* a ^ b          */
        CCO_And,                        /* a & b          */
        CCO_EQ,                         /* a == b         */
        CCO_NE,                         /* a != b         */
        CCO_LT,                         /* a <  b         */
        CCO_LE,                         /* a <= b         */
        CCO_GT,                         /* a >  b         */
        CCO_GE,                         /* a >= b         */
        CCO_USh,                        /* a << b         */
        CCO_DSh,                        /* a >> b         */
        CCO_Plus,                       /* a + b          */
        CCO_Minus,                      /* a - b          */
        CCO_Star,                       /* a * b          */
        CCO_Div,                        /* a / b          */
        CCO_Mod,                        /* a % b          */
        CCO_Cast,                       /* (a) b          */
        CCO_Sizeof,                     /* sizeof a       */
        CCO_Not,                        /* ~a             */
        CCO_LNot,                       /* !a             */
        CCO_PreAnd,                     /* &a             */
        CCO_PreStar,                    /* *a             */
        CCO_PrePlus,                    /* +a             */
        CCO_PreMinus,                   /* -a             */
        CCO_PreInc,                     /* ++a            */
        CCO_PreDec,                     /* --a            */
        CCO_PostInc,                    /* a++            */
        CCO_PostDec,                    /* a--            */
        CCO_PostStar,                   /* a*             */
        CCO_ARef,                       /* a[b]           */
        CCO_FCall,                      /* a(b)           */
        CCO_Dot,                        /* a . b          */
        CCO_PointsTo,                   /* a->b           */
        CCO_Paren,                      /* (a)            */

        CCO_StringVal,                  /* "abcdefg"      */
        CCO_CharVal,                    /* 'c'            */
        CCO_IntVal,                     /* 8              */
        CCO_FloatVal,                   /* 8.9            */
        CCO_EnumId,                     /* id             */
        CCO_Id,                         /* id             */

	/* Sytlized lexical items */
	CCO_CppLine,			/* # define xyzzy */
	CCO_Comment,			/* (* xyzzy *)    */

        /* Meta-nodes */
        CCO_Many,                       /* many args, determined by context */
    CCO_LIMIT
};

typedef Enum(ccodeTag) CCodeTag;


/******************************************************************************
 *
 * :: Constructor Macros
 *
 *****************************************************************************/

#define ccoUnit(m)       ccoNew(CCO_Unit,1,m)

#define ccoAuto()        ccoNew(CCO_Auto,0)
#define ccoRegister()    ccoNew(CCO_Register,0)
#define ccoStatic()      ccoNew(CCO_Static,0)
#define ccoExtern()      ccoNew(CCO_Extern,0)
#define ccoTypedef()     ccoNew(CCO_Typedef,0)

#define ccoConst()       ccoNew(CCO_Const,0)
#define ccoVolatile()    ccoNew(CCO_Volatile,0)

#define ccoVoid()        ccoNew(CCO_Void,0)
#define ccoChar()        ccoNew(CCO_Char,0)
#define ccoShort()       ccoNew(CCO_Short,0)
#define ccoInt()         ccoNew(CCO_Int,0)
#define ccoLong()        ccoNew(CCO_Long,0)
#define ccoFloat()       ccoNew(CCO_Float,0)
#define ccoDouble()      ccoNew(CCO_Double,0)
#define ccoSigned()      ccoNew(CCO_Signed,0)
#define ccoUnsigned()    ccoNew(CCO_Unsigned,0)
#define ccoTypedefId(id) ccoNew(CCO_TypedefId,1,id)

#define ccoStructRef(i)  ccoNew(CCO_StructRef,1,i)
#define ccoStructDef(i,b)ccoNew(CCO_StructDef,2,i,b)
#define ccoUnionRef(i)   ccoNew(CCO_UnionRef,1,i)
#define ccoUnionDef(i,b) ccoNew(CCO_UnionDef,2,i,b)
#define ccoEnumRef(i)    ccoNew(CCO_EnumRef,1,i)
#define ccoEnumDef(i,b)  ccoNew(CCO_EnumDef,2,i,b)

#define ccoParam(i,w,v)  ccoNew(CCO_Param,3,i,w,v)
#define ccoVAParam()     ccoNew(CCO_VAParam,0)
#define ccoDecl(w,v)     ccoNew(CCO_Decl,2,w,v)
#define ccoFDef(s,h,d,b) ccoNew(CCO_FDef,4,s,h,d,b)
#define ccoType(r,d)	 ccoNew(CCO_Type,2,r,d)
#define ccoBitField(a,b) ccoNew(CCO_BitField,2,a,b)
#define ccoInit(i)       ccoNew(CCO_Init,1,i)
#define ccoQual(m,t)     ccoNew(CCO_Qual,2,m,t)

#define ccoLabel(i,s)    ccoNew(CCO_Label,2,i,s)
#define ccoCase(e,s)     ccoNew(CCO_Case,2,e,s)
#define ccoDefault(s)    ccoNew(CCO_Default,1,s)
#define ccoCompound(a)   ccoNew(CCO_Compound,1,a)
#define ccoStat(e)       ccoNew(CCO_Stat,1,e)
#define ccoGoto(i)       ccoNew(CCO_Goto,1,i)
#define ccoContinue()    ccoNew(CCO_Continue,0)
#define ccoBreak()       ccoNew(CCO_Break,0)
#define ccoReturn(e)     ccoNew(CCO_Return,1,e)
#define ccoIf(b,t,e)     ccoNew(CCO_If,3,b,t,e)
#define ccoSwitch(e,b)   ccoNew(CCO_Switch,2,e,b)
#define ccoWhile(b,s)    ccoNew(CCO_While,2,b,s)
#define ccoDo(s,e)       ccoNew(CCO_Do,2,s,e)
#define ccoFor(a,b,c,s)  ccoNew(CCO_For,4,a,b,c,s)

#define ccoComma(a,b)    ccoNew(CCO_Comma,2,a,b)
#define ccoAsst(a,b)     ccoNew(CCO_Asst,2,a,b)
#define ccoStarAsst(a,b) ccoNew(CCO_StarAsst,2,a,b)
#define ccoDivAsst(a,b)  ccoNew(CCO_DivAsst,2,a,b)
#define ccoModAsst(a,b)  ccoNew(CCO_ModAsst,2,a,b)
#define ccoPlusAsst(a,b) ccoNew(CCO_PlusAsst,2,a,b)
#define ccoMinusAsst(a,b)ccoNew(CCO_MinusAsst,2,a,b)
#define ccoUShAsst(a,b)  ccoNew(CCO_UShAsst,2,a,b)
#define ccoDShAsst(a,b)  ccoNew(CCO_DShAsst,2,a,b)
#define ccoAndAsst(a,b)  ccoNew(CCO_AndAsst,2,a,b)
#define ccoXorAsst(a,b)  ccoNew(CCO_XorAsst,2,a,b)
#define ccoOrAsst(a,b)   ccoNew(CCO_OrAsst,2,a,b)
#define ccoQuest(a,b,c)  ccoNew(CCO_Quest,3,a,b,c)
#define ccoLOr(a,b)      ccoNew(CCO_LOr,2,a,b)
#define ccoLAnd(a,b)     ccoNew(CCO_LAnd,2,a,b)
#define ccoOr(a,b)       ccoNew(CCO_Or,2,a,b)
#define ccoXor(a,b)      ccoNew(CCO_Xor,2,a,b)
#define ccoAnd(a,b)      ccoNew(CCO_And,2,a,b)
#define ccoEQ(a,b)       ccoNew(CCO_EQ,2,a,b)
#define ccoNE(a,b)       ccoNew(CCO_NE,2,a,b)
#define ccoLT(a,b)       ccoNew(CCO_LT,2,a,b)
#define ccoLE(a,b)       ccoNew(CCO_LE,2,a,b)
#define ccoGT(a,b)       ccoNew(CCO_GT,2,a,b)
#define ccoGE(a,b)       ccoNew(CCO_GE,2,a,b)
#define ccoUSh(a,b)      ccoNew(CCO_USh,2,a,b)
#define ccoDSh(a,b)      ccoNew(CCO_DSh,2,a,b)
#define ccoPlus(a,b)     ccoNew(CCO_Plus,2,a,b)
#define ccoMinus(a,b)    ccoNew(CCO_Minus,2,a,b)
#define ccoStar(a,b)     ccoNew(CCO_Star,2,a,b)
#define ccoDiv(a,b)      ccoNew(CCO_Div,2,a,b)
#define ccoMod(a,b)      ccoNew(CCO_Mod,2,a,b)
#define ccoCast(a,b)     ccoNew(CCO_Cast,2,a,b)
#define ccoSizeof(a)     ccoNew(CCO_Sizeof,1,a)
#define ccoNot(a)        ccoNew(CCO_Not,1,a)
#define ccoLNot(a)       ccoNew(CCO_LNot,1,a)
#define ccoPreAnd(a)     ccoNew(CCO_PreAnd,1,a)
#define ccoPreStar(a)    ccoNew(CCO_PreStar,1,a)
#define ccoPrePlus(a)    ccoNew(CCO_PrePlus,1,a)
#define ccoPreMinus(a)   ccoNew(CCO_PreMinus,1,a)
#define ccoPreInc(a)     ccoNew(CCO_PreInc,1,a)
#define ccoPreDec(a)     ccoNew(CCO_PreDec,1,a)
#define ccoPostInc(a)    ccoNew(CCO_PostInc,1,a)
#define ccoPostDec(a)    ccoNew(CCO_PostDec,1,a)
#define ccoPostStar(a)   ccoNew(CCO_PostStar,1,a)
#define ccoARef(a,b)     ccoNew(CCO_ARef,2,a,b)
#define ccoFCall(a,b)    ccoNew(CCO_FCall,2,a,b)
#define ccoDot(a,b)      ccoNew(CCO_Dot,2,a,b)
#define ccoPointsTo(a,b) ccoNew(CCO_PointsTo,2,a,b)
#define ccoParen(a)      ccoNew(CCO_Paren,1,a)

#define ccoStringVal(s)  ccoNew(CCO_StringVal,1,s)
#define ccoCharVal(c)    ccoNew(CCO_CharVal,1,c)
#define ccoIntVal(i)     ccoNew(CCO_IntVal,1,i)
#define ccoFloatVal(f)   ccoNew(CCO_FloatVal,1,f)
#define ccoEnumId(id)    ccoNew(CCO_EnumId,1,id)
#define ccoId(id)        ccoNew(CCO_Id,1,id)

#define ccoCppLine(s,t)  ccoNew(CCO_CppLine,2,s,t)
#define ccoComment(s)    ccoNew(CCO_Comment,1,s)

#define ccoMany0()                      ccoNew(CCO_Many,0)
#define ccoMany1(a)                     ccoNew(CCO_Many,1,a)
#define ccoMany2(a,b)                   ccoNew(CCO_Many,2,a,b)
#define ccoMany3(a,b,c)                 ccoNew(CCO_Many,3,a,b,c)
#define ccoMany4(a,b,c,d)               ccoNew(CCO_Many,4,a,b,c,d)
#define ccoMany5(a,b,c,d,e)             ccoNew(CCO_Many,5,a,b,c,d,e)
#define ccoMany6(a,b,c,d,e,f)           ccoNew(CCO_Many,6,a,b,c,d,e,f)
#define ccoMany7(a,b,c,d,e,f,g)         ccoNew(CCO_Many,7,a,b,c,d,e,f,g)
#define ccoMany8(a,b,c,d,e,f,g,h)       ccoNew(CCO_Many,8,a,b,c,d,e,f,g,h)
#define ccoMany9(a,b,c,d,e,f,g,h,i)     ccoNew(CCO_Many,9,a,b,c,d,e,f,g,h,i)

/******************************************************************************
 *
 * :: Structure Declarations
 *
 *****************************************************************************/

typedef union ccode     *CCode;

struct ccoHdr {
        BPack(CCodeTag) tag;
	SrcPos		pos;
};

struct ccoNode {
        struct ccoHdr   hdr;
        UShort          argc;
        CCode           argv[NARY];
};

struct ccoToken {
        struct ccoHdr   hdr;
        Symbol          symbol;
};


union ccode {
        struct ccoHdr   ccoHdr;
        struct ccoNode  ccoNode;
        struct ccoToken ccoToken;
};

/******************************************************************************
 *
 * :: ccoInfoTable
 *
 *****************************************************************************/

enum ccodeKind {
        CCOK_Keywd,     /* auto, char, ... */
        CCOK_Infix,     /* +,  -,  ->, ... */
        CCOK_Prefix,    /* ++, --, &, ... */
        CCOK_Postfix,   /* ++, -- */
        CCOK_Token,     /* ids, strings, ... */
        CCOK_Misc       /* for, decls, ... */
};

typedef Enum(ccodeKind) CCodeKind;

struct cco_info {
        BPack(CCodeTag)  tag;
        BPack(CCodeKind) kind;  
        BPack(int)       precedence;            
        BPack(Bool)      isLeftToRight;

        String           str;
};

extern struct cco_info ccoInfoTable[];

# define ccoInfo(i)     (ccoInfoTable[i])

/******************************************************************************
 *
 * :: Code types
 *
 *****************************************************************************/

/* Note: StandardC and OldC are mutually exclusive. */
enum ccodeMode {
        CCOM_StandardC = 1 << 0,
        CCOM_OldC      = 1 << 1,
	CCOM_LineNo    = 1 << 2
};

typedef Enum(ccodeMode) CCodeMode;


typedef void (*CCodeSrcFn)(SrcPos, String *fname, Length *lno, Length *cno);

/******************************************************************************
 *
 * :: Operations
 *
 *****************************************************************************/

#define         ccoTag(cco)     ((cco)->ccoHdr.tag)
#define         ccoPos(cco)     ((cco)->ccoHdr.pos)
#define         ccoArgc(cco)    ((cco)->ccoNode.argc)
#define         ccoArgv(cco)    ((cco)->ccoNode.argv)

#define         ccoIsToken(cco) (ccoInfo(ccoTag(cco)).kind == CCOK_Token)
#define         ccoIsKeywd(cco) (ccoInfo(ccoTag(cco)).kind == CCOK_Keywd)
#define         ccoIsExpr(cco)  (ccoInfo(ccoTag(cco)).precedence > 0)

extern CCode    ccoNewToken     (CCodeTag, Symbol);
extern CCode    ccoNewNode      (CCodeTag, int argc);
extern CCode    ccoNew          (CCodeTag tag, int argc, ...);

extern Bool	ccoTypeEqual	(CCode, CCode);
extern CCode    ccoCopy         (CCode);
extern void     ccoFree         (CCode);
extern int      ccoPrint        (FILE *, CCode, CCodeMode);
extern int	ccoPrintDb	(CCode);

#define         ccoIdOf(str)    ccoId(symIntern(str))
#define         ccoIntOf(num)   ccoIntVal(symIntern(strPrintf("%ldL",num)))
#define         ccoFloatOf(buf,flo) \
				ccoFloatVal(symIntern(DFloatSprint(buf,flo)))
#define		ccoCharOf(chr)	ccoCharVal(symIntern(strPrintf("%c",chr)))
#define		ccoEnumOf(str)	ccoEnumId(symIntern(str))
#define		ccoStringOf(str)ccoStringVal(symIntern(str))

#endif /* !_CCODE_H_ */
