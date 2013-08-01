#ifndef _EMITCPP_H_
#define _EMITCPP_H_


/* ---------- ENUMERALS ---------- */

typedef enum { AC_NONE, AC_FUNCTION, AC_CLASS, AC_TEMPL_CLASS, 
		 AC_ABS_CLASS, AC_ABS_TMPL_CLASS, AC_VAR, AC_CST } cpp_types;

typedef enum { CT_FUNC, CT_VAR, CT_STR, CT_PARENT, CT_TMPLPARMS, CT_EXTRA } CellType;

typedef enum { NONE, T4AT_BT, T4AT_UT } Type4AldorType;

/* --------- MACROS --------------- */

#define alloc(x) (x *) stoAlloc(OB_Other,sizeof(x))
/* #define STRCAT strcat */
#define STRSIZE 500
#define InitStr() strAlloc(STRSIZE)
#define tfIsMultiReturn(tf) (tfIsMulti(tf) && tfMultiArgc(tf))

#define BANG  '!'
#define QMARK '?'

/* ---------- GLOBAL VARIABLES ------------ */

extern String CurrentClassAldor;
extern String CurrentClassCpp;

extern String CurrentClass;
extern String CurrentClassInUse;
extern const int theTRUE;
extern const int theFALSE;
extern String *basicAldorTypes;
extern int bAT_size;
extern String *basicCppTypes;
extern String commonOperators[];
extern int CO_size;
extern String infixOps[];
extern int IO_size;
extern String typesToSkip[];
extern int tTS_size;
extern String cppKeywords[];
extern int CK_size;
#if 0
extern struct ccSpecCharId_info *ccSpecCharIdTable;
#endif
extern int globalFuncCounter;
extern const int TabZero;


/* ---------- TO ACCESS VALUES OF ABSYN (extension of what is in absyn.h) --------- */

#define abIdStr(a)       ((a)->abId.sym->str)
#define abDefineLhs(a)   ((a)->abDefine.lhs)
#define abDefineRhs(a)   ((a)->abDefine.rhs)
#define abDeclareId(a)   ((a)->abDeclare.id)
#define abDeclareType(a) ((a)->abDeclare.type)
#define abSeqArg(a,i)    ((a)->abSequence.argv[(i)])
#define abCommaArg(a,i)  ((a)->abComma.argv[(i)])
#define abWithin(a)      ((a)->abWith.within)
#define abWithBase(a)    ((a)->abWith.base)
#define abAddBase(a)     ((a)->abAdd.base)

/* ---------- TRUE/FALSE values ---------- */

#define BL_Exported       theTRUE
#define BL_UsePercent     theTRUE
#define BL_WithType       theTRUE
#define BL_WithName       theTRUE
#define BL_InBody         theTRUE
#define BL_Global         theTRUE
#define BL_IsStatic       theTRUE
#define BL_Abstract       theTRUE
#define BL_Export         theTRUE
#define BL_IsTmpl         theTRUE
#define BL_UseTmplParms   theTRUE
#define BL_ForDeclInBody  theTRUE

/* ---------- DATA STRUCTURES ---------- */

typedef enum { OPERATOR, SPECIAL_SYM, REGULAR } OpState;

/* ------------- */

typedef struct cell {
  struct cell *next;
  CellType    ct;
  void        *Item;
} Cell;

typedef struct {
  Cell *head;
  Cell *current;
  Cell *tail;
} MyList;

/* ------------- */

typedef struct {
  /* example for template <class T, class U> class A {...}; */
  String basic;       /* A */
  String export;      /* ATU */
  String typeCpp;     /* A<PercentType,T,U> */
  String typeAldor;   /* A(T,U) */
} Ident;

typedef struct _class_ {
  Ident     id;
  int       methCounter;
  int       classCounter;
  MyList      *extraClasses;
  TForm     params;
  TForm     methods; 
  MyList      *parents;
} Class;

typedef struct {
  String    id;
  TForm     mapType;      /* from there we can get everything: params and return type */
  Class     *ParentClass; /* needed to translate '%' */
  MyList      *params;
  AbSyn     ret_typ;
  int       isOperator;
  OpState   OpStatus;
  int       isStatic;
  int       multipleReturns;
} Function;

typedef struct {
  Ident     id;
  AbSyn     type;
} Parent;

/* ---------- TO ACCESS VALUES OF THE DATA STRUCTS ----------- */

#define atBt(a)    ((a)->basetype)    /* Basetype from an Aldor Type */
#define funcRet(f) ((f)->ret_typ)       /* Return type of a function   */

/*
 * ---------------------------------------------------------
 *              FUNCTIONS OF THE INTEROP
 * ---------------------------------------------------------
 */

/* 
 * MyList functions
 * ================
 * 
 * Implements a List ADT.
 * Will be replaced by the "standard" list of the aldor compiler code
 */

MyList *InitMyList();
void Append(MyList *, CellType, void *);
void Rewind(MyList *);
void *GetItem(MyList *);
void GotoNext(MyList *);
void HeadToNext(MyList *);
int  Empty(MyList *);
int  NotEmpty(MyList *);
int  EOList(MyList *);

Function *InitFunc();
Class *InitClass();

String GettypId(AbSyn);
cpp_types GettypApply(AbSyn);
cpp_types GettypType(AbSyn);
cpp_types GettypDeclare(AbSyn);
cpp_types GettypDefine(AbSyn);

void FreeList(MyList *);
void FreeFunction(Function *);
void FreeClass(Class *);

String MappingTypes(String);
void genCpp(AbSyn, String, String);
String STRCAT(String, String);


int cppOption(String);
void updateBasicTypes(String, int);
int InitBasicTypesArray(String);
void CreateOutputFiles(String, String, FILE **, FILE **);
int SequenceCheck(AbSyn);
void ExportGen(AbSyn, FILE *, FILE *);
void CodeGen(AbSyn, FILE *, FILE *);
String outputCppFnPtrParms(TForm, Class *, int, int, int, int, String);
String GenCppFunc_ParamsRetFnPtr(TForm, Class *, int, int, int);
String GenCppFunc_RetFnPtr(TForm);

/* ---------- Header files ---------- */

/* ----- Building  */

/* ---------- ERROR ---------- */
Class *errorBuildClass(Class *);

/* ---------- ID HANDLING ---------- */
void BuildParentIdAbs(Ident *, AbSyn);
void BuildExtraClassId(Class *, Class *, int);
void BuildClassId(Class *, int);
String BldId(AbSyn);
void BuildParentId(Ident *, AbSyn);

/* ---------- ABSTRACT CLASS HANDLING ---------- */
void getParentsAbs(Class *, TForm, int);
Class *BuildAbstractClass(AbSyn, int);

/* ---------- EXTRA CLASS HANDLING ---------- */
Class *buildExtraClass(Class *, TForm, int);

/* ---------- CLASS HANDLING ---------- */
void getParents(Class *, TForm, int);
Class *BuildClass(AbSyn, int);

/* ----- Determination of type */

String GettypId(AbSyn);
cpp_types GettypApply(AbSyn);
cpp_types GettypType(AbSyn);
cpp_types GettypDeclare(AbSyn);
cpp_types GettypDefine(AbSyn);
TForm tfFollowDefDecl(TForm);


/* ----- Main functions for code generation */

/* ---------- FUNCTION SIGNATURES ---------- */

String GenAldorFnSig(TForm, String, Class *, int, int, int, int);
String GenCppFnSig(TForm, String, Class *, int, int);

/* ---------- END FUNCTION SIGNATURES ---------- */

/* ---------- STUBS ---------- */

String GenAldorStubs(TForm, String, Class *, int, int);

/* ---------- END STUBS ---------- */

/* ---------- GLOBAL FUNCTIONS ---------- */

String GenCppFunc(TForm, String, Class *, int);
void GenFunction(AbSyn, FILE *, FILE *);

/* ---------- END GLOBAL FUNCTIONS ---------- */

/* ---------- HEADER ---------- */

void GenCppConstructors(Class *, int, FILE *);
void GenCppDestructor(String, int, FILE *);
void GenCppAccessToRealObj(Class *, int, FILE *);
void GenCppGiveType(Class *, int, FILE *);
void GenHeaderForClass(Class *, FILE *);

/* ---------- END HEADER ---------- */

/* ---------- CLASS ---------- */

String GCFC_HeaderTmpl(Class *);
String GCFC_HeaderInher(Class *);
String GCFC_Methods(Class *, Syme);
void GenExtraClass(Class *, Class *, FILE *);
void GenClassesForDomains(AbSyn, int, FILE *, FILE *);
void GenClassForCategories(AbSyn, int, FILE *, FILE *);

/* ---------- END CLASS ---------- */

/* ---------- EXPORT / IMPORT ---------- */

void GenExportClass(AbSyn, int, FILE *, FILE *);
void GenExportFunction(AbSyn, FILE *, FILE *);

/* ---------- END EXPORT / IMPORT ---------- */

/* ----- Generation of methods */

/* ---------- METHODS ---------- */

String GenCppBodyForMethods(TForm, String, Class *);
String GenCppBodyStaticPercentForMethods(Syme, Class *, int, int);
void GenMethodsForClass(Class *, FILE *);

/* ---------- END METHODS ---------- */


/* ----- Special work for multiple return functions */

/* ---------- MULTIPLE RETURN ---------- */

String GenCppMultiRetLocalDecl(TForm, int);
String GenCppMultiRetAssign(TForm);
String GenCppMultiRet(TForm, int, int, int, int);

/* ---------- END MULTIPLE RETURN ---------- */


/* ----- Generation of parameters for methods */

/* ---------- PARAMS HANDLING ---------- */

/* -- Common Code for Aldor -- */
String outputAldorTmplParms(TForm, int);
String outputAldorRegParms(TForm, int, int, int, String, String);

/* -- Aldor Params -- */
String GenAldorParams(TForm, Class *, int, int, int, int, int);
String GenAldorOneParam(TForm, int, int, int, int);

/* -- Common Code for C++ -- */

/* -- C++ Params -- */
String GenCppParams(TForm, Class *, int, int, int, int, int);
String GenCppParamsBody_VirtualForStatic(TForm, Class *, int);
String GenCppParamsStaticPercent(TForm, Class *, int, int, int, int);
String GenCppParams_Body(TForm, String, Class *, int);
String GenCppParams_Global_Body(TForm, String);

/* ---------- END PARAMS ---------- */

/* ----- Type handling in code generation */

/* ---------- TYPES ---------- */

/* -- Basic -- */
String MappingTypes(String);
String MachineToCpp(AbSyn);

/* -- AbSyn -- */
String ApplyToAldor(AbSyn, int);
String CommaToAldor(AbSyn, int);
String QualifyToString(AbSyn, int);
String AbSynToAldor(AbSyn, int);
String AbSynToCpp(AbSyn);
int AbSynIsUserDef(AbSyn);
int AbSynIsVoid(AbSyn);

/* -- TForm -- */
String TFormToAldor(TForm, int);
TForm tfMakeSubst(TForm);
TForm tfFollowDefDecl(TForm);

/* -- Import/Export -- */
String GiveTypeAldorExport(Class *, int);
String GiveTypeCppExtern(Class *, int);
void GenGiveTypeForExport(Class *, FILE *, FILE *);

/* ---------- END TYPES ---------- */


/* ----- Utilities for code generation */

/* ---------- UTILS ---------- */

/* -- String -- */
void printTab(FILE *, int);
String sprintTab(String, int);
/* LDR */
#if 0
#pragma warning (disable:4030)
String itoa(int);
#pragma warning (default:4030)
#endif
String OutputDomNameAldor(String);
String addUnderScore(String);
String transOperatorLike(String);
String AldorifySpeSym(String);
String RealTypeString(AbSyn, int);

/* -- Test -- */
int PercentsInParms(TForm, Class *);
int StaticOrNot(TForm, Class *);
int SpecialCppFunc(String);

/* -- ID Handling -- */
String operatorForC(String);
String createFnIdExternC(String, Class *);
String createFnIdExportC(String, Class *);
String createFnIdForCPP(String, Class *);
String GenAldorFuncName(String, Class *, int);
String GetIdFromAbSyn(AbSyn);
int abIsFuncPtr(AbSyn);
int tfIsFuncPtr(TForm);
int IsCommonOperator(String);
int IsInfixOperator(String);
int IsSpecialSymbol(char,int);
String GetStringForSpecialSymbol(char,int);
int ToSkipClass(Class*);


/* ---------- END UTILS ---------- */

/* ----- Init of structures used for C++ gen */

Function *InitFunc();
Class *InitClass();

/* ----- Type List */

MyList *InitList();
void Append(MyList *, CellType, void *);
void Rewind(MyList *);
void *GetItem(MyList *);
void GotoNext(MyList *);
void HeadToNext(MyList *);
int Empty(MyList *);
int NotEmpty(MyList *);
int EOList(MyList *);

/* ----- Memory management */

void FreeList(MyList *);
void FreeFunction(Function *);
void FreeClass(Class *);


#endif
