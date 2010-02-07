
%{
# include "zacc.h"
%}
%no-lines
%defines
%token  TK_Id TK_Num TK_String TK_Char TK_Comment TK_Space
%token  TK_Colon TK_Semicolon TK_Comma TK_VBar 
%token  TK_OPren TK_CPren TK_OBrace TK_CBrace TK_OAngle TK_CAngle 
%token  TK_OPct TK_CPct TK_PctPct TK_PctId
%token  TK_PctTokenType TK_PctRuleType TK_PctIncludeEnum
%token  TK_CTok TK_Other TK_NewLine

%type<str> TK_Id TK_String

%%

Goal:		Prolog TK_PctPct Rules TK_PctPct Epilog 
|		Prolog TK_PctPct Rules           Epilog
;

Nothing:      	;


Prolog:		{ startProlog(); } LogWords { endProlog(); } ;
Epilog:		{ wholeEpilog(); YYACCEPT; } ;

LogWords:	Nothing 
|		LogWords LogWord
|		LogWords { startCommand(); } Command { endCommand(); }
;	

Command:	TK_PctTokenType TK_OAngle TK_Id TK_CAngle
			{ seeTokenType($3); }
|		TK_PctRuleType  TK_OAngle TK_Id TK_CAngle
			{ seeRuleType($3); }
|		TK_PctIncludeEnum TK_String TK_Id 	 
			{ seeIncludeEnum($2, $3); }
;

Rules:		Nothing
|		Rules { startRule(); } Rule { endRule(); }
;
Rule:		PhraseName OptType TK_Colon RuleAlts TK_Semicolon
|		error TK_Semicolon 
;
RuleAlts:	RuleAlt 
| 		RuleAlts TK_VBar RuleAlt 
;
RuleAlt:	Nothing 			{ midRule(); }
| 		RuleAlt RulePart 
;
RulePart:	PhraseName
|		Action
|		TK_Num
|		TK_Char
|		TK_PctId
;

PhraseName:	{startPhraseName();} PhraseName0 {endPhraseName();} ;
PhraseName0:	TK_Id 				 { seeName($1); }
|		TK_Id TK_OPren Args TK_CPren 	 { seeName($1); }
;
Args:		TK_Id 				 { seeArg($1); }
|		Args TK_Comma TK_Id		 { seeArg($3); }
;
OptType:	Nothing
|		TK_OAngle TK_Id TK_CAngle 	 { seeType($2); }
;


Action:		{ startAction(); } TK_OBrace Code TK_CBrace {endAction();} 
;
Code:		Nothing
|		Code CWord
|		Code TK_OBrace Code TK_CBrace
;

/*
 * Token Classes
 */

Space:		TK_Comment
|		TK_Space
| 		TK_NewLine { incLineCount(); } Space
;

CWord:		Space
|		TK_Id
|		TK_Num
|		TK_String
|		TK_Char
|		TK_Colon
|		TK_Semicolon
|		TK_Comma
|		TK_VBar
|		TK_OPren
|		TK_CPren
|		TK_OAngle
|		TK_CAngle
|		TK_OPct
|		TK_CPct
|		TK_PctId
|		TK_Other
;

LogWord:	CWord
|		TK_OBrace
|		TK_CBrace
;



