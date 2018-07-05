$ The -Cfname option does not work FIXME!
$
$ \t-C fname=<name>\tUse <name> as the name of the generated file.\n\
$ \t               \tThis is also used as the prefix for any external ids.\n\
$
$          comsgdb.msg: Messages issued by the compiler.
$
$copyright Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
$
$
$quote     "
$set       ALDOR_GROUP
$
$
$          F -- errors (compiler stops)
$          E -- errors (compiler keeps going)
$          N -- notes (cross references to other src lines)
$          D -- details
$          W -- warnings
$          R -- remarks
$          H -- help
$          M -- interactive loop messages
$          P -- punctuation
$          X -- continuation
$          I -- initial messages

ALDOR_E_ExplicitMsg       "%s"
ALDOR_N_ExplicitMsg       "%s"

ALDOR_N_Here              ""

ALDOR_F_CmdBadOption	"Improper use of `%s' option.  Type `%s -help' for help."
ALDOR_F_CmdNoOption	"`%s' is not an option.  Type `%s -help' for help."
ALDOR_F_CmdCantUseEntry	"Could not use file `%s' as the main file."
ALDOR_F_CmdNoOutputDir	"The specified output directory does not exist."
ALDOR_W_CmdFunnyEntry	"Bogus main file name `%s' ignored."
ALDOR_W_DisableNotKeyword	"Unable to enable or disable `%s': not a keyword."

ALDOR_W_FtnVarStringRet	"You cannot receive variable-length strings from Fortran functions. Use FixedString() instead."
ALDOR_W_FtnNotFtnArg	"Unrecognised argument type for Fortran call."
ALDOR_F_MsgTooManyErrors  "Too many errors (use `-M emax=n' or `-M no-emax' to change the limit)."
ALDOR_R_MsgCountMessages  "This file had %d errors, %d warnings and %d remarks."
ALDOR_R_MsgAdviseDetails  "Use `aldor -M<n>' to get more or less detail."
ALDOR_R_MsgCondolences    "Sorry, your file did not compile."
ALDOR_R_MsgCongratulations "Congratulations, your file compiled!!"

ALDOR_P_MsgTagRemark      "(Remark) "
ALDOR_P_MsgTagWarning     "(Warning) "
ALDOR_P_MsgTagError       "(Error) "
ALDOR_P_MsgTagFatal       "(Fatal Error) "
ALDOR_P_MsgTagNote        "(Note %d) "
ALDOR_P_MsgPreview        "(Message Preview)"
ALDOR_P_MsgAfterMacEx     "(After Macro Expansion) "
ALDOR_P_MsgExpandedExpr   "Expanded expression was: "
ALDOR_P_MsgSposFileLine   "\"%s\", line %d: "
ALDOR_P_MsgSposLineChar   "[L%d C%d] "
ALDOR_P_MsgNote	       "Note %d"
ALDOR_P_MsgSeeNote        "(see %s)"
ALDOR_P_MsgCfNote         "(cf. L%d C%d)"
ALDOR_P_MsgCfFarNote      "(cf. \"%s\" L%d C%d)"
ALDOR_P_MsgConjunction    " and "

ALDOR_E_InclBadUnassert   "Unassert: property unknown: `%s'."
ALDOR_E_InclIfEof         "End of file encountered in `#if'."
ALDOR_E_InclInfinite      "Circular include files: %s."
ALDOR_E_InclUnbalElse     "Unbalanced `#else'."
ALDOR_E_InclUnbalElseif   "Unbalanced `#elseif'."
ALDOR_E_InclUnbalEndif    "Unbalanced `#endif'."

ALDOR_W_OldOptMain	"Deprecated option: use `-Fmain' instead of `-Faxlmain'"
ALDOR_W_OldOptAbsyn	"Deprecated option: use `-Fap' instead of `-Fax'"
ALDOR_W_OldTypeAbsyn	"Deprecated file suffix: use `.ap' instead of `.ax'"
ALDOR_W_OldMsgPrefix    "Deprecated message prefix: use `ALDOR_' instead of `_AXL'"

ALDOR_E_SysCmdBad         "Improper use of `%s' system command."
ALDOR_W_SysCmdUnknown     "Unknown system command."

ALDOR_E_ScanBadAftRad     "improper number (after radix specification)."
ALDOR_E_ScanBadChar       "bad character in input."
ALDOR_E_ScanBadExpon      "improper number (no digits in exponent)."
ALDOR_E_ScanBadRadix	"bad radix specification (2 <= radix <= 36)."
ALDOR_E_ScanNoDigits      "improper number (no digits)."
ALDOR_E_ScanOpenString    "string not closed."

ALDOR_E_NormMacDecl       "Macros must not be given return types."
ALDOR_E_NormMacBadBody    "Improper body in `macro' statement."
ALDOR_W_NormNoId		"Couldn't find identifier for documentation"
ALDOR_W_NormFornForeign	"Foreign(Foreign) is deprecated: use Foreign instead."
ALDOR_W_NormFornBuiltin	"Foreign(Builtin) is deprecated: use Builtin instead."
ALDOR_W_NormNullForeign	"Foreign() is deprecated: use Foreign instead."
ALDOR_E_MacBadDefn        "Improper form for macro definition."
ALDOR_E_MacBadParam       "Improper macro parameter (should be an identifier)."
ALDOR_E_MacBadParamDecl   "Macro parameters must not have type declarations."
ALDOR_E_MacBadArgc        "Macro used with incorrect number of arguments."
ALDOR_E_MacBadArg         "Macro cannot match the given argument."
ALDOR_E_MacInfinite       "Circular macro expansion: %s."
ALDOR_W_MacHides		"Definition of macro `%s' hides an outer definition."
ALDOR_W_MacRedefined	"Macro `%s' redefined in the same scope."

ALDOR_F_SyntaxOverflow    "Parser stack overflow (in state %d)."
ALDOR_E_SyntaxError       "Syntax error."
ALDOR_E_SyntaxErrorDebug  "Syntax error (in state %d)."
ALDOR_E_SyntaxErrorHuh    "%s."
ALDOR_E_SyntaxNoRecovery  "Cannot recover from earlier syntax errors."
ALDOR_E_SyntaxFullError   "Syntax error: %s"

ALDOR_E_LinUnbalanced     "Unbalanced `%s' -- missing `%s'."
ALDOR_W_LinUnbalanced     "Unbalanced `%s' -- missing `%s'."

ALDOR_F_LoadNotAbSyn      "This is not an abstract syntax operator."
ALDOR_F_LoadNotFoam       "This is not a Foam operator."
ALDOR_F_LoadNotList       "Expecting a (parenthesized) list here."
ALDOR_F_LoadNotUnary      "Expecting exactly one argument."
ALDOR_F_LoadNotString     "Expecting a \"quoted\" string here."
ALDOR_F_LoadNotSymbol     "Expecting a symbol here."
ALDOR_F_LoadNotInteger    "Expecting an integer."
ALDOR_F_LoadNotFloat      "Expecting a float."

ALDOR_W_CantUnKeywordRef	"Unable to consider this `ref' as a non-keyword (compiler limitation: try a simpler definition)."

ALDOR_E_ChkBadAssign      "Incorrect left-hand side of an assignment."
ALDOR_E_ChkBadDeclare     "Improper form appearing within a declaration."
ALDOR_E_ChkBadDefine      "Incorrect left-hand side of a definition. Check indentation of succeeding definitions, if any."
ALDOR_W_ChkBadDependent	"Bad dependent type detected (or compiler bug)"
ALDOR_E_ChkBadFor         "Expecting an identifier or single declaration after `for'."
ALDOR_E_ChkBadForm        "Improper form appearing in `%s' statement."
ALDOR_E_ChkBadGoto        "A goto must have a label's identifier as its target."
ALDOR_E_ChkBadLabel       "A label must consist of a single identifier."
ALDOR_E_ChkBadMLambda     "Improper macro expansion."
ALDOR_E_ChkBadMacro       "Improper macro definition."
ALDOR_E_ChkBadParams      "Expecting a comma separated list of parameters."
ALDOR_E_ChkBadParamsDups  "Improper duplicate use of parameter name."
ALDOR_E_ChkBadQualification "Improper LHS in $-qualification."
ALDOR_E_ChkBadRecordOrUnion "Duplicate selector/type pair within Record, RawRecord or Union."
ALDOR_E_ChkMissingRetType "Function return type must be specified."
ALDOR_D_ChkUseFromHint    "  Maybe you want to use `import from ...'."
ALDOR_E_ChkSelectSeq	"`select <E> in' must be followed by a sequence."
ALDOR_E_ChkSelectExits    "Unexpected `=>' in select"
ALDOR_E_ChkMustExportJavaToPackage "Java exports must specify a package";

ALDOR_W_FunnyJuxta        "Suspicious juxtaposition.  Check for missing `;'.\n\
Check indentation if you are using `#pile'."
ALDOR_W_FunnyColon        "Suspicious `:'.  Do you mean `local' or `default'?"
ALDOR_W_FunnyEquals       "Suspicious `='.  Do you mean `==' or `:=' ?"
ALDOR_W_FunnyEscape	"Escape character ignored.  Do you mean '__'?"

ALDOR_W_OldSyntaxAlways	"Deprecated syntax: use `finally' instead of `always'"
ALDOR_W_OldSyntaxCatch	"Deprecated syntax: use `catch' instead of `but'"
ALDOR_W_OldSyntaxThrow	"Deprecated syntax: use `throw' instead of `except'"
ALDOR_W_OldSyntaxUnknown	"Deprecated syntax"

ALDOR_W_AbnDomainWithout	"The lefthand side of this domain definition does not have a `with'."

ALDOR_E_ScoAssAndDef      "Cannot both assign and define `%s' in the same scope.\n\
Choose `==', `:=', or use as a `for' variable."
ALDOR_E_ScoAssAndRef	"Cannot both define and reference `%s' in the same scope.\n\
Declare it as a variable instead."
ALDOR_E_ScoAssTypeId      "`%s' is used in a type, so must be constant, and so cannot be assigned to."
ALDOR_E_ScoBadLexConst    "A local constant may not have the same name as an outer variable or parameter."
ALDOR_E_ScoFluidShadow    "A fluid variable cannot shadow an outer non-fluid binding."
ALDOR_E_ScoBadLoopAss     "Cannot explicitly assign a `for' variable."
ALDOR_E_ScoBadParameter   "Improper form appearing in a parameter context."
ALDOR_E_ScoBadTypeFree    "Free variable `%s' is bound elsewhere with a different type."
ALDOR_E_ScoDupDefine      "Constant `%s' cannot be redefined."
ALDOR_E_ScoFreeAndLoc     "Cannot declare `%s' both free and local."
ALDOR_E_ScoFreeConst      "A constant declared free in an inner scope (`%s') cannot be defined in that scope."
ALDOR_E_ScoLateFreeLocal  "It is illegal to declare an identifier free or local once it has already been used, defined or assigned."
ALDOR_E_ScoLibrary        "Cannot assign to or redefine library or archive constant `%s'."
ALDOR_E_ScoNoFree         "A built-in or foreign function cannot be declared `free' or `local'."
ALDOR_E_ScoNoParm         "A built-in or foreign function cannot have the same signature as a parameter."
ALDOR_E_ScoNoSet          "A built-in or foreign function cannot be assigned to or defined."
ALDOR_E_ScoNotBuiltin     "Unknown built-in."
ALDOR_E_ScoParmLocFree    "Parameters cannot be declared local or free."
ALDOR_E_ScoSameSig        "A built-in function cannot have the same signature as a foreign function."
ALDOR_E_ScoParmType       "Parameter type (for %s) must be specified explicitly or with default."
ALDOR_E_ScoVarOverload    "Variables cannot have different types in the same scope."
ALDOR_E_ScoUnknownFree    "Cannot find scope in which free variable `%s' is bound."
ALDOR_W_ScoNotProtocol    "Unknown foreign interface protocol."
ALDOR_W_ScoBadLocal       "Implicit local `%s' is a parameter, local or explicit free in an outer scope. Add a `local' declaration if this is what you intended."
ALDOR_W_ScoFunnyUse       "Identifier `%s' has different declarations in the same scope. Are all implicit and explicit declarations compatible?"
ALDOR_W_ScoBadUse         "Local `%s' is used without being assigned or defined."
ALDOR_W_ScoLocalNoUse     "Local `%s' is not assigned, defined, or used."
ALDOR_W_ScoVarDefault     "`%s' has a default type and a different explicit type declaration."
ALDOR_R_ScoMeaning        "Introducing %s meaning for %s with type %s."
ALDOR_E_ScoEarlyUse       "Implementation restriction: you cannot use a non-lazy constant outside an `add' before it has been defined. Perhaps you ought to define `%s' sooner."

ALDOR_E_StabDupLabels     "Cannot use label `%s' more than once in a given scope."
ALDOR_R_StabImporting     "Importing %s."
ALDOR_R_StabImportingQual "The import was restricted to: %s."
ALDOR_W_StabNotImporting  "Ignoring explicit import from %s."

ALDOR_F_LibOutOfDate      "The file `%s.ao' is newer than `%s.ao'."
ALDOR_F_LibBadVersion     "Library format (obsolete version) in file `%s'.\n\
Current library format version %d.%d.\n\
Found library format version %d.%d."
ALDOR_F_LibExportNotFound "Looking for `%s' with code `%d' in `%s'. Export of `%s' not found."
ALDOR_E_LibBadMagic       "Library format (bad magic number) in file `%s'."
ALDOR_E_LibBadNumSect     "Library format (bad number of sections) in file `%s'."
ALDOR_E_LibBadSectHdr     "Library format (bad section header) in file `%s'."
ALDOR_E_LibBadSectName    "Library format (bad section name) in file `%s'."
ALDOR_E_LibSectDup        "Library format (duplicate section) in file `%s'."
ALDOR_E_LibSectLimit      "Library format (too many sections) in file `%s'."
ALDOR_E_LibSectOffset     "Library format (offset out of range) in file `%s'."
ALDOR_W_LibRedefined      "Redefinition of library symbol `%s'."

ALDOR_E_ArTruncated       "Archive `%s' is truncated or corrupted."
ALDOR_E_ArBadNumber       "Bad number in archive `%s'."

ALDOR_E_TinNoMeaningForId  "No meaning for identifier `%s'."
ALDOR_E_TinNoMeaningForLit "No meaning for %s-style literal `%s'."
ALDOR_E_TinBadDeclare      "Improper form appearing within a declaration."
ALDOR_E_TinIfMeans         "The `if' expression has %d possible types."
ALDOR_E_TinAssMeans        "Assignment has %d meanings."
ALDOR_E_TinDefnMeans       "Definition has %d meanings."
ALDOR_E_TinCantSplitRHS    "This right hand side cannot be split for multiple assignment."
ALDOR_E_TinAssignCreatesDepType "The type of this variable includes a variable, '%s'.\n\
Consider using '==' instead of ':='"
ALDOR_E_TinExprMeans       "Have determined %d possible types for the expression."
ALDOR_E_TinNMeanings       "There are %d meanings for `%s' in this context."
ALDOR_E_TinBadGoto         "A goto must have a label's identifier as its target."
ALDOR_E_TinFarGoto         "Goto must be in the same scope as its label."
ALDOR_E_TinOpMeans         "Operator has %d possible types."
ALDOR_E_TinWildExit        "The `=>' is not inside a sequence."
ALDOR_E_TinWildReturn      "The `return' is not inside a function."
ALDOR_E_TinWildYield       "The `yield' is not inside a `generate'."
ALDOR_E_TinContext         "A value is needed but %s does not produce one."
ALDOR_E_TinContextAssert   "A value is needed but `assert' does not produce one."
ALDOR_E_TinContextDo       "A value is needed but `do' does not produce one."
ALDOR_E_TinContextExit     "A value is needed but `=>' does not produce one."
ALDOR_E_TinContextIf       "A value is needed but `if' expression has no `else'."
ALDOR_E_TinContextRepeat   "A value is needed but `repeat' does not produce one."
ALDOR_E_TinContextSeq      "A value is needed but an empty sequence does not produce one."
ALDOR_E_TinCantInferLhs    "The type of the assignment cannot be inferred."
ALDOR_E_TinNoGoodOp        "There are no suitable meanings for the operator `%s'."
ALDOR_E_TinNoGoodInterp    "There is no suitable interpretation for the expression %s"
ALDOR_E_TinFirstExitType   "The possible type for this %s is %s."
ALDOR_E_TinFirstExitTypes  "The possible types for this %s are:"
ALDOR_E_TinTypeConstIntro  "The interpretation of the type expression"
ALDOR_X_TinTypeConstFailed "failed to satisfy the condition that"
ALDOR_E_TinCantBeAnalyzed  "\
Cannot determine the meaning of this expression because\n\
the type of one of its subexpressions cannot yet be completely analyzed."
ALDOR_E_TinEmbeddedSet	 "Implicit set within a multi-assign is not yet implemented."
ALDOR_E_TinMultiTry	 "try expressions must yield a single value (will be fixed later).
ALDOR_E_TinPackedNotSat	"Raw record type does not satisfy %s"

ALDOR_D_TinNoGoodInterp         "There is no suitable interpretation for the expression %s"
ALDOR_D_TinNoMeaningForId       "No meaning for identifier `%s'."
ALDOR_D_TinSubexprMeans         "Subexpression `%s':"
ALDOR_D_TinPossTypesLhs         "The possible types of the left hand side are:"
ALDOR_D_TinPossTypes            "The possible types were:"
ALDOR_D_TinPossInterps          "The possible interpretations of `%s' are:"
ALDOR_D_TinPossTypesRhs         "The possible types of the right hand side (`%s') are:"
ALDOR_D_TinAlternativeMeanings  "The following could be suitable if imported:"
ALDOR_D_TinOneMeaning           "Meaning %d: %s"
ALDOR_D_TinContextType          "The context requires an expression of type %s."
ALDOR_D_TinMissingExports       "The domain is missing some exports."
ALDOR_D_TinMissingExport        "Missing %s: %s"
ALDOR_D_TinRejectedTypes        "These possible types were rejected:"
ALDOR_X_TinNoArgumentMatch      "rejected because argument %d did not match `%s'."
ALDOR_X_TinParameterMissing     "rejected because parameter %d (%s) is missing."
ALDOR_X_TinBadArgumentNumber    "rejected because it cannot take %d arguments."
ALDOR_X_TinBadFnType            "rejected because the context requires type `%s'."
ALDOR_D_TinRejectedType         "The rejected type is %s."
ALDOR_D_TinRejectedTypesForRhs  "These possible types for the right hand side were rejected:"
ALDOR_D_TinRejectedTypeForRhs   "The rejected type for the right hand side is %s."
ALDOR_D_TinShouldUseDoubleEq    "You should use %s==%s and not %s."
ALDOR_D_TinAvailableTypesForArg "The available types for argument %d were:"
ALDOR_D_TinArgNoMatchParTypes   "Argument %d of `%s' did not match any possible parameter type."
ALDOR_D_TinOperatorNoMatch      "Operator (argument %d of apply) did not match any possible parameter type."
ALDOR_D_TinMoreMeanings         "There are %d meanings for the operator `%s'."
ALDOR_D_TinRetTypesCantContext  "No one possible return type satisfies the context type."
ALDOR_D_TinExpectedType         "Expected type %s."
ALDOR_D_TinExpectedTypes        "Expected one of:"
ALDOR_D_TinRejectedRetTypes     "These possible return types were rejected:"
ALDOR_D_TinOtherDiffArgNum      "There are other meanings rejected due to different number of arguments."
ALDOR_D_TinPossSelectorTypes    "Possible types for the selector `%s' were:"
ALDOR_D_TinPossRetTypeSetBang   "Possible return types for `set!' expression were:"
ALDOR_D_TinPossTypesForSetBang  "No `set!' found for any of the possible types for `%s':"
ALDOR_D_TinSetBangBadArgNum     "There is no `set!' definition with this number of arguments."
ALDOR_D_TinOneImpMeaning        "%s: %s from %s"
ALDOR_D_TinOneLexMeaning        "%s: %s, a local"
ALDOR_D_TinOneLibMeaning        "%s: %s, a library"
ALDOR_D_TinOneMeaning0          "(...): %s"
ALDOR_D_TinFirstExitCant        "This is not compatible with the types of the other %ss."

ALDOR_N_TinOtherExitType        "Here the %s type is %s."
ALDOR_N_TinOtherExitTypes       "Here the %s types are:"

ALDOR_W_TinNoValReturn          "The `return' gives a value but none is expected."
ALDOR_E_TinReturnNoVal          "The `return' gives no value where one is expected."
ALDOR_F_TinRestrictOops		"Use of restrict (@) in the presence of embeddings is not handled yet."

ALDOR_W_TinEarlyImport          "import from %s before its exports could be determined"

ALDOR_R_TinInferring            "Inferring %s: %s."

ALDOR_W_TqNotBuiltin		"`%s: %s' is not exported by Builtin."

ALDOR_E_GenImpNoRep		"Domains with implicit exports must define Rep (as a constant not a macro)"
ALDOR_W_GenDomFunNotConst		"Function returns a domain that might not be constant (which may cause problems if it is used in a dependent type)."
ALDOR_W_GenCatFunNotConst		"Function returns a category that might not be constant (which may cause problems if it is used in a dependent type)."

ALDOR_W_GenBadDefCycle		"Illegal recursive definition: %s"
ALDOR_W_GenBadDefOrder		"Implementation restriction: the value of `%s' depends on the value of %s. Perhaps you ought to define `%s' later?"

ALDOR_M_FintBreakHandler    "Execution terminated: use #quit if you want to quit."
ALDOR_M_FintBreakHandler0   "Use '#quit' to quit Aldor."
ALDOR_M_FintYesOrNo         "\nPlease, answer y or n."
ALDOR_M_FintRedefined       "%s redefined."
ALDOR_M_FintOptionState     "%s is %s."
ALDOR_M_FintOptionValue     "%s is %d."
ALDOR_M_FintUnknownOpt      "Unrecognized option. Type:\n#int %s\nfor help."
ALDOR_M_FintTimings         "                                           Comp: %d msec, Interp: %d msec"
ALDOR_M_FintGbcStart        "Garbage collection..."
ALDOR_M_FintGbcEnd          "done."
ALDOR_M_FintIntOptionsNoFile "Cannot give files, such as `%s', with `#int options'"
ALDOR_M_ShellSyntax         "The correct syntax is: #int %s \"<shell-command>\"\n"
ALDOR_M_CdSyntax            "The correct syntax is: #int %s <directory>\n"
ALDOR_M_InvalidDir	         "Invalid directory.\n"
ALDOR_M_FintOptions         "\nAvailable options:\n\n\
#int %s [on|off]\t print the value of an evaluated expression.\n\
#int %s [on|off]\t try to wrap an assignment around the current line.\n\
#int %s [on|off]\t ask for confirmation before redefining something.\n\
#int %s [on|off]\t display timings after every input.\n\
#int %s [num]\t set the limit size of some messages; 0 for no-limit.\n\
#int %s ...\t reset command line options.\n\
#int %s \t\t\t perform garbage collection.\n\
#int %s \"<command>\"\t execute a shell command.\n\
#int %s <directory>\t change current directory.\n\
#int %s [0|1|2]\t display backtrace when an exception occurs.\n\
     0: never, 1: only when not caught, 2: always.\n\
#int %s \t\t\t display this message.\n\
\n\
#quit   \t\t\t quit the interactive loop."

ALDOR_F_CdFailed          "Could not change working directory to `%s'."
ALDOR_F_CcFailed          "C compile failed.  Command was: %s"
ALDOR_F_LinkFailed        "Linker failed.  Command was: %s"
ALDOR_F_BadFType          "Cannot handle file `%s' of type `%s'.  Try using file type `%s'."
ALDOR_F_WdClobberIn       "Output would clobber input file `%s'."
ALDOR_F_WdClobberFile     "Output would clobber the source file `%s'."
ALDOR_W_WillObsolete      "The file `%s' will now be out of date."
ALDOR_W_RemovingFile      "Removing file `%s'."
ALDOR_W_NotCreatingFile   "Cannot create file `%s' from input file."
ALDOR_W_NoFiles           "No files!  Type `%s -help' for help."
ALDOR_F_NoConfig		"Could not find aldor.conf"
ALDOR_W_CfgError		"%s"


ALDOR_F_NoFNameProperty   "Fortran naming scheme field (%s) is not specified in \
aldor.conf"
ALDOR_F_BadFNameValue     "Unrecognised Fortran naming scheme (%s) specified in \
aldor.conf"
ALDOR_F_NoFCmplxProperty  "Fortran complex functions field (%s) is not specified \
in aldor.conf"
ALDOR_F_BadFCmplxValue    "Unrecognised Fortran complex functions field value (%s) \
specified in aldor.conf"

ALDOR_F_EmitNoFileSupplied "No file name supplied for `%s' argument"

ALDOR_F_ProtoNotSupported    "The language %s does not support the calling %s protocol"

ALDOR_M_BreakEnter          "\
Aldor compiler break ------------------------------------------------------"
ALDOR_M_BreakExit           "\
------------------------------------------------------------------------------"
ALDOR_M_BreakNoMsg         "No message."
ALDOR_M_BreakNoCmd         "Unrecognized command: `%s'."
ALDOR_M_BreakMsgPrompt     "::: "
ALDOR_M_BreakMsgHelpAvail  "Help is available."
ALDOR_M_BreakMsgBadNode    "Bad node."
ALDOR_M_BreakMsgNoNode     "No node."
ALDOR_M_BreakMsgNoStab     "No symbol table."
ALDOR_M_BreakMsgNoTypeInfo "No type info yet."
ALDOR_M_BreakMsgAtTop      "At top."
ALDOR_M_BreakMsgAtLeaf     "At leaf."
ALDOR_M_BreakMsgNoPrev     "No prev."
ALDOR_M_BreakMsgNoNext     "No next."
ALDOR_M_BreakMsgCantSelect "No such selection."
ALDOR_M_BreakMsgNTypes     "The expression has %d possible types."
ALDOR_M_BreakMsg1Type      "The expression has the unique type: "
ALDOR_M_BreakMsgUsedContext "Used in `%s' context."
ALDOR_M_BreakMsgNotId      "Can only ask for meanings of an identifier."
ALDOR_M_BreakHelp          "\
Commands are:\n\
 \thelp \t-- give this help\n\
 \tgetcomsg  \t-- get information on the current message \n\
 \tnotes  \t-- show the notes associated with the current message \n\
 \tmselect i \t-- select message i to be the current message \n\
 \tmnext \t-- select the next message in the list \n\
 \tmprev \t-- select the previous message in the list \n\
 \tmsg  \t-- display the error message again\n\
 \tnice \t-- show with pretty printed form\n\
 \tugly \t-- show with more detailed, internal form\n\
\n\
 \tshow \t-- show the current node\n\
 \tmeans\t-- show the possible meanings of the current node\n\
 \tuse  \t-- show how the current node is used\n\
 \tseman\t-- show the extra semantic information for the current node\n\
 \tscope\t-- show information about the current scope\n\
\n\
 \tup   \t-- use the parent as the current node\n\
 \tdown \t-- use 0th child as the current node\n\
 \tnext \t-- use the next sibling as the current node\n\
 \tprev \t-- use the previous sibling as the current node\n\
 \thome \t-- return to the original node\n\
 \twhere \t-- returns the line and column location of the current node \n\
\n\
 \tquit \t-- exit the compiler, showing all messages so far"

ALDOR_M_GloopBanner	"Aldor\n\
\n\
Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).\n"


ALDOR_H_HelpCmd "\
Usage:            aldor [options] files ...\n\
\n\
 \t               \taldor -help      \tBasic help.\n\
 \t               \taldor -hfiles    \tInput file types.\n\
 \t               \taldor -hoptions  \tSummary of options.\n\
 \t               \taldor -hhelp     \tHelp on help.\n\
 \t               \taldor -hall      \tAll help.\n\
"

ALDOR_H_HelpFileTypes "\
File types accepted:\n\
 \t.as            \tAldor source\n\
\n\
 \t.ai           \tIncluded Aldor source\n\
 \t.ap           \tParsed Aldor source\n\
 \t.fm            \tFoam source\n\
 \t.ao           \tMachine-independent object file (interface, code, ...)\n\
 \t.al           \tArchive of machine-independent object files\n\
 \t.%-4s          \tObject file\n\
 \t.a             \tArchive of object files\n\
\n\
 \t               \tFiles with no type extension are taken as Aldor source.\n\
"

ALDOR_H_HelpOptionSummary "\
Option summary:\n\
 \t-V             \tRun verbosely, giving compilation information.\n\
 \t-D <id>        \tAdd global assertion as `#assert <id>'.\n\
 \t-U <id>        \tRemove global assertion as `#unassert <id>'.\n\
 \t-A <fn>        \tRead command line options from response file <fn>.\n\
 \t-K <n>         \tCompile only the first <n> files (0-9).\n\
 \t--             \tTreat remaining arguments as input files.\n\
 \t-H ...         \tHelp.\n\
 \t-B <dir>       \tUse <dir> as the base directory for aldor system files.\n\
 \t-I <dir>       \tSearch <dir> for additional include files.\n\
 \t-Y <dir>       \tSearch <dir> for additional libraries.\n\
 \t-R <dir>       \tPut the resulting files in directory <dir>.\n\
 \t-L <name>      \tUse the given library.\n\
 \t-F ...         \tIndicate which output files are to be generated.\n\
 \t-E <fn>        \tSpecify the main entry point.\n\
 \t-G ...         \tRun the program.\n\
 \t-O             \tStandard optimizations.\n\
 \t-Q ...         \tSelect code optimizations.\n\
 \t-Z ...         \tDebug and profiling options.\n\
 \t-C ...         \tControl C generation.\n\
 \t-P ...         \tControl C++ generation.\n\
 \t-S ...         \tControl Lisp generation.\n\
 \t-M ...         \tControl compiler messages.\n\
 \t-N ...         \tConfiguration file options.\n\
 \t-W ...         \tDeveloper options.\n\
\n\
 \t               \tOptions are case-insensitive so `-F ap' and `-f AP'\n\
 \t               \tare equivalent.\n\
 \t               \t`...' indicates that more help is available via\n\
 \t               \t`aldor -h <option>'.\n\
"

ALDOR_H_HelpHelpOpt "\
Help options:\n\
 \t-H elp         \tBrief, general help.\n\
 \t-H all         \tHelp about\t... ALL options.\n\
 \t-H files       \t          \t... input file types.\n\
 \t-H options     \t          \t... summary of options.\n\
 \t-H info        \t          \t... product information.\n\
 \t-H {A|args}    \t          \t... argument gathering options.\n\
 \t-H {H|help}    \t          \t... help options.\n\
 \t-H dir         \t          \t... directory and library options.\n\
 \t-H {F|fout}    \t          \t... output file options.\n\
 \t-H {G|go}      \t          \t... execution options.\n\
 \t-H {O|Q|optimize} \t       \t... optimization options.\n\
 \t-H {Z|debug}   \t          \t... debugging options.\n\
 \t-H C           \t          \t... C code generation options.\n\
 \t-H {S|lisp}    \t          \t... Lisp code generation options.\n\
 \t-H {M|message} \t          \t... message options.\n\
 \t-H {W|dev}     \t          \t... developer options.\n\
"

ALDOR_H_HelpArgOpt "\
Argument gathering options:\n\
 \t-A <fn>        \tRead command line options from response file <fn>.\n\
\n\
 \t-K <n>         \tCompile only the first <n> files (0-9).  The remaining\n\
 \t               \targuments are given to the Aldor program, if run with `-G'.\n\
 \t--             \tTreat remaining arguments as input files, even if they begin\n\
 \t               \twith `-'.\n\
 \t               \tIf the environment variable `ALDORARGS' is defined, its\n\
 \t               \tcontents are handled as options before the command line.\n\
"

ALDOR_H_HelpDirOpt "\
Directory and library options:\n\
 \t-I <dir>       \tSearch <dir> for additional include files.\n\
 \t               \tThe environment variable `INCPATH' is tried for defaults.\n\
 \t-Y <dir>       \tSearch <dir> for additional libraries.\n\
 \t               \tThe environment variable `LIBPATH' is tried for defaults.\n\
 \t-R <dir>       \tPut the resulting files in directory <dir>.\n\
 \t               \tThe default is the current directory.\n\
 \t-B <dir>       \tUse <dir> as the base directory for aldor system files.\n\
 \t               \tWithout `-B', the environment variable `ALDORROOT' is tried.\n\
\n\
 \t-L <fn>        \tUse the library given by file name <fn>.\n\
 \t               \tAn alphanumeric <fn> is a short form for `lib<fn>.a'.\n\
 \t-L <id>=<fn>   \tSame as `-L <fn>' but the source name <id> is \n\
 \t               \tassociated with the library.\n\
"

ALDOR_H_HelpFileOpt "\
File options:\n\
 \t-F <ft>[=<fn>] \tIndicate which output files are to be generated.\n\
 \t               \tThe parameter <ft> is one of:\n\
 \t               \t \tai     \t(*) Source after inclusion     \t(from .as)\n\
 \t               \t \tap     \t(*) Macro-expanded parse tree  \t(from .as)\n\
 \t               \t \tasy    \t(*) Symbol information\n\
 \t               \t \tao     \t(*) Machine-independent object\n\
 \t               \t \tfm     \t(*) Foam code\n\
 \t               \t \tlsp    \t(*) Lisp code\n\
 \t               \t \tc      \t(*) C code\n\
 \t               \t \tc++    \t(2) C++ code and Aldor stubs\n\
 \t               \t \to      \t(*) Object file\n\
 \t               \t \tx      \t(1) Executable file\n\
 \t               \t \tmain   \t(1) Generate `aldormain.c' with function `main'.\n\
\n\
 \t               \tIf no `-F' or `-G' option is given, then `-Fao' is assumed.\n\
\n\
 \t               \tCases marked `(*)', above, cause one file of the given\n\
 \t               \ttype to be generated for each input file.\n\
 \t               \tCases marked `(1)' cause only one file to be generated for\n\
 \t               \tthe entire compilation.\n\
 \t               \tCases marked `(2)' cause two files of the given type\n\
 \t               \tto be generated for each input file.\n\
\n\
 \t               \tIf the parameter <fn> is given, it is used as the name of\n\
 \t               \tthe corresponding output file.\n\
\n\
 \t               \tThe compiler will not overwrite a C or Lisp file\n\
 \t               \tit did not generate.\n\
"

ALDOR_H_HelpGoOpt "\
Execution options:\n\
 \t-G run         \tRun the program, as compiled to machine code.\n\
 \t               \tThe executable file is removed when finished unless\n\
 \t               \tthe `-Fx' option is present.\n\
 \t-G interp      \tInterpret the program, as translated to intermediate Foam code.\n\
 \t-G loop        \tRun interactively, interpreting each expression as it is\n\
 \t               \tgiven.  With this option, the file `aldorinit.as' is used for\n\
 \t               \tinitialization.\n\
\n\
 \t-E <fn>        \tUse the input file `<fn>.*' as the main entry point.\n\
 \t               \tThe default is the first file.\n\
 \t               \t`-e' is useful only with `-Fx', `-Grun' or `-Ginterp'.\n\
"

ALDOR_H_HelpOptimOpt "\
Optimization options:\n\
 \t-O             \tOptimize.  This is equivalent to `-Q2'.\n\
\n\
 \t-Q <n>         \tSelect a level of code optimization. (default `-Q1')\n\
\n\
 \t-Q <opt>       \tTurn on optimization <opt>.\n\
 \t-Q no-<opt>    \tTurn off optimization <opt>.\n\
 \t               \t                                        \tQ0  Q1  Q2  Q3  Q4\n\
 \t-Q all         \tAll supported optimizations.            \t             X   X\n\
 \t-Q inline      \tAllow open coding of functions.         \t         X   X   X\n\
 \t-Q inline-all  \tAllow open coding of any functions.     \t             X   X\n\
 \t-Q inline-limit=<n>\tSet maximum inline code factor to <n>.\t 1   1   5   6   8\n\
 \t-Q inline-size=<n>\tSet maximum inline code size to <n>. \t 1   1  10  20  40\n\
 \t-Q cfold       \tEvaluate constants at compile time.     \t     X   X   X   X\n\
 \t-Q ffold       \tEvaluate floating point constants.      \t         X   X   X\n\
 \t-Q hfold       \tEvaluate type code computations.        \t     X   X   X   X\n\
 \t-Q deadvar     \tEliminate unused variables and values.  \t     X   X   X   X\n\
 \t-Q dassign     \tEliminate assignments to dead variables.\t         X   X   X\n\
 \t-Q peep        \tLocal ``peep-hole'' optimizations.      \t     X   X   X   X\n\
 \t-Q cprop       \tCopy propagation.                       \t         X   X   X\n\
 \t-Q cse         \tCommon sub-expression elimination.      \t         X   X   X\n\
 \t-Q cast        \tSimplify variable types.                \t         X   X   X\n\
 \t-Q env         \tVarious environment optimisations.      \t         X   X   X\n\
 \t-Q emerge      \tEliminate lexical levels and records.   \t         X   X   X\n\
 \t-Q emerge-rr   \tEliminate raw records.                  \t         X   X   X\n\
 \t-Q flow        \tSimplify computed tests and jumps.      \t         X   X   X\n\
 \t-Q cast        \tReduce the number of casts.             \t         X   X   X\n\
 \t-Q cc          \tUse the C compiler's optimizer.         \t         X   X   X\n\
 \t-Q del-assert  \tDo not check 'assert' statements.       \t         X   X   X\n\
 \t-Q cc-fnonstd  \tPossibly faster but not IEEE compliant  \t                 X\n\
\n\
 \t               \tCombinations can be used, e.g. `-Q3 -Qno-ffold'.\n\
 \t               \tThe option `-Qcc' may exclude `-Z' on some platforms.\n\
"

ALDOR_H_HelpDebugOpt "\
Debug options:\n\
 \t-Z db          \tGenerate debugging information in object files.\n\
 \t-Z prof        \tGenerate profiling code in object files.\n\
"

ALDOR_H_HelpConfigOpt "\
Configuration options:\n\
 \t-N file=<file>      \tSpecify name of config file.\n\
 \t-N sys=<name>       \tSpecify system name.\n\
"

ALDOR_H_HelpCOpt "\
C generation options:  Control the behaviour of `-Fc' `-Fo' and `-Fx'.\n\
 \t-C standard    \t`-Fc' generates ANSI/ISO standard C.\n\
 \t-C old         \t`-Fc' generates old C (the default).\n\
 \t               \tThe options `-Cstandard' and `-Cold' are mutually exclusive.\n\
\n\
 \t-C comp=<name> \tUse <name> instead of the default C compiler.\n\
 \t-C link=<name> \tUse <name> instead of the default linker.\n\
 \t-C cc=<name>   \tUse <name> instead of the default C compiler and linker.\n\
 \t               \tWithout `-Ccc=', the environment variable `CC' is tried.\n\
 \t-C go=<name>   \tUse <name> to run the output of the linker.\n\
 \t               \tWithout `-Cgo=', the environment variable `CGO' is tried.\n\
\n\
 \t-C args=<opts> \tPass <opts> as options to the C compiler.\n\
 \t-C smax=<n>    \tTry to put no more than <n> statements per file, \n\
 \t               \tif necessary splitting the generated file into:\n\
 \t               \t<name>.h, <name>.c, <name>001.c, <name>002.c, etc.\n\
 \t               \tUsing -C smax=0 turns off file splitting.\n\
 \t               \t(default: smax=0)\n\
 \t-C idlen=<n>   \tSet the maximum length of C identifiers to be <n>. \n\
 \t               \tUsing -C idlen=0 turns off identifier truncation.\n\
 \t               \t(default: idlen=30)\n\
 \t-C {no-}idhash \tUse hash codes in global C identifiers.\n\
 \t               \t(default: idhash)\n\
 \t-C {no-}lines  \tPreserve Aldor source line numbers in generated C.\n\
 \t               \t(default: no-lines).\n\
\n\
 \t-C sys=<name>  \tPass '-W sys=<name> to the C compiler and linker.\n\
 \t               \tThe unicl driver understands this option.\n\
 \t-C runtime=<name1>,<name2>  \n\
 \t               \tLink in lib<name1>.a lib<name2>.a as the runtime library (default: foam).\n\
 \t-C fortran     \tPass '-Wfortran' to link driver which adds fortran options\n\
 \t               \tsuch as fortran runtime libraries.\n\
 \t-C lib=<name>  \tLink with the library lib<name>.a\n\
\n\
"

ALDOR_H_HelpCppOpt "\
C++ generation options:  Control the behaviour of `-Fc++'.\n\
 \t-P basicfile=<bf>      \tif the filename <bf> (absolute filename) is provided,\n\
 \t                       \tthe standard basic types correspondence between C++ and Aldor\n\
 \t                       \twill be overridden.\n\
 \t                       \tIf this option is not provided, the compiler uses 'basic.typ'\n\
 \t                       \tlocated in $ALDORROOT/include.\n\
\n\
 \t-P discrim-return      \tWill discriminate functions on the return type by changing\n\
 \t                       \tthe name of the function to 'fnname_return-type'.\n\
\n\
 \t-P no-discrim-return   \tWon't discriminate functions on the return type.\n\
 \t                       \tNames of the functions will be as the original, however\n\
 \t                       \tthe code generated for overloaded functions on the return\n\
 \t                       \ttype only won't compile.\n\
 \t                       \tThis option is the default.\n\
"

ALDOR_H_HelpLispOpt "\
Lisp generation options:  Control the behaviour of `-Flsp'.\n\
 \t-S common      \t`-Flsp' produces Common Lisp code   (the default).\n\
 \t               \tThe options `-Scommon', `-Sstandard' and `-Sscheme'\n\
 \t               \tare mutually exclusive.\n\
\n\
 \t-S ftype=<ft>  \tUse <ft> as the file type for the generated lisp file\n\
 \t               \t(default: `-Sftype=lsp').\n\
"

ALDOR_H_HelpMsgOpt "\
Message options:  Use `-M no-<word>' to negate `-M <word>'.\n\
 \t-M <n>         \tControl how much detail is given.      \t(default: `-M2')\n\
\n\
 \t-M no-<opt>    \tTurn off `-M <opt>'.\n\
 \t               \t                                       \tM0  M1  M2  M3\n\
 \t-M warnings    \tDisplay warnings.                      \t N   Y   Y   Y\n\
 \t-M number      \tDisplay the order of discovery number. \t N   Y   Y   Y\n\
 \t-M source      \tDisplay the program lines for messages.\t N   N   Y   Y\n\
 \t-M details     \tDisplay details.                       \t N   N   Y   Y\n\
 \t-M notes       \tDisplay cross reference notess.        \t N   N   Y   Y\n\
 \t-M remarks     \tDisplay remarks.                       \t N   N   N   Y\n\
\n\
 \t-M sort        \tSort messages by source position.      \t Y   Y   Y   Y\n\
 \t-M mactext     \tPoint to macro text, rather than use.  \t Y   Y   Y   Y\n\
 \t-M abbrev      \tAbbreviate types in messages.          \t Y   Y   Y   Y\n\
 \t-M human       \tHuman-oriented format.                 \t Y   Y   Y   Y\n\
 \t-M release     \tDisplay release version with -gloop.   \t Y   Y   Y   Y\n\
 \t-M name        \tShow the name of each message.         \t N   N   N   N\n\
 \t-M antiques    \tDisplay warnings for old-style code.   \t N   N   N   N\n\
 \t-M preview     \tDisplay messages as they occur.        \t N   N   N   N\n\
 \t-M inspect     \tUse interactive inspector for errors.  \t N   N   N   N\n\
\n\
 \t-M emax=<n>    \tStop after <n> error messages.         \t(default: 10)\n\
 \t-M db=<fn>     \tUse <fn> as the message database.      \t(default: no-db)\n\
\n\
 \t-M <msgname>   \tTurn on warning or remark named <msgname>\n\
 \t-M no-<msgname>\tTurn off warning or remark named <msgname>\n\
 \t               \tUse `-Mname' to find the names of messages.\n\
\n\
 \t               \tThese can be combined, e.g. `-M2 -Mno-source -Mpreview'\n\
 \t-M base=<dir>  \tStrip <dir> from pathnames in error messages\n\
 \t-M no-base     \tDisable -M base=<dir>\n\
"

ALDOR_H_HelpDevOpt "\
Developer options (subject to change):\n\
\n\
 \t-W debug       \tTurn on experimental debugging code generation.\n\
 \t-W depend      \tPrint compile-time dependencies for this file.\n\
 \t-W small-hcodes\tTurn on experimental short hashcode generation.\n\
 \t-W emerge-noalias\tWork around for an optimizer bug.\n\
 \t-W check       \tTurn on internal safety checks.\n\
 \t-W nhash       \tAssume all exported types have constant hashcodes.\n\
 \t-W trace-cfuns \tRuntime announcements of entry to generated C functions.\n\
 \t-W no-keyword=<name>\n\
 \t               \tThe symbol <name> will not be treated as a keyword. This\n\
 \t               \tmay only be used to compile old files containing symbols\n\
 \t               \tthat were later made into reserved words (e.g. ref)\n\
 \t-W name=<name> \tCompile file as if it was called <name>.as.\n\
 \t-W prefix=<prefix>\tCompile file as if it had <prefix> at the start of its name.\n\
 \t-W runtime     \tProduce code suitable for the runtime system.\n\
 \t-W loops       \tAlways inline generators when possible.\n\
 \t-W missing-ok  \tDo not stop compilation if some exports are missing.\n\
 \t-W audit       \tSet maximum foam auditing level.\n\
 \t-W dumb-import \tDo not trace imported domains for their base value.\n\
 \t-W rtcache=N   \tSet the runtime cache size for domain constructors to N.\n\
 \t               \t(the default is N=15; use N=0 for the libfoam default)\n\
 \t-W trap        \tTrap failure exits (for debugging Aldor).\n\
\n\
 \t-W gc          \tGarbage collect as needed (if gc is available; default).\n\
 \t-W gcfile      \tGarbage collect after each file (if gc is available).\n\
\n\
 \t-W no-where    \tDon't rewrite `where' expressions.\n\
 \t-W no-negated-tests\n\
 \t               \tDon't optimise `if (!x) T else E' to `if (x) E else T'.\n\
 \t-W sexpr       \tRun a read-print loop.\n\
 \t-W seval       \tRun a read-eval-print loop.\n\
\n\
 \t-W test=<name> \tRun compiler self-test <name>.\n\
 \t               \tUse `-W test=show' to see a list of self-tests.\n\
 \t-W D+<name>    \tTurn on debug hook <name>.\n\
 \t               \tUse `-W D+show' to see a list of debug hooks.\n\
 \t-W D-<name>    \tTurn off debug hook <name>.\n\
 \t               \tUse `-W D+show' to see a list of debug hooks.\n\
 \t-W T[apdrgst0]+<ph>\n\
 \t               \tPhase tracing:\n\
 \t               \t \t-WTa+<ph>      \tAnnounce entry to <ph>\n\
 \t               \t \t-WTp+<ph>      \tPretty print result of <ph>\n\
 \t               \t \t-WTd+<ph>      \tPrint debug information for <ph>\n\
 \t               \t \t-WTr+<ph>      \tShow result of <ph>\n\
 \t               \t \t-WTg+<ph>      \tGarbage collect after <ph>\n\
 \t               \t \t-WTs+<ph>      \tStorage audit after <ph>\n\
 \t               \t \t-WTt+<ph>      \tTerminate after <ph>\n\
 \t               \t \t-WT0+<ph>      \tIgnore earlier `-WT' option for <ph>\n\
 \t               \tSeveral phases can be given as `ph1+ph2+ph3' or `all'.\n\
 \t               \tSeveral options can be given at once, e.g. `-WTags+all'.\n\
 \t               \tCompile a file with `-v' to see the phase abbreviations.\n\
"

ALDOR_H_HelpMenuPointer "\
 \tUse `aldor -h help' for a menu of more detailed help.\n\
"

ALDOR_H_HelpProductInfo "Contact infodesk@nag.co.uk for product support\
 and information.\n\
Use the aldorbug program for reporting any bugs.\n\
"

ALDOR_E_SigAbrt           "Program fault (abort process)."
ALDOR_E_SigBus            "Program fault (bus error)."
ALDOR_E_SigEmt            "Program fault (emulator instruction)."
ALDOR_E_SigFpe            "Program fault (arithmetic exception)."
ALDOR_E_SigHup            "User break (hangup)."
ALDOR_E_SigIll            "Program fault (illegal instruction)."
ALDOR_E_SigInt            "User break (interrupt)."
ALDOR_E_SigPipe           "Program fault (write on a pipe with no one to read it)."
ALDOR_E_SigDanger         "Program fault (paging space low)"
ALDOR_E_SigQuit           "User break (quit)."
ALDOR_E_SigSegv           "Program fault (segmentation violation)."
ALDOR_E_SigSys            "Program fault (bad argument to system call)."
ALDOR_E_SigTerm           "User break (software termination signal)."
ALDOR_E_SigTrap           "Program fault (trace trap)."
ALDOR_E_SigXcpu           "Exceeded time limit imposed by operating system."
ALDOR_E_SigXfsz           "Exceeded file size limit imposed by operating system."
ALDOR_E_SigUnknown        "Unexpected signal (%d)."

ALDOR_F_SxAlreadyShare    "Share label #nn= previously defined."
ALDOR_F_SxBadArgumentTo   "Inappropriate argument to function `%s'."
ALDOR_F_SxBadChar         "Illegal character 0x%x."
ALDOR_F_SxBadCharName     "Improper character name after #\\."
ALDOR_F_SxBadComplexNum   "Improper complex number #C...."
ALDOR_F_SxBadFeatureForm  "Improper feature form following #+ or #-."
ALDOR_F_SxBadPotNum       "Meaningless potential number `%s'."
ALDOR_F_SxBadPunct        "Misplaced `%s'."
ALDOR_F_SxBadToken        "Missing escape in token."
ALDOR_F_SxBadUninterned   "Package given with `#:'"
ALDOR_F_SxCantMacroArg    "Macro #%c does not take a numeric argument."
ALDOR_F_SxCantShare       "Share label #nn= not previously defined."
ALDOR_F_SxInternNeeds     "Intern requires a string."
ALDOR_F_SxMacroIlleg      "Illegal macro character `#%c'."
ALDOR_F_SxMacroUndef      "Undefined macro character `#%c'."
ALDOR_F_SxMacroUnimp      "Unimplemented macro character `#%c'."
ALDOR_F_SxMustMacroArg    "Macro #n%c requires a numeric argument."
ALDOR_F_SxNReverseNeeds   "NReverse requires the last cdr of a list to be nil."
ALDOR_F_SxNumDenNeeds     "%s requires an integer or ratio."
ALDOR_F_SxPackageExists   "A package with the name %s already exists."
ALDOR_F_SxReadEOF         "End of file during read."
ALDOR_F_SxTooManyElts     "Number of elements greater than given size."

ALDOR_F_StoCantBuild      "Storage allocation error (can't build internal structure)."
ALDOR_F_StoOutOfMemory    "Storage allocation error (out of memory)."
ALDOR_F_StoUsedNonalloc   "Storage allocation error (using non-allocated space)."
ALDOR_F_StoFreeBad	"Storage allocation error (atempt to free unknown space)."

ALDOR_F_CantOpen          "Could not open file `%s'."
ALDOR_F_CantOpenMode      "Could not open file `%s' with mode `%s'."
ALDOR_F_CantFindTemp      "Could not find unused temporary file names."
ALDOR_W_CantUseObject     "Could not use object file `%s'."
ALDOR_W_CantUseLibrary    "Could not use library file `%s'."
ALDOR_W_CantUseArchive    "Could not use archive file `%s'."
ALDOR_W_OverRideLibraryFile "Current file over-rides existing library in\n `%s'."

ALDOR_F_Bug		"Compiler bug: %s."
ALDOR_W_Bug		"Internal compiler warning: %s."
ALDOR_F_BugNoBoolean	"Missing vital semantic information: please add `import from Boolean' to this scope!"

ALDOR_F_BugExportSymeNotInit "Compiler bug: I am trying to create a slot for the export `%s:%s'. However, gen0SymeSetInit() has not been used to initialise it so I can not continue (sorry)."

ALDOR_F_BadJavaFileName "Cannot generate java for %s; name reserved by java"

ALDOR_I_PreRelease        "This is a pre-release of %s. `aldor -h info' for \
more details."
ALDOR_I_DemoExpiry        "This is a demo version of %s. `aldor -h info' for \
information.\n\
This program should not be used after %s"

ALDOR_S_Syme_Label   	"label"
ALDOR_S_Syme_Param   	"parameter"    
ALDOR_S_Syme_LexVar  	"lexical variable"
ALDOR_S_Syme_LexConst	"lexical constant"
ALDOR_S_Syme_Import  	"import"
ALDOR_S_Syme_Export  	"export"
ALDOR_S_Syme_Extend  	"extend"
ALDOR_S_Syme_Library 	"library"
ALDOR_S_Syme_Archive 	"archive"
ALDOR_S_Syme_Builtin 	"builtin"
ALDOR_S_Syme_Foreign 	"foreign"
ALDOR_S_Syme_Fluid    	"fluid variable"
ALDOR_S_Syme_Trigger  	"trigger"
ALDOR_S_Syme_Temp  	"temporary"
