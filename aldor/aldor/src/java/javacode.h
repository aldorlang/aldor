#ifndef _JAVACODE_H
#define _JAVACODE_H
#include "javaobj.h"

enum jcoModifier {
	JCO_MOD_Public    = (1 << 0),
	JCO_MOD_Private   = (1 << 1),
	JCO_MOD_Protected = (1 << 2),
	JCO_MOD_Static    = (1 << 3),
	JCO_MOD_Final     = (1 << 4),
	JCO_MOD_Transient = (1 << 5),
	JCO_MOD_Volatile  = (1 << 6),
	JCO_MOD_MAX       = (1 << 7)
};

typedef Enum(jcoModifier) JcoModifier;

enum jcOperation {
	JCO_OP_Not,
	JCO_OP_LogAnd,
	JCO_OP_LogOr, 
	JCO_OP_And,
	JCO_OP_Or, 
	JCO_OP_XOr, 
	JCO_OP_Equals,
	JCO_OP_NEquals,
	JCO_OP_Assign,
	JCO_OP_Plus,
	JCO_OP_Minus,
	JCO_OP_Times,
	JCO_OP_Divide,
	JCO_OP_Modulo,
	JCO_OP_LT,
	JCO_OP_LE,
	JCO_OP_GT,
	JCO_OP_GE,
	JCO_OP_Negate,
	JCO_OP_TimesPlus,
	JCO_OP_ShiftUp,
	JCO_OP_ShiftDn
};
			  
typedef Enum(jcOperation) JcOperation;
#if 0
			   /* emacs gets confused by Enum(x) */
			   }
#endif

/* All creation functions take ownership of any passed
 * in JavaCode objects, strings and lists.
 */

extern JavaCode jcId(String id);
extern JavaCode jcNull();
extern JavaCode jcThis();
extern JavaCode jcTrue();
extern JavaCode jcFalse();

extern JavaCode jcGenericId(JavaCode root, JavaCodeList genArgs);

extern JavaCode jcClass(int modifiers, String comment, 
		     JavaCode id, JavaCode superclass,
		     JavaCodeList implList, JavaCodeList body);

extern JavaCode jcMethod(int modifiers, String comment, 
			 JavaCode retnType,
			 JavaCode id, JavaCodeList genArgs,
			 JavaCodeList args,
			 JavaCodeList exns, JavaCode body);

extern JavaCode jcConstructor(int modifiers, String comment, 
			      JavaCode className, JavaCodeList genArgs,
			      JavaCodeList args,
			      JavaCodeList exns, JavaCode body);

extern JavaCode jcDeclaration(int modifiers, 
			      JavaCode retnType,
			      JavaCode id, JavaCodeList genArgs,
			      JavaCode args,
			      JavaCodeList exns);
extern JavaCode jcParamDecl(int modifiers, JavaCode type, JavaCode id);
extern JavaCode jcInitialisation(int modifiers, JavaCode type, 
				 JavaCode id, JavaCode value);

extern JavaCode jcFile(JavaCode pkg, JavaCode name, JavaCodeList imports, JavaCode body);
extern JavaCodeList jcCollectImports(JavaCode code);
extern JavaCode jcDocumented(String comment, JavaCode code);
extern JavaCode jcComment(String comment);
extern JavaCode jcImportedId(String pkg, String name);
extern JavaCode jcImportedStaticId(String pkg, String clss, String name);
extern JavaCode jcLiteralString(String s);
extern JavaCode jcLiteralStringWithTerminalChar(String s);
extern JavaCode jcLiteralChar(String s);
extern JavaCode jcLiteralInteger(AInt i);
extern JavaCode jcLiteralIntegerFrString(String s);
extern JavaCode jcLiteralFloatFrString(String s);
extern JavaCode jcKeyword(Symbol sym);

extern JavaCode jcReturn(JavaCode c);
extern JavaCode jcReturnVoid();
extern JavaCode jcBreak(JavaCode label);
extern JavaCode jcContinue(JavaCode label);

extern JavaCode jcPackage(JavaCode pkgName);
extern JavaCode jcImport(JavaCode type);

extern JavaCode jcBlock(JavaCode l);
extern JavaCode jcBlockNoNL(JavaCode body);
extern JavaCode jcIf(JavaCode test, JavaCode stmt);
extern JavaCode jcWhile(JavaCode test, JavaCode stmt);
extern JavaCode jcSwitch(JavaCode test, JavaCodeList body);
extern JavaCode jcCaseLabel(JavaCode arg);
extern JavaCode jcThrow(JavaCode arg);

extern JavaCode jcOp(JcOperation op, JavaCodeList args);
extern JavaCode jcBinOp(JcOperation op, JavaCode e1, JavaCode e2);
extern JavaCode jcNot(JavaCode expr);
extern JavaCode jcNegate(JavaCode expr);

extern JavaCode jcAssign(JavaCode lhs, JavaCode rhs);
extern JavaCode jcMemRef(JavaCode lhs, JavaCode rhs);
extern JavaCode jcCast(JavaCode type, JavaCode val);

extern JavaCode jcStatement(JavaCode stmt);
extern JavaCode jcSeq(JavaCodeList lst);
extern JavaCode jcSeqV(int n, ...);
extern JavaCode jcCommaSeq(JavaCodeList lst);
extern JavaCode jcCommaSeqP(int n, va_list l);
extern JavaCode jcSpaceSeq(JavaCodeList lst);
extern JavaCode jcSpaceSeqV(int n, ...);
extern JavaCode jcNLSeq(JavaCodeList lst);
extern JavaCode jcNLSeqV(int n, ...);

extern JavaCode jcParens(JavaCode c);
extern JavaCode jcBraces(JavaCode c);
extern JavaCode jcSqBrackets(JavaCode c);
extern JavaCode jcABrackets(JavaCode c);

extern JavaCode jcApply(JavaCode c, JavaCodeList args);
extern JavaCode jcApplyV(JavaCode c, int n, ...);
extern JavaCode jcApplyP(JavaCode c, int n, va_list argp);
extern JavaCode jcApplyMethod(JavaCode obj, JavaCode id, JavaCodeList args);
extern JavaCode jcApplyMethodV(JavaCode obj, JavaCode id, int n, ...);

extern JavaCode jcGenericMethodName(JavaCode methodName, JavaCodeList genArgs);
extern JavaCode jcGenericMethodNameV(JavaCode methodName, int n, ...);

extern JavaCode jcNAry(JavaCode t);
extern JavaCode jcArrayOf(JavaCode t);
extern JavaCode jcArrayNew(JavaCode t, JavaCode sz);
extern JavaCode jcArrayRef(JavaCode arr, JavaCode idx);

extern JavaCode jcConstruct(JavaCode type, JavaCodeList l);
extern JavaCode jcConstructV(JavaCode type, int n, ...);
extern JavaCode jcConstructBase(JavaCode type, JavaCode body);
extern JavaCode jcConstructSubclass(JavaCode type, JavaCodeList args, JavaCode body);

extern JavaCode jcConditional(JavaCode test, JavaCode c1, JavaCode c2);
/*
 * :: Utility methods
 */
extern JavaCodeList jcCollectImports(JavaCode clss);
extern SExpr        jcNodeSExpr(JavaCode code);
extern void         jcNodePrint(JavaCodePContext ctxt, JavaCode code);

extern Bool         jcIsLegalClassName(String word);
extern JavaCode     jcIdFrImported(JavaCode id);
extern JavaCode     jcImportedIdFrString(String str);
extern String       jcImportedIdName(JavaCode);
extern String       jcImportedIdPkg(JavaCode);

extern JavaCode     jcImportedStaticIdFrString(String str);
extern String       jcImportedStaticIdName(JavaCode);
extern String       jcImportedStaticIdClass(JavaCode);
extern String       jcImportedStaticIdPkg(JavaCode);

extern String       jcIdName(JavaCode);

#endif
