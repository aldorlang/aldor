/*****************************************************************************
 *
 * tinfer.h: Type inference.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#ifndef _TINFER_H_
#define _TINFER_H_

#include "axlobs.h"

extern void	tinferInit();

extern TForm	typeInferAs(Stab, AbSyn, TForm);
			/*
			 * Infer the type of the given abstract syntax tree, in
			 * a context which requires a value of the given type.
			 */

extern TForm	typeInfer(Stab, AbSyn);
			/*
			 * Infer the type of the given abstract syntax tree.
			 * The resulting type of the expression is returned.
			 *
			 * On input, the abstract syntax tree must satisfy:
			 * 1) all scope nodes have abStab(ab).
			 * 2) all type context nodes have abTForm(ab).
			 * 3) all nodes have abUse(ab).
			 *
			 * On output the tree is annotated with:
			 * 1) all Id nodes have abSyme(ab).
			 * 2) all nodes have abTUnique(ab).
			 */

extern Bool	typeInferAudit(Stab, AbSyn);
			/*
			 * Verify the abstract syntax tree has consistent
			 * information after type inference.
			 */

extern void	typeInferCheck(Stab, AbSyn, TForm);
			/*
			 * Verify uses of types as domains and categories.
			 */

extern void	typeInferTForms(Stab);
			/*
			 * Type infer all type forms used in stab.
			 */

extern TForm	typeInferTForm(Stab, TForm);
			/*
			 * Add semantics to a type form.
			 */

extern AbSyn	abExpandDefs(Stab, AbSyn);
			/*
		 	* Expand any constant definitions.
		 	*/


/*
 * Internal functions used by ti_bup.c, ti_tdn.c and ti_sef.c.
 */

extern void		tiTfSefo		(Stab, TForm);
extern void		tiTfPushDefinee 	(AbSyn);
extern void		tiTfPopDefinee		(void);
extern Bool		tiTfDoDefault		(Sefo);
extern void		tiTfImportCascades	(Stab stab, TQualList tq);
extern SymeList		tiAddSymes		(Stab, AbSyn, TForm, TForm, SymeList *);
extern TForm		tiGetTForm		(Stab, AbSyn);
extern Syme		tiGetMeaning		(Stab, AbSyn, TForm);
extern Syme		tiGetExtendee		(Stab, AbSyn, TForm);
extern Bool		tiUnaryToRaw		(Stab, AbSyn, TForm);
extern Bool		tiRawToUnary		(Stab, AbSyn, TForm);
extern TForm		tiDefineFilter		(AbSyn, TForm);
extern TPoss		tiDefineTPoss		(AbSyn);

extern void		tiSetSoftMissing	(Bool);
extern Bool		tiIsSoftMissing		(void);

/*
 * Common things for ti_bup, ti_tdn, and ti_sef.
 */

#define			tfIsForeign(tf)		\
	(tfIsTheId(tf, ssymForeign) ||		\
	 (tfIsApply(tf) && tfApplyOp(tf)->abId.sym == ssymForeign))

extern Bool 	tiCheckLambdaType (TForm);
extern Bool	tiMergeSyme(Syme syme, SymeList symes);

extern Bool	tipAddDebug;
extern Bool	tipApplyDebug;
extern Bool	tipAssignDebug;
extern Bool	tipDeclareDebug;
extern Bool	tipDefineDebug;
extern Bool	tipFarDebug;
extern Bool	tipIdDebug;
extern Bool	tipLitDebug;
extern Bool	tipEmbedDebug;

#define tipAddDEBUG		DEBUG_IF(tipAdd)     afprintf
#define tipApplyDEBUG		DEBUG_IF(tipApply)   afprintf
#define tipAssignDEBUG		DEBUG_IF(tipAssign)  afprintf
#define tipDeclareDEBUG		DEBUG_IF(tipDeclare) afprintf
#define tipDefineDEBUG		DEBUG_IF(tipDefine)  afprintf
#define tipFarDEBUG		DEBUG_IF(tipFar)     afprintf
#define tipIdDEBUG		DEBUG_IF(tipId)      afprintf
#define tipLitDEBUG		DEBUG_IF(tipLit)     afprintf
#define tipEmbedDEBUG		DEBUG_IF(tipEmbed)   afprintf

#endif /* !_TINFER_H_ */
