%{
# include	"zacc.h"
# include	"zaccgram.h"

# define	yywrap()	1
# define        YY_SKIP_YYWRAP        /* (YC) Strange: defining YY_SKIP_YYWRAP is necessary to say that yywrap is overriden ... If not defined we get an error saying "lex.yy.c:544:25: macro "yywrap" passed 1 arguments, but takes just 0", where the line indicated is simply a prototype declaration!! */

# define	See(z)	{token((z),(char *) yytext);}
# define	Ret(z)	{token((z),(char *) yytext); return(z);}

%}

Alpha		[a-zA-Z]
Num		[0-9]
Hex		[0-9a-fA-F]
Id0		[`$_a-zA-Z]
Id1		[$_a-zA-Z0-9]

%%

{Id0}{Id1}*			{ Ret(TK_Id); }
{Num}+|0x{Hex}+			{ Ret(TK_Num); }
"\""([^"]|"\\\"")*"\""		{ Ret(TK_String); }
"'"([^']|"\'")*"'"		{ Ret(TK_Char); }
"/*"([^*]|"*"[^/])*"*/"		{ }
[\ \t]+			        { See(TK_Space); }
"\n"				{ See(TK_NewLine); }
":"				{ Ret(TK_Colon); }
";"				{ Ret(TK_Semicolon); }
","				{ Ret(TK_Comma); }
"|"				{ Ret(TK_VBar); }
"("				{ Ret(TK_OPren); }
")"				{ Ret(TK_CPren); }
"{"				{ Ret(TK_OBrace); }
"}"				{ Ret(TK_CBrace); }
"<"				{ Ret(TK_OAngle); }
">"				{ Ret(TK_CAngle); }
"%{"				{ Ret(TK_OPct); }
"%}"				{ Ret(TK_CPct); }
"%%"				{ Ret(TK_PctPct); }
"%default-token-type"		{ Ret(TK_PctTokenType); }
"%default-rule-type"		{ Ret(TK_PctRuleType); }
"%include-enum"			{ Ret(TK_PctIncludeEnum); }
"%"{Id1}*			{ Ret(TK_PctId); }
.				{ Ret(TK_Other); }
