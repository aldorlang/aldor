#!/bin/sh

#TSTAB="-wd+stabConst -wd+stab -wd+stabImport" #remove info about imported symbols"
TSCO="-wd+sco -wd+scoFluid -wd+scoStab"
TSTAB="-wd+stabConst -wd+stab"
TSYME="-wd+syme -wd+symeFill -wd+symeRefresh"
#TSEFO="-wd+sefoPrint -wd+sefoSubst -wd+sefoEqual -wd+sefoInter" #remove sefo intersection of symes
TSEFO="-wd+sefoPrint -wd+sefoSubst -wd+sefoEqual"
#TLIB="-wd+libConst -wd+lib -wd+libVerbose -wd+libRep -wd+libLazy"
TLIB=""
TTFORM="-wd+tf -wd+tfCascade -wd+tfCat -wd+tfCross -wd+tfExpr -wd+tfFloat -wd+tfHas -wd+tfImport -wd+tfMap -wd+tfMulti -wd+tfParent -wd+tfWith"
TTFS="-wd+tfs -wd+tfsExport -wd+tfsMulti -wd+tfsParent"
TTFS="-wd+tfs -wd+tfsExport -wd+tfsMulti -wd+tfsParent"
TTIP="-wd+tipAdd -wd+tipApply -wd+tipAssign -wd+tipBup -wd+tipDecl -wd+tipDeclare -wd+tipDefine -wd+tipFar -wd+tipId -wd+tipLit -wd+tipEmbed -wd+tipSef -wd+tipTdn -wd+titf -wd+titfOne -wd+titfStab"

TRACEFLAGS="$TSCO $TSTAB $TSYME $TSEFO $TLIB $TTFORM $TTFS $TTIP"
TRACEFILE=trace.log

echo "aldor -fc $TRACEFLAGS poly3.as > $TRACEFILE"
aldor -fc $TRACEFLAGS poly3.as > $TRACEFILE

