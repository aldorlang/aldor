#include "absyn.h"
#include "debug.h"
#include "sefo.h"
#include "tposs.h"
#include "abpretty.h"

local void tibupAuditExpr(Stab stab, AbSyn absyn);

Bool	tipAuditDebug	= false;
#define tipAuditDEBUG	DEBUG_IF(tipAudit)	afprintf

local Bool tiAuditOne(int idx, Stab stab, AbSyn ab);
local String tiAuditPrettyString(AbSyn ab, int n);

local int tiAuditExprIdx;
local int tiAuditCallIdx;
local int tiAuditBupCount;

void
tibupAudit(Stab stab, AbSyn ab)
{
	tiAuditCallIdx = tiAuditBupCount++;
	if (tipAuditDebug) {
		afprintf(dbOut, "([%d]TIBUP Audit\n", tiAuditCallIdx);
	}
	tiAuditExprIdx = 0;
	tibupAuditExpr(stab, ab);

	if (tipAuditDebug) {
		afprintf(dbOut, " [%d]TIBUP Audit)\n", tiAuditCallIdx);
	}
}

local void
tibupAuditExpr(Stab stab, AbSyn ab)
{
	int id = tiAuditExprIdx++;
	if (abStab(ab) != NULL) {
		stab = abStab(ab);
	}

	if (!abIsLeaf(ab)) {
		for (int i = 0; i < abArgc(ab); i += 1) {
			AbSyn abi = abArgv(ab)[i];
			tibupAuditExpr(stab, abi);
		}
	}

	tiAuditOne(id, stab, ab);
}

local Bool
tiAuditOne(int idx, Stab stab, AbSyn ab)
{
	Bool isSefo;
	TPoss tposs;
	
	isSefo = abIsSefo(ab);
	tposs = abReferTPoss(ab);

	if (tipAuditDebug) {
		String s = tiAuditPrettyString(ab, 50);
		afprintf(dbOut, "[%d:%d] %oAbState(%d): \"%s\" %pTPoss\n", tiAuditCallIdx, idx, abState(ab), tpossCount(tposs), s, tposs);
		if (tpossCount(tposs) == 0 || abState(ab) == AB_State_Error) {
			afprintf(dbOut, "[%d:%d] ***\n", tiAuditCallIdx, idx);
		}
	}
	tpossFree(tposs);
	return true;
}

// Something short and parenthesis balanced
local String
tiAuditPrettyString(AbSyn ab, int n)
{
	String s = abPretty(ab);
	s = strReplace(s, "\n", "\\n");
	if (strlen(s) > n) {
		s = strReplace(s, "(", "<o>");
		s = strReplace(s, ")", "<c>");
		s = strReplace(s, "{", "<O>");
		s = strReplace(s, "}", "<C>");
		s = strReplace(s, "[", "<B>");
		s = strReplace(s, "]", "<E>");
		s = strResize(s, n-3);
		s = strConcat(s, "...");
	}
	return s;
}

















