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
  JCO_MOD_MAX  = (1 << 6),
};

typedef Enum(jcoModifier) JcoModifier;

enum jcoOperation {
  JCO_OP_Assign,
  JCO_OP_Plus,
  JCO_OP_Minus,
  JCO_OP_Times,
  JCO_OP_Divide,
};
			  
typedef Enum(jcoOperation) JcoOperation;

/* All creation functions take ownership of any passed
 * in JavaCode objects, strings and lists.
 */

extern JavaCode jcClass(int modifiers, String comment, 
		     JavaCode id, JavaCode superclass,
		     JavaCodeList extendList, JavaCodeList body);
JavaCode 
jcMethod(int modifiers, String comment, 
	 JavaCode retnType,
	 JavaCode id, JavaCodeList genArgs,
	 JavaCodeList args,
	 JavaCodeList exns, JavaCode body);

extern JavaCode jcDeclaration(int modifiers, 
			      JavaCode retnType,
			      JavaCode id, JavaCodeList genArgs,
			      JavaCode args,
			      JavaCodeList exns);
extern JavaCode jcParamDecl(int modifiers, JavaCode type, JavaCode id);

extern JavaCode jcFile(JavaCode pkg, JavaCode name, JavaCodeList imports, JavaCode body);
extern JavaCodeList jcCollectImports(JavaCode code);
extern JavaCode jcId(String id);
extern JavaCode jcDocumented(String comment, JavaCode code);
extern JavaCode jcComment(String comment);
extern JavaCode jcImportedId(String pkg, String name);
extern JavaCode jcImportedStaticId(String pkg, String name);
extern JavaCode jcLiteralString(String s);
extern JavaCode jcLiteralInteger(AInt i);
extern JavaCode jcKeyword(Symbol sym);
extern JavaCode jcBinaryOp(JavaCodeClass c, JavaCode lhs, JavaCode rhs); // FIXME? Exposing classes
extern JavaCode jcAssign(JavaCode lhs, JavaCode rhs);
extern JavaCode jcStatement(JavaCode stmt);
extern JavaCode jcCommaSeq(JavaCodeList lst);
extern JavaCode jcSpaceSeq(JavaCodeList lst);
extern JavaCode jcSpaceSeqV(int n, ...);
extern JavaCode jcNLSeq(JavaCodeList lst);
extern JavaCode jcNLSeqV(int n, ...);
extern JavaCode jcNull();

extern JavaCode jcParens(JavaCode c);
extern JavaCode jcBraces(JavaCode c);
extern JavaCode jcSqBrackets(JavaCode c);
extern JavaCode jcABrackets(JavaCode c);

extern JavaCode jcApply(JavaCode c, JavaCodeList args);

/*
 * :: Utility methods
 */
JavaCodeList jcCollectImports(JavaCode clss);

#endif
