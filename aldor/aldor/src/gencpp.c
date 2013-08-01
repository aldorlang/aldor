#include "axltop.h"
#include "gencpp.h"
#include "stab.h"
#include "store.h"
/*
 *  2006/Dec/20
 *
 *  Including cconfig.h to gain access to CC_SF_is_double
 */
#include "cconfig.h"
/*
 *  Define a local macro for the C data type being used to represent
 *  a single float.  We cannot just go and include the normal definition
 *  because that becomes a tangle of conflicting definitions from all the
 *  other files that are included as a result.
 */
#ifdef CC_SF_is_double
#  define SF_STRING "double"
#else
#  define SF_STRING "float"
#endif

local String itoa_(int);

/* ---------- Extra structure ----------- */
/* Also defined in genc.c */
struct ccSpecCharId_info {
  int		ch;
  String          str;
}; 

extern struct ccSpecCharId_info ccSpecCharIdTable[];

/* ----------- GLOBAL OBJECTS ----------- */

String CurrentClassAldor = NULL;
String CurrentClassCpp   = NULL;
const int theTRUE  = 1;
const int theFALSE = 0;

String commonOperators[] = { "+", "-", "/" };
int CO_size = sizeof(commonOperators)/sizeof(String);

/* Some types can not be used in an export statement */
String typesToSkip[] = { "HInt", "Bool", "Char", "SFlo", "XByte" };
int tTS_size = sizeof(typesToSkip)/sizeof(String);

String infixOps[] = { "+", "-", "/", "*", "mod", "rem", "quo", "by", "^", 
			     "/\\", "\\/", "=", "~", "~=", "<", "<=", ">", ">=" };
int IO_size = sizeof(infixOps)/sizeof(String);

String cppKeywords[] = { "case", "switch", "class" };
int CK_size = sizeof(cppKeywords)/sizeof(String);

int globalFuncCounter = 0;
const int TabZero = 0;

String *basicAldorTypes;
int bAT_size;
String *basicCppTypes; 
int basicHasBeenUsrDefined = 0;    /* basic type correspondence not defined yet */
int discriminationReturnType = 0;  /* no discrimination on return type */

#define CPPOPT_BASICFILE "basicfile="           /* file containing basic type correspondence */
#define CPPOPT_DISCRETYP "discrim-return"       /* discrimination on return type for overloading */
#define CPPOPT_NODISCRETYP "no-discrim-return"  /* no discrimination (default) */

/* ----------- BASIC TYPES default ------------- */

static String defaultBasicAldorTypes[] = { 
  "SingleInteger", "String", "Character", "SingleFloat", "DoubleFloat", 
  "Pointer", "()", "Word", "Ptr", "Bool", "Byte", "SInt", "Char", 
  "Arr", "Rec", "BInt", "DFlo" };
static int def_bAT_size = sizeof(defaultBasicAldorTypes)/sizeof(String);

static String defaultBasicCppTypes[]   = { 
  "int", "char *", "char", SF_STRING, "double*",
  "void *", "void", "FiWord", "FiPtr", "FiBool", "FiByte", "FiSInt", "FiChar",
  "FiArr", "FiRec", "FiBInt", "FiDFlo" };

/* ----------- DIRECTLY USED FROM genCpp -------------- */

/* THE MAIN FUNCTION */

void genCpp(AbSyn ab, String dir, String file) {
  /* From a sequence, two jobs are done:
     1- Determination of the "type" of the node
     2- Code generation
  */

  FILE *as_file, *cc_file;

  if (!ab) {
    fprintf(stderr, "C++ generation can not proceed.\nAbstract Syntax tree not available.\n");
    return;
  }

  if (!basicHasBeenUsrDefined) InitBasicTypesArray(NULL);
  CreateOutputFiles(dir,file,&cc_file,&as_file);

  if (SequenceCheck(ab)) return;

  /*  PreDeclarationOfClasses(ab,cc_file); This should not be needed anymore */
  ExportGen(ab,cc_file,as_file);
  CodeGen(ab,cc_file,as_file); 

  fclose(as_file);
  fclose(cc_file);
}

int cppOption(String opt) {
  String s;

  if ((s = strIsPrefix(CPPOPT_BASICFILE, opt))) {
    if (InitBasicTypesArray(s) == -1) return -1;
    basicHasBeenUsrDefined = theTRUE;
    return 0;
  } 

  if (strEqual(CPPOPT_DISCRETYP,opt)) {
    discriminationReturnType = theTRUE;
    return 0;
  }

  if (strEqual(CPPOPT_NODISCRETYP,opt)) {
    discriminationReturnType = theFALSE;
    return 0;
  }

  return -1;
}

void updateBasicTypes(String s, int typeNumber) {
  String current;
  int lg, i, previndex;
  
  lg = strlen(s);
  
  /* spaces in the front */
  i = 0;
  while ((s[i] == ' ') && (i < lg)) i++;
  
  /* type */
  previndex = i;
  while ((s[i] != ' ') && (i < lg)) i++;
  basicAldorTypes[typeNumber] = strAlloc(i-previndex+1);
  i = previndex;
  current = basicAldorTypes[typeNumber];
  while ((s[i] != ' ') && (i < lg)) {
    if (s[i] != '\n') {
      *current = s[i];
      current++;
    }
    i++;
  }
  *current = '\0';
  
  /* spaces in the middle */
  while ((s[i] == ' ') && (i < lg)) i++;
  
  /* type */
  previndex = i;
  while ((s[i] != ' ') && (i < lg)) i++;
  basicCppTypes[typeNumber] = strAlloc(i-previndex+1);
  i = previndex;
  current = basicCppTypes[typeNumber];
  while ((s[i] != ' ') && (i < lg)) {
    if (s[i] != '\n') {
      *current = s[i];
      current++;
    }
    i++;
  }
  *current = '\0';
}

int InitBasicTypesArray(String file) {
  /* If parameter is NULL, then no '-P' option has been used */
  /* returned value: 0 -> ok, -1 -> problem */

  FILE *basic = NULL;
  int numberOfTypes, i;
  String s = strAlloc(1000);

  /* Open the file */

  if (file) {
    basic = fopen(file,"r");
    if (!basic) return -1;
  }
  else { /* no '-P' option, we have to do it ourselves */
    /* Try 'basic.typ' in the current directory */
    basic = fopen("./basic.typ","r");
    if (!basic) { /* no user defined basic types */
      /* Try in the standard directory */
#if !AXL_EDIT_1_1_13_17
      String compRootDir = strCopyIf(osGetEnv("AXIOMXLROOT"));
#endif
      if (compRootDir) { 
	String s = strPrintf("%s/include/basic.typ",compRootDir);
	basic = fopen(s,"r");
      }
      if (!basic) {   /* argh ! no standard defined basic types */
	/* we use the default hard-coded one */
	fprintf(stderr,"Unable to find basic.typ\n");
	fprintf(stderr,"Using default basic types correspondence\n");
	basicAldorTypes = defaultBasicAldorTypes;
	bAT_size = def_bAT_size;
	basicCppTypes = defaultBasicCppTypes;
	return 0; /* no more work to do */
      }
    }
  }
    
  /* Create the list */

  /* 1st: how many lines in the file ? */
  numberOfTypes = 0;
  while (!feof(basic)) {
    IgnoreResult(fgets(s,1000,basic));
    numberOfTypes++;
  }
  numberOfTypes--; /* there is always one extra loop, don't know why */

  /* 2nd: allocation of memory */
  basicAldorTypes = (String *) malloc(numberOfTypes*sizeof(String));
  basicCppTypes = (String *) malloc(numberOfTypes*sizeof(String));
  bAT_size = numberOfTypes;

  /* 3rd: filling the arrays */
  rewind(basic);
  for (i = 0; i < numberOfTypes; i++) {
    IgnoreResult(fgets(s,1000,basic));
    updateBasicTypes(s, i);
  }

#if !defined(NDEBUG)
  fprintf(stderr, " Basic types correspondence:\n");
  for (i = 0; i < numberOfTypes; i++)
    fprintf(stderr," [%s] and [%s]\n",basicAldorTypes[i],basicCppTypes[i]);
#endif

  strFree(s);
  fclose(basic);
  return 0;
}


void CreateOutputFiles(String dir, String file, FILE **cc, FILE **as) {
  /*
    Nothing special here, we create the C++ and the Aldor files
    to output the generated code ...
  */

  String s;

  s = strPrintf("%s/%s_cc.h",dir,file);
  *cc = fopen(s,"w+");
  strFree(s);
  s = strPrintf("%s/%s_as.as",dir,file);
  *as = fopen(s,"w+");
  strFree(s);

  /* Output the headers (#include) */
  fprintf(*as,"#include \"%s.as\"\n",file);
}

int SequenceCheck(AbSyn ab) {
  /* A simple check to be sure we deal with a sequence */
  if (abTag(ab) != AB_Sequence) {
    fprintf(stderr,"Should be sequence ! (node code = %d)\n",abTag(ab));
    return 1;
  }
  return 0;
}


int DeterminationTypeOf(AbSyn ab) {
  /* 
     Depending on the structure of the subtree, it is possible to
     determine a correspondence with C++ types (classes, abstract
     classes, templates, variable, function) 
  */
  int typof;

  switch (abTag(ab)) {
  case AB_Define:
    typof = GettypDefine(ab);
    break;
  case AB_Assign:
    typof = AC_VAR;
    break;
  case AB_Local:
    typof = AC_VAR;
    break;
  case AB_Default:
    typof = AC_VAR;
    break;
  default:
    typof = AC_NONE;
    break;
  }

  return typof;
}

/* --------------------------------------------------------- */
/*            HERE COMES THE THREE MAIN FUNCTIONS            */
/*                                                           */
/*    1. PreDeclarationOfClasses => class A;                 */
/*    2. ExportGen => extern "C" and import from Foreign C   */
/*    3. CodeGen   => classes generation and stubs           */
/* --------------------------------------------------------- */

void PreDeclarationOfClasses(AbSyn ab, FILE *cc) {
  AbSyn tmp;
  int typof,i;

  /* C++ and Aldor code generation for predeclaration of classes in C++ */
  for (i=0; i < abArgc(ab); i++) {
    tmp = abSeqArg(ab,i);

    typof = DeterminationTypeOf(tmp);

    /* Code generation */
    switch (typof) {
    case AC_NONE:
      break;
    case AC_CLASS:
      fprintf(cc,"class %s;\n",abIdStr(abDeclareId(abDefineLhs(tmp))));
      if (abTag(abDeclareType(abDefineLhs(tmp))) == AB_With) 
	fprintf(cc,"class ALDORCategory%s;\n",abIdStr(abDeclareId(abDefineLhs(tmp))));
      break;
    case AC_ABS_CLASS:
      fprintf(cc,"class %s;\n",abIdStr(abDeclareId(abDefineLhs(tmp))));
      break;
    default:
      break;
    }
  }
}

void DebugGen(int typof) {
#if !defined(NDEBUG)
  switch (typof) {
  case AC_NONE:
    break;
  case AC_FUNCTION:
    fprintf(stderr,"Function generation\n");
    break;
  case AC_CLASS:
    fprintf(stderr,"Class generation\n");
    break;
  case AC_TEMPL_CLASS:
    fprintf(stderr,"Template class generation\n");
    break;
  case AC_ABS_CLASS:
    fprintf(stderr,"Abstract class generation\n");
    /* Should not be necessary ... well maybe for the default section ...*/
    break;
  case AC_ABS_TMPL_CLASS:
    fprintf(stderr,"Abstract template class generation\n");
    /* Should not be necessary ... well maybe for the default section ...*/
    break;
  case AC_VAR:
    fprintf(stderr,"Variable generation\n");
    break;
  default:
    break;
  }
#endif
}

void ExportGen(AbSyn ab, FILE *cc, FILE *as) {
  AbSyn tmp;
  int typof,i;

#if !defined(NDEBUG)
  fprintf(stderr,"\n\n    IMPORT/EXPORT GENERATION    \n\n");
#endif
  
  fprintf(as,"export {\n");
  fprintf(cc,"#include <foam_c.h>\n");
  fprintf(cc,"extern \"C\" {\n");

  globalFuncCounter = 0;

  /* C++ and Aldor code generation for C interface */
  for (i=0; i < abArgc(ab); i++) {
    tmp = abSeqArg(ab,i);

    typof = DeterminationTypeOf(tmp);

    /* Debug */
    DebugGen(typof);

    /* Code generation */
    switch (typof) {
    case AC_NONE:
      break;
    case AC_FUNCTION:
      GenExportFunction(tmp,as,cc);
      break;
    case AC_CLASS:
      GenExportClass(tmp,!BL_IsTmpl,as,cc);
      break;
    case AC_TEMPL_CLASS:
      GenExportClass(tmp,BL_IsTmpl,as,cc);
      break;
    case AC_ABS_CLASS:
      /* Should not be necessary ... well maybe for the default section ...*/
      break;
    case AC_ABS_TMPL_CLASS:
      /* Should not be necessary ... well maybe for the default section ...*/
      break;
    case AC_VAR:
      break;
    default:
      break;
    }
  }

  fprintf(as,"} to Foreign C;\n");
  fprintf(cc,"}\n");
}

void CodeGen(AbSyn ab, FILE *cc, FILE *as) {
  AbSyn tmp;
  int typof,i;

#if !defined(NDEBUG)
  fprintf(stderr,"\n\n    CODE GENERATION    \n\n");
#endif

  globalFuncCounter = 0;

  for (i=0; i < abArgc(ab); i++) {
    tmp = abSeqArg(ab,i);

    typof = DeterminationTypeOf(tmp);

    /* Debug */
    DebugGen(typof);

    /* Code generation */
    switch (typof) {
    case AC_NONE:
      break;
    case AC_FUNCTION:
      GenFunction(tmp,as,cc);
      break;
    case AC_CLASS:
      GenClassesForDomains(tmp,theFALSE,as,cc);
      break;
    case AC_TEMPL_CLASS:
      GenClassesForDomains(tmp,theTRUE,as,cc);
      break;
    case AC_ABS_CLASS:
      GenClassForCategories(tmp,theFALSE,as,cc);
      break;
    case AC_ABS_TMPL_CLASS:
      GenClassForCategories(tmp,theTRUE,as,cc);
      break;
    case AC_VAR:
      break;
    default:
      break;
    }
  }
}

/* ------------------------------------------------------------------- BUILDING CLASSES ---------- */

/* ---------- ERROR ---------- */

Class *errorBuildClass(Class *cl) {
  FreeClass(cl);
  return NULL;
}

/* ---------- END ERROR ---------- */

/* ---------- ID HANDLING ---------- */

void BuildParentIdAbs(Ident *id, AbSyn ab) {
  /* abTag(ab) should be AB_Apply */
  String tmp;

  if (abHasTag(ab,AB_Apply)) id->basic = abIdStr(abApplyOp(ab));
  else id->basic = abIdStr(ab); /* ab is an abId */
  tmp = BldId(ab);
  if (!tmp) {
    id->typeAldor = strPrintf("%s",abIdStr(ab));
    id->typeCpp   = strPrintf("%s<PercentType>",abIdStr(ab));
    id->export = id->basic;
  }
  else {
    id->typeAldor = strPrintf("%s(%s)",abIdStr(abApplyOp(ab)),tmp);
    id->typeCpp   = strPrintf("%s<PercentType,%s>",abIdStr(abApplyOp(ab)),tmp);
    id->export = id->basic;
    strFree(tmp);
  }
}

void BuildExtraClassId(Class *mother, Class *cl, int isTmpl) {
  if (!isTmpl) {
    cl->id.export = cl->id.basic;
    cl->id.typeAldor = strPrintf("%s",cl->id.basic);
    cl->id.typeCpp   = strPrintf("%s<%s>",cl->id.basic,mother->id.typeCpp);
  }
  else {
    if ((!tfIsMulti(cl->params)) && (!tfIsDeclare(cl->params))) {
      cl->id.export = cl->id.basic;
      cl->id.typeAldor = strPrintf("%s",cl->id.basic);
      cl->id.typeCpp   = strPrintf("%s<%s>",cl->id.basic,mother->id.typeCpp);
    }
    else if (tfIsDeclare(cl->params)) {
      String str = symString(tfDeclareId(cl->params));

      cl->id.export = strPrintf("%s%s", cl->id.basic, str);
      cl->id.typeCpp = strPrintf("%s<%s,%s>", cl->id.basic, mother->id.typeCpp, str);
      cl->id.typeAldor = strPrintf("%s(%s)", cl->id.basic, str);
    } 
    else {
      String str;
      String str_tmp = InitStr();
      size_t i,j;
      j = tfMultiArgc(cl->params);

      /* Create strings for the identifiers used for import/export, in C++, in Aldor */

      /* export */
      str_tmp = STRCAT(str_tmp,cl->id.basic);
      for (i = 0; i < j; i++) {
	str = symString(tfDeclareId(tfMultiArgN(cl->params,i)));
	str_tmp = STRCAT(str_tmp,str);
      }
      cl->id.export = strCopy(str_tmp);

      /* typeCpp */
      str_tmp[0] = 0;
      str_tmp = STRCAT(str_tmp,cl->id.basic);
      str_tmp = STRCAT(str_tmp,"<");
      str_tmp = STRCAT(str_tmp,mother->id.typeCpp);
      if (j) str_tmp = STRCAT(str_tmp,",");
      for (i = 0; i < j; i++) {
	str = symString(tfDeclareId(tfMultiArgN(cl->params,i)));
	str_tmp = STRCAT(str_tmp,str);
	if (i < j-1) str_tmp = STRCAT(str_tmp,",");
      }
      str_tmp = STRCAT(str_tmp,">");
      cl->id.typeCpp = strCopy(str_tmp);

      /* typeAldor */
      str_tmp[0] = 0;
      str_tmp = STRCAT(str_tmp,cl->id.basic);
      str_tmp = STRCAT(str_tmp,"(");
      if (j) str_tmp = STRCAT(str_tmp,",");
      for (i = 0; i < j; i++) {
	str = symString(tfDeclareId(tfMultiArgN(cl->params,i)));
	str_tmp = STRCAT(str_tmp,str);
	if (i < j-1) str_tmp = STRCAT(str_tmp,",");
      }
      str_tmp = STRCAT(str_tmp,")");
      cl->id.typeAldor = strCopy(str_tmp);

      strFree(str_tmp);
    }
  }
}

void BuildClassId(Class *cl, int isTmpl) {
  if (!isTmpl)
    cl->id.export = cl->id.typeAldor = cl->id.typeCpp = cl->id.basic;
  else {
    size_t i,j;
    String str;

    /* Compute the lengths of the strings export and type ... */
    if ((!tfIsMulti(cl->params)) && (!tfIsDeclare(cl->params))) 
      cl->id.export = cl->id.typeAldor = cl->id.typeCpp = cl->id.basic;
    else if (tfIsDeclare(cl->params)) {
      str = symString(tfDeclareId(cl->params));
      cl->id.export = strPrintf("%s%s", cl->id.basic, str);
      cl->id.typeCpp = strPrintf("%s<%s>", cl->id.basic, str);
      cl->id.typeAldor = strPrintf("%s(%s)", cl->id.basic, str);
    } else {
      String str_tmp = InitStr();
      j = tfMultiArgc(cl->params);

      /* Create strings */
      
      /* export */
      str_tmp = STRCAT(str_tmp,cl->id.basic);
      for (i = 0; i < j; i++) {
	str = symString(tfDeclareId(tfMultiArgN(cl->params,i)));
	str_tmp = STRCAT(str_tmp,str);
      }
      cl->id.export = strCopy(str_tmp);

      /* typeCpp */
      str_tmp[0] = 0;
      str_tmp = STRCAT(str_tmp,cl->id.basic);
      str_tmp = STRCAT(str_tmp,"<");
      for (i = 0; i < j; i++) {
	str = symString(tfDeclareId(tfMultiArgN(cl->params,i)));
	str_tmp = STRCAT(str_tmp,str);
	if (i < j-1) str_tmp = STRCAT(str_tmp,",");
      }
      str_tmp = STRCAT(str_tmp,">");
      cl->id.typeCpp = strCopy(str_tmp);

      /* typeAldor */
      str_tmp[0] = 0;
      str_tmp = STRCAT(str_tmp,cl->id.basic);
      str_tmp = STRCAT(str_tmp,"(");
      for (i = 0; i < j; i++) {
	str = symString(tfDeclareId(tfMultiArgN(cl->params,i)));
	str_tmp = STRCAT(str_tmp,str);
	if (i < j-1) str_tmp = STRCAT(str_tmp,",");
      }
      str_tmp = STRCAT(str_tmp,")");
      cl->id.typeAldor = strCopy(str_tmp);

      strFree(str_tmp);
    }
  }
}

String BldId(AbSyn ab) {
  String result;
  String tmp2 = NULL;
  int i;

  if (!abHasTag(ab,AB_Apply)) return NULL;

  result = InitStr();

  for (i = 0; i < abApplyArgc(ab); i++) {
    switch (abTag(abApplyArg(ab,i))) {
    case AB_Id:
      if (result[0]) result = STRCAT(result,","); /* if (result[0]) then this is not the first time, so we need a comma */
      result = STRCAT(result,abIdStr(abApplyArg(ab,i)));
      break;
    case AB_Apply:
      tmp2 = BldId(abApplyArg(ab,i));
      if (!tmp2) { strFree(result); return NULL; }
      if (result[0]) result = STRCAT(result,",");  /* for the comma */
      result = STRCAT(result,abIdStr(abApplyOp(ab)));
      result = STRCAT(result,"<");
      result = STRCAT(result,tmp2);
      result = STRCAT(result,">");
      strFree(tmp2);
      break;
    default:
      break;
    }
  }
  if (!(result[0])) { strFree(result); return NULL; }
  tmp2 = strCopy(result);
  strFree(result);
  return tmp2;
}

void BuildParentId(Ident *id, AbSyn ab) {
  /* abTag(ab) should be AB_Apply */
  String tmp;

  if (abHasTag(ab,AB_Apply)) {
    id->basic = abIdStr(abApplyOp(ab));
    id->export = id->basic;
    tmp = BldId(ab);
    if (tmp) {
      id->typeAldor = strPrintf("%s(%s)",abIdStr(abApplyOp(ab)),tmp);
      id->typeCpp = strPrintf("%s<%s,%s>",abIdStr(abApplyOp(ab)),CurrentClassCpp,tmp);
      strFree(tmp);
    }
  }
  else {
    id->basic = abIdStr(ab); /* ab is an abId */
    id->export = id->basic;
    id->typeAldor = strPrintf("%s",abIdStr(ab));
    id->typeCpp = strPrintf("%s<%s>",abIdStr(ab),CurrentClassCpp);
  }
}

/* ---------- END ID HANDLING ---------- */

/* ---------- MAIN FUNCTIONS ---------- */

void getParentsAbs(Class *cl, TForm tf, int isTmpl) {
  AbSyn ab;
  Ident *id;
  int i;

  switch (tfTag(tf)) {
  case TF_Multiple:
    return;
  case TF_General:
    if (!tfHasExpr(tf)) return;
    ab = tfGetExpr(tf);
    switch (abTag(ab)) {
    case AB_Id:
      id = alloc(Ident);
      BuildParentIdAbs(id,ab);
      Append(cl->parents,CT_PARENT,id);
      break;
    case AB_Apply:
      id = alloc(Ident);
      BuildParentIdAbs(id,ab);
      Append(cl->parents,CT_PARENT,id);
      break;
    default:
      break;
    }
    break;
  case TF_With:
    getParentsAbs(cl,tfWithBase(tf),isTmpl);
    break;
  case TF_Join:
    for (i=0; i < tfJoinArgc(tf); i++) getParentsAbs(cl,tfJoinArgN(tf,i),isTmpl);
    break;
  default:
    fprintf(stderr,"Error in building class (getParents): %d\n",tfTag(tf));
    break;
  }
}

Class *BuildAbstractClass(AbSyn ab, int isTmpl) {
  /* 
     1. BuildAbstractClass is working for abTag(ab) == AB_Define
     
     2. abDefine has two parts:
     a) lhs => declaration (~ header == X: Category)
     b) rhs => definition  (~ body   == Join(Y,Z) with {...})  
	  
     3. BuildClass allocates CurrentClass
  */

  Class *cl;
  TForm tf;

  if (abTag(ab) != AB_Define) return NULL;

  cl = InitClass();

  /* ---- id */
  cl->id.basic = abIdStr(abDeclareId(abDefineLhs(ab)));

  /* ---- Body ---- */
  tf = abTUnique(abDefineLhs(ab));

  /* -- params */
  if (isTmpl) {
    cl->params = tfMapArg(tf);
    if (ToSkipClass(cl)) {
      FreeClass(cl);
      return NULL;
    }
  }

  /* -- methods */
  if (isTmpl) tf=tfMapRet(tf);
  tf = tfFollowDefDecl(tf);

  if (!tfIsThird(tf)) return errorBuildClass(cl);

  if (!tfThdExports(tf)) {
    /* we are in this case when this is a template abstract class */
    if (!tfArgc(tf)) return errorBuildClass(cl);
    tf = tfThirdRestrictions(tf);
    if (!tfIsWith(tf)) return errorBuildClass(cl);
    tf = tfTUnique(tf);
    if (!tfIsThird(tf)) return errorBuildClass(cl);
    if (!tfThdExports(tf)) return errorBuildClass(cl);
  } 

  /* Here tfThdExports(tf) contains all the definitions of the methods */
  cl->methods = tf;

  /* ---- id */
  BuildClassId(cl,isTmpl);
  
  CurrentClassCpp = "PercentType";
  CurrentClassAldor = cl->id.typeAldor;

  /* -- Inheritance */

  tf = abTUnique(abDefineLhs(ab));

  if (isTmpl) tf = tfMapRet(tf);
  tf = tfFollowDefDecl(tf);
  tf = tfThirdRestrictions(tf);
  getParentsAbs(cl,tf,isTmpl);

  
  /* ---- free some memory */
  if (Empty(cl->extraClasses)) { stoFree(cl->extraClasses); cl->extraClasses = NULL;} 
  if (Empty(cl->parents)) { stoFree(cl->parents); cl->parents = NULL;} /* we KNOW that cl->parents != NULL */

  return cl;
}

Class *buildExtraClass(Class *cl, TForm tf, int isTmpl) {
  String str_tmp;
  String str_tmp2;
  TForm tmp;
  Class *extra = InitClass();

  /* id */
  extra->id.basic = strPrintf("ALDORCategory%s%d",cl->id.basic,cl->classCounter);

  /* params */
  if (isTmpl) extra->params = cl->params;
  
  /* id is back */
  BuildExtraClassId(cl,extra,isTmpl);

  /* inheritance */
  str_tmp = CurrentClassAldor;
  str_tmp2 = CurrentClassCpp;
  CurrentClassAldor = extra->id.typeAldor;
  CurrentClassCpp = "PercentType";
  getParents(extra,tfWithBase(tf),isTmpl);
  CurrentClassAldor = str_tmp;
  CurrentClassCpp = str_tmp2;

  /* body */
  tmp = tfWithWithin(tf);
  if (!tfIsThird(tmp)) {
    tmp = tfTUnique(tmp);
    tmp = tfFollowDefDecl(tmp);
  }
  if (!tfIsThird(tmp)) extra->methods = NULL;
  else extra->methods = tmp;

  /* ---- free some memory */
  if (Empty(extra->extraClasses)) { stoFree(extra->extraClasses); extra->extraClasses = NULL; }
  if (Empty(extra->parents))      { stoFree(extra->parents); extra->parents = NULL; } /* we KNOW that extra->params != NULL */

  return extra;
}

void getParents(Class *cl, TForm tf, int isTmpl) {
  Class *extra;
  AbSyn ab;
  Ident *id;
  int i;

  switch (tfTag(tf)) {
  case TF_Multiple:
    return;
  case TF_General:
    if (!tfHasExpr(tf)) return;
    ab = tfGetExpr(tf);
    switch (abTag(ab)) {
    case AB_Id:
      id = alloc(Ident);
      BuildParentId(id,ab);
      Append(cl->parents,CT_PARENT,id);
      break;
    case AB_Apply:
      id = alloc(Ident);
      BuildParentId(id,ab);
      Append(cl->parents,CT_PARENT,id);
      break;
    default:
      break;
    }
    break;
  case TF_With:
    extra = buildExtraClass(cl,tf,isTmpl);
    if (extra) {
      Append(cl->extraClasses,CT_EXTRA,extra);      /* Create extra Class Il peut y avoir plusieurs extra classes */
      cl->classCounter++;
      Append(cl->parents,CT_PARENT,&(extra->id));
    }
    break;
  case TF_Join:
    for (i=0; i < tfJoinArgc(tf); i++) getParents(cl,tfJoinArgN(tf,i),isTmpl);
    break;
  default:
    fprintf(stderr,"Error in building class (getParents): %d\n",tfTag(tf));
    break;
  }
}

Class *BuildClass(AbSyn ab, int isTmpl) {
  /* 
     1. BuildClass is working for abTag(ab) == AB_Define
     
     2. abDefine has two parts:
     a) lhs => declaration (~ header == X: some category)
     b) rhs => definition  (~ body   == Y add {...})  
	  
     3. BuildClass allocates CurrentClass
  */

  /* Variables declaration */
  Class *cl;
  TForm tf, tmp;
  
  /* Source code */
  if (abTag(ab) != AB_Define) return NULL;

  cl = InitClass();

  /* ---- id */
  cl->id.basic = abIdStr(abDeclareId(abDefineLhs(ab)));

  /* ---- Body */
  tf = abTUnique(abDefineLhs(ab));

  /* params */
  if (isTmpl) {
    cl->params = tfMapArg(tf);
    if (ToSkipClass(cl)) {
      FreeClass(cl);
      return NULL;
    }
  }

  /* Methods */
  tmp = tf;
  if (isTmpl) tmp = tfMapRet(tmp);
  tmp = tfTUnique(tfFollowDefDecl(tmp));
  tmp = tfFollowDefDecl(tmp);

  if (!tfIsThird(tmp)) return errorBuildClass(cl);

  cl->methods = tmp;

  /* ---- id */
  BuildClassId(cl,isTmpl);

  CurrentClassAldor = cl->id.typeAldor;
  CurrentClassCpp = cl->id.typeCpp;

  /* Parents */
  tmp = tf;
  if (isTmpl) tmp = tfMapRet(tmp);
  tmp = tfFollowDefDecl(tmp);
  getParents(cl,tmp,isTmpl);

  /* ---- free some memory */
  if (Empty(cl->extraClasses)) {stoFree(cl->extraClasses); cl->extraClasses = NULL;} 
  if (Empty(cl->parents)) {stoFree(cl->parents); cl->parents = NULL;} /* we KNOW that cl->parents != NULL */

  return cl;
}

/* ---------- END MAIN FUNCTIONS ---------- */

/* -------------------------------------------------------------- END BUILDING CLASSES ---------- */

/* 
%   ------------------------------------------------
%                 Determination part
%   ------------------------------------------------
*/


cpp_types detCppType(AbSyn ab) {
  TForm tf;

  if (!ab) return AC_NONE;

  tf = abTUnique(ab); /* get the tform */

  tf = tfFollowDefDecl(tf); /* get the interesting cell */

  if (tfIsMap(tf)) {
    tf = tfMapRet(tf); /* get the return type of the Map */

    tf = tfFollowDefDecl(tf); /* get the interesting cell */

    /* param category ? */
    if (tfIsThird(tf)) return AC_ABS_TMPL_CLASS;

    /* param domain ? */
    tf = tfFollowDefDecl(tfTUnique(tf));
    if (tfIsThird(tf)) return AC_TEMPL_CLASS;

    /* still here ? well it is a function */
    return AC_FUNCTION;
  }

  /* tf is not a TF_Map */
  
  /* category ? */
  if (tfIsThird(tf)) return AC_ABS_CLASS;

  /* domain ? */
  tf = tfFollowDefDecl(tfTUnique(tf));
  if (tfIsThird(tf)) return AC_CLASS;

  /* still here ? well it is a constant */
  return AC_CST;
}

cpp_types GettypDefine(AbSyn ab) {

  if (abTag(ab) == AB_Local) return AC_VAR;
  if (abTag(ab) == AB_Default) return AC_VAR;
  if (abTag(ab) != AB_Define) return AC_NONE;
  /* if the object is not a definition then we can't determine the type */

#if !defined(NDEBUG)
  if (abTag(abDefineLhs(ab)) == AB_Declare)
    fprintf(stderr,"%s ",abIdStr(abDeclareId(abDefineLhs(ab))));
#endif

  return detCppType(abDefineLhs(ab));
}

/* Bugs and to do:
. If SInt is specified without '$Machine' (using 'import from Machine;' for example), then
the program doesn't translate SInt to FiSInt
. Multiple return with a % in the list of returned things => just crash I think
. overloading discrimine sur type de retour
. function pointers
. default parameters
. value parameters for domains (like SI or SF)
. global variable
. Overloading resolved only on return type is not handled.
*/

int scanForBelongsTo(String *paramList, int lgList, String toCompare) {
  int i;
  for (i=0 ; i < lgList; i++)
    if (!strcmp(toCompare,paramList[i])) return theTRUE;
  return theFALSE;
}

int abBelongsTo(AbSyn ab, String *paramList, int lgList) {

  int i;

  switch (abTag(ab)) {
  case AB_Id:
    return scanForBelongsTo(paramList,lgList,abIdStr(ab));
  case AB_Apply:
    if (scanForBelongsTo(paramList,lgList,abIdStr(abApplyOp(ab)))) return theTRUE;
    for (i = 0; i < abApplyArgc(ab); i++)
      if (abBelongsTo(abApplyArg(ab,i),paramList,lgList)) return theTRUE;
    return theFALSE;
  case AB_Nothing:
    return theFALSE;
  case AB_Comma:
    for (i=0; i < abArgc(ab); i++)
      if (abBelongsTo(abCommaArg(ab,i),paramList,lgList)) return theTRUE;
    return theFALSE;
  case AB_Qualify:
    if (abBelongsTo(ab->abQualify.what,paramList,lgList)) return theTRUE;
    if (abBelongsTo(ab->abQualify.origin,paramList,lgList)) return theTRUE;
    return theFALSE;
  case AB_Declare:
    if (abBelongsTo(abDeclareType(ab),paramList,lgList)) return theTRUE;
    return theFALSE;
  default:
    return theFALSE;
  }    

}

int BelongsTo(TForm type, String *paramList, int lgList) {
  /* two cases: either we get an absyn from the type,
     or it is a multiple value (A,B,C) */
  AbSyn ab = tfGetExpr(type);
  int i, lg;

  if (ab) return abBelongsTo(ab,paramList,lgList);

  if (!tfIsMulti(type)) return theFALSE; /* we don't know how to check so it doesn't belong to it */
  lg = tfMultiArgc(type);
  for (i = 0; i < lg; i++)
    if (BelongsTo(tfMultiArgN(type,i),paramList,lgList)) return theTRUE;
  return theFALSE;
}

int HasDependentTypes(TForm tf) {
  int paramNb, returnNb;
  int actualParamNb, actualReturnNb;
  TForm args, rets;
  String *argsList;
  String *retsList;

  /*
    We want to check if a function uses dependent types,
    (e.g f: (E: Type, a: E) -> E). If yes, then we will skip the function.
    To do that, we are going to consider the String values of the names
    of parameters. Here we store in an array the names of all parameters
    and then for each parameter and returned type, we check if its type
    is or uses any element of the array.
    To deal with things like:
    g: (a: E, E: Type) -> E
    we need to store all the parameters names, that is why we use an array.
  */

  /* We get a pointer to the arguments and the number of arguments */
  if (!tfIsMap(tf)) return theFALSE;
  args = tfMapArg(tf);
  paramNb = tfIsMulti(args) ? tfMultiArgc(args) : 1;

  /* We get a pointer to the returned values and the number of returned values */
  rets = tfMapRet(tf);
  returnNb = tfIsMulti(rets) ? tfMultiArgc(rets) : 1;

  /* Let's fill the array for parameters */
  actualParamNb = 0; 
  argsList = NULL;
  if (paramNb) {
    argsList = (String *)stoAlloc(int0,paramNb*sizeof(String));
    if (paramNb == 1) {
      if (tfIsDeclare(args)) argsList[actualParamNb++] = symString(tfDeclareId(args));
      /* should be a "tfDeclare", but it may be a function prototype, so if
	 it is not a tfDeclare we don't do anything */
    } else {
      int i;
      for (i=0 ; i<paramNb; i++) {
	TForm tmp = tfMultiArgN(args,i);
	if (tfIsDeclare(tmp)) argsList[actualParamNb++] = symString(tfDeclareId(tmp));
	/* should be a "tfDeclare", but it may be a function prototype, so if
	   it is not a tfDeclare we don't do anything */
      }
    }
    

    /* Let's check the type of the parameters */
    if (paramNb == 1) {
      if (tfIsDeclare(args))
	if (BelongsTo(tfDeclareType(args),argsList,actualParamNb)) {
	  if (argsList) stoFree(argsList); 
	  return theTRUE;
	}
      /* should be a "tfDeclare", but it may be a function prototype, so if
	 it is not a tfDeclare we don't do anything */
    } else {
      int i;
      for (i=0 ; i<paramNb; i++) {
	TForm tmp = tfMultiArgN(args,i);
	if (tfIsDeclare(tmp)) 
	  if (BelongsTo(tfDeclareType(tmp),argsList,actualParamNb)) {
	    if (argsList) stoFree(argsList); 
	    return theTRUE;
	  }
	/* should be a "tfDeclare", but it may be a function prototype, so if
	   it is not a tfDeclare we don't do anything */
      }
    }
  }


  actualReturnNb = 0;
  retsList = NULL;
  if (returnNb) {

    int i;
    retsList = (String *)stoAlloc(int0,(actualParamNb+returnNb)*sizeof(String));
    
    /* we merge the list of parameters and the list of returned values */
    if (argsList) {
      for (i=0; i < actualParamNb; i++) retsList[i] = argsList[i];
      stoFree(argsList);
      argsList = NULL;
    }

    actualReturnNb = actualParamNb;

    /* Filling array for return values */
    if (returnNb == 1) {
      if (tfIsDeclare(rets)) retsList[actualReturnNb++] = symString(tfDeclareId(rets));
      /* should be a "tfDeclare", but it may be a function prototype, so if
	 it is not a tfDeclare we don't do anything */
    } else {
      int i;
      for (i=0 ; i<returnNb; i++) {
	TForm tmp = tfMultiArgN(rets,i);
	if (tfIsDeclare(tmp)) retsList[actualReturnNb++] = symString(tfDeclareId(tmp));
	/* should be a "tfDeclare", but it may be a function prototype, so if
	   it is not a tfDeclare we don't do anything */
      }
    }

    /* Let's check the return value */
    if (returnNb == 1) {
      if (BelongsTo(rets,retsList,actualReturnNb)) {
	if (retsList) stoFree(retsList); 
	return theTRUE;
      }
      /* should be a "tfDeclare", but it may be a function prototype, so if
	 it is not a tfDeclare we don't do anything */
    } else {
      int i;
      for (i=0 ; i<returnNb; i++) {
	TForm tmp = tfMultiArgN(rets,i);
	if (BelongsTo(tmp,retsList,actualReturnNb)) {
	  if (retsList) stoFree(retsList); 
	  return theTRUE;
	}
	/* should be a "tfDeclare", but it may be a function prototype, so if
	   it is not a tfDeclare we don't do anything */
      }
    }
  }
  
  /* Free the memory */
  if (argsList) stoFree(argsList);
  if (retsList) stoFree(retsList);

  return theFALSE;

}

int CheckSkipClassParam(TForm tf) {
  /* Does tf describe an appropriate parameter for a class ? */
  /* return: theTRUE -> ok, we don't skip
             theFALSE -> not ok, we skip
  */

  /* Rules:
        TF_Type => OK
	TF_General
	       -> Look at tfUnique in absyn of tf
	              -> TF_Third => OK
		      -> else NOT OK
        else NOT OK

	We should thus allow regular categories and parameterized categories.
	We don't allow function pointers, value, or inline categories.
  */

  AbSyn ab;
  TForm tmp;

  if (tfIsType(tf)) return theTRUE;
  if (!tfIsGeneral(tf)) return theFALSE;
  ab = tfGetExpr(tf);
  if (!ab) return theFALSE; /* should not happen, it is safer to skip */
  tmp = abTUnique(ab);
  if (!tmp) return theFALSE; /* should not happen, it is safer to skip */
  tmp = tfFollowDefDecl(tmp);
  if (!tmp) return theFALSE; /* should not happen, it is safer to skip */
  if (!tfIsThird(tmp)) return theFALSE; /* something we don't handle */
  return theTRUE; /* TF_Third corresponds to a regular or parameterized non-inline category */
}

int ToSkipClass(Class *cl) {
  /* precondition: the class cl has to be built */

  /* We check if the parameters of a class are suitable 
     for the interface, we skip everything which is not
     of type non-inline category (we don't handle things like:
         A(E: with { .... }) ...
  */
  /* 
     Return: theTRUE -> to skip
             theFALSE -> no skip
  */

  TForm tf = cl->params;

  /* if the type is not parameterized, nothing has to be done here */
  if (!tf) return theFALSE;

  /* 
     parameters are either tfDeclare (1 param) or tfMulti (0 or more than 1 param)
     if not it is safer to skip that 
  */
  if (!tfIsDeclare(tf) && !tfIsMulti(tf)) return theTRUE;

  if (tfIsDeclare(tf))
    return (!CheckSkipClassParam(tfDeclareType(tf)));
  else {
    int i = 0;
    int sizeMulti = tfMultiArgc(tf);

    for ( i=0 ; i < sizeMulti ; i++) {
      TForm tmp = tfMultiArgN(tf,i);
      if (!tfIsDeclare(tmp)) return theTRUE; /* should not happen, safer to skip */
      if (!CheckSkipClassParam(tfDeclareType(tmp))) return theTRUE;
    }
  }
  return theFALSE;
}

int ToSkip(TForm tf) {
  int paramNb, returnNb;
  TForm args, rets;

  /*
    We want to check if a function uses forbidden types,
    for that we use the array typesToSkip.
  */

  /* We get a pointer to the arguments and the number of arguments */
  if (!tfIsMap(tf)) return theFALSE;
  args = tfMapArg(tf);
  paramNb = tfIsMulti(args) ? tfMultiArgc(args) : 1;

  /* We get a pointer to the returned values and the number of returned values */
  rets = tfMapRet(tf);
  returnNb = tfIsMulti(rets) ? tfMultiArgc(rets) : 1;

  if (paramNb) {
    if (paramNb == 1) {
      if (tfIsDeclare(args)) {
	if (BelongsTo(tfDeclareType(args),typesToSkip,tTS_size)) return theTRUE;
      } else {
	if (BelongsTo(args,typesToSkip,tTS_size)) return theTRUE;
      }
      /* should be a "tfDeclare", but it may be a function prototype, so if
	 it is not a tfDeclare we don't do anything */
    } else {
      int i;
      for (i=0 ; i<paramNb; i++) {
	TForm tmp = tfMultiArgN(args,i);
	if (tfIsDeclare(tmp)) {
	  if (BelongsTo(tfDeclareType(tmp),typesToSkip,tTS_size)) return theTRUE;
	} else {
	  if (BelongsTo(tmp,typesToSkip,tTS_size)) return theTRUE;
	}
      }
    }
  }

  if (!returnNb) return theFALSE;

  if (returnNb == 1) {
    if (tfIsDeclare(rets)) {
      if (BelongsTo(tfDeclareType(rets),typesToSkip,tTS_size)) return theTRUE;
    } else {
      if (BelongsTo(rets,typesToSkip,tTS_size)) return theTRUE;
    }
  } else {
    int i;
    for (i=0 ; i<returnNb; i++) {
      TForm tmp = tfMultiArgN(rets,i);
      if (tfIsDeclare(tmp)) {
	if (BelongsTo(tfDeclareType(tmp),typesToSkip,tTS_size)) return theTRUE;
      } else {
	if (BelongsTo(tmp,typesToSkip,tTS_size)) return theTRUE;
      }
    }
  }
  
  return theFALSE;
}

/* ---------- FUNCTION SIGNATURES ---------- */

String GenAldorFnSig(TForm tf, String name, Class *cl, int tab, int isExported, int usePercent, int useTmplParms) {
  String str_tmp,result;

  if (!tfIsMap(tf)) return NULL;

  if (HasDependentTypes(tf)) return NULL;
  if (ToSkip(tf)) return NULL;

  result = InitStr();

  /* Function id */
  result = sprintTab(result,tab);
  str_tmp = GenAldorFuncName(name,cl,isExported);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp); result = STRCAT(result,": ");
  strFree(str_tmp);

  /* Parameters */
  str_tmp = GenAldorParams(tf,cl,BL_WithType,!BL_WithName,!BL_InBody,usePercent,useTmplParms);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp); 
  strFree(str_tmp);

  /* Return values */
  str_tmp = TFormToAldor(tfMapRet(tf),usePercent);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result," -> "); 
  result = STRCAT(result,str_tmp); 
  result = STRCAT(result,";\n");
  strFree(str_tmp);

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenCppFnSig(TForm tf, String name, Class *cl, int tab, int isExported) {
  String str_tmp;
  TForm tmp;
  String result;

  if (!tfIsMap(tf)) return NULL;

  if (HasDependentTypes(tf)) return NULL;
  if (ToSkip(tf)) return NULL;

  result = InitStr();

  result = sprintTab(result,tab);  

  /* return */
  tmp = tfMapRet(tf);
  if (tfIsMulti(tmp)) result = STRCAT(result,"void"); /* means multiple return or no return */
  else if (tfIsExit(tmp)) { strFree(result); return NULL; }
  else if (tfIsGenerator(tmp)) { strFree(result); return NULL; }
  else {
    /* If we have function pointers, this is a special case */
    if (tfIsFuncPtr(tmp))
      result = STRCAT(result, "void *");
    else {
      str_tmp = AbSynToCpp(tfGetExpr(tmp));
      if (!str_tmp) { strFree(result); return NULL; }
      if (AbSynIsUserDef(tfGetExpr(tmp)))
	result = STRCAT(result,"void *"); 
      /* currently (maybe for a long time !) it is void * and not the actual type,
	 because: 1) it is easier 2) we don't need any type checking ... */
      else
	result = STRCAT(result,str_tmp); 
      strFree(str_tmp);
    }
  }

  result = STRCAT(result," ");

  str_tmp = createFnIdExternC(name,cl);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);

  str_tmp = GenCppParams(tf,cl,BL_WithType,!BL_WithName,BL_IsStatic,!BL_Abstract,BL_Export);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp); result = STRCAT(result,";\n");
  strFree(str_tmp);

  if (strstr(result, "fputc") != NULL)
  {
    str_tmp = InitStr();
    STRCAT(str_tmp, "extern int fputc();\n");
  }
  
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

/* ---------- END FUNCTION SIGNATURES ---------- */

/* ---------- STUBS ---------- */

String GenAldorStubs(TForm tf, String name, Class *cl, int Global, int tab) {
  String result,str_tmp;
  int isInfix;

  if (!tfIsMap(tf)) return NULL;

  if (HasDependentTypes(tf)) return NULL;
  if (ToSkip(tf)) return NULL;

  result = InitStr();

  /* function id */
  result = sprintTab(result,tab);  
  str_tmp = GenAldorFuncName(name,cl,BL_Exported);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);

  /* params */
  str_tmp = GenAldorParams(tf,cl,BL_WithType,BL_WithName,!BL_InBody,!BL_UsePercent,BL_UseTmplParms);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp); result = STRCAT(result,": ");
  strFree(str_tmp);

  /* return */
  str_tmp = TFormToAldor(tfMapRet(tf),theFALSE);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp); result = STRCAT(result," == ");
  strFree(str_tmp);

  /* body */

  /* -- header */
  result = STRCAT(result,"{ \n");
  result = sprintTab(result,tab+1);
  if (!Global) {
    str_tmp = OutputDomNameAldor(cl->id.typeAldor);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,"import from "); result = STRCAT(result,str_tmp); result = STRCAT(result,";\n");
    strFree(str_tmp);
    result = sprintTab(result,tab+1);
  }

  /* -- id */

  /* ---- output */
  isInfix = IsInfixOperator(name);
  
  if (!isInfix) {

    String s = AldorifySpeSym(name);
    result = STRCAT(result,s);
    strFree(s);

    /* -- params */
    str_tmp = GenAldorParams(tf,cl,!BL_WithType,BL_WithName,BL_InBody,!BL_UsePercent,BL_UseTmplParms);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);

  } else { /* Operator is infix */

    int nbParams,cnt;
    tf = tfMapArg(tf);
    nbParams = tfIsMulti(tf) ? tfMultiArgc(tf) : 1;

    /* nbParams == 1 or 2 */
    if (nbParams == 1) {
      result = STRCAT(result,name);
      cnt = 0;
      str_tmp = GenAldorOneParam(tf,!BL_WithType,BL_WithName,cnt,!BL_UsePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result," ("); result = STRCAT(result,str_tmp); result = STRCAT(result,")");
      strFree(str_tmp);
    } 
    else if (nbParams != 2) { strFree(result); return NULL; }
    else { 
      /* output first param */
      cnt = 0;
      str_tmp = GenAldorOneParam(tfMultiArgN(tf,(Length) 0),!BL_WithType,BL_WithName,cnt,!BL_UsePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,"("); result = STRCAT(result,str_tmp); result = STRCAT(result,") ");
      strFree(str_tmp);
      
      /* output name */
      result = STRCAT(result, name);

      /* output second param */
      cnt = 1;
      str_tmp = GenAldorOneParam(tfMultiArgN(tf,1),!BL_WithType,BL_WithName,cnt,!BL_UsePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result," ("); result = STRCAT(result,str_tmp); result = STRCAT(result,")");
      strFree(str_tmp);
    }

  }

  result = STRCAT(result,";\n");
  result = sprintTab(result,tab);
  result = STRCAT(result,"}\n");
  
  return result;
}

/* ---------- END STUBS ---------- */

/* ---------- GLOBAL FUNCTIONS ---------- */

/* Check multiple return, params, ... for function pointers */

/* Create the String corresponding to function pointer parameters */
String GenCppFunc_ParamsRetFnPtr(TForm tf, Class *cl, int isStatic, int abstract, int export) {
  TForm retType;
  String result, str_tmp, str_tmp2;

  if (!tfIsMap(tf)) return NULL;
  retType = tfMapRet(tf);
  if (!tfIsFuncPtr(retType)) {
    /* base case */
    str_tmp = GenCppParams(tf,cl,BL_WithType,!BL_WithName,isStatic,abstract,export);
    if (!str_tmp) return NULL;
    result = InitStr();
    result = STRCAT(result,")"); result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  }
  else {
    str_tmp2 = GenCppFunc_ParamsRetFnPtr(tfMapRet(tf),cl,isStatic,abstract,export);
    str_tmp = GenCppParams(tf,cl,BL_WithType,!BL_WithName,isStatic,abstract,export);
    if (!str_tmp) return NULL;
    result = InitStr();
    result = STRCAT(result,")"); result = STRCAT(result,str_tmp);
    result = STRCAT(result,str_tmp2);
    strFree(str_tmp);
    strFree(str_tmp2);
  }

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

/* Create the String corresponding to a function pointer return value */
String GenCppFunc_RetFnPtr(TForm tf) {
  String result;
  String str_tmp;
  TForm retType;
  AbSyn ab;

  if (!tfIsFuncPtr(tf)) {
    /* base case: if ab is not a function pointer, 
       then we translate directly */
    ab = tfGetExpr(tf);
    if (!ab) return NULL;
    str_tmp = AbSynToCpp(ab);
    if (!str_tmp) return NULL;
    result = InitStr();
    result = STRCAT(result,str_tmp); 
    if (AbSynIsUserDef(ab)) result = STRCAT(result,"*");
    strFree(str_tmp);
  }
  else {
    /* general case: if the return type is a function pointer,
       we call recursively the function */
    retType = tfMapRet(tf);
    str_tmp = GenCppFunc_RetFnPtr(retType);
    if (!str_tmp) return NULL;
    result = InitStr();
    result = STRCAT(result,str_tmp); result = STRCAT(result,"(*");
    strFree(str_tmp);
  }

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenCppFunc(TForm tf, String name, Class *cl, int tab) {
  TForm tmp;
  String result, typeToAddToName = NULL;
  String str_tmp;
  int flagIsFnPtr = theFALSE;

  if (!tfIsMap(tf))           return NULL;
  if (HasDependentTypes(tf))  return NULL;
  if (ToSkip(tf))             return NULL;

  result = InitStr();

  result = sprintTab(result,tab);

  /* Return */
  tmp = tfMapRet(tf);
  flagIsFnPtr = theFALSE;
  if (tfIsExit(tmp)) { strFree(result); return NULL; }
  else if (tfIsGenerator(tmp)) { strFree(result); return NULL; }
  else if (tfIsMulti(tmp)) {
    result = STRCAT(result,"void ");
    if (discriminationReturnType) {
      typeToAddToName = InitStr();
      typeToAddToName = STRCAT(typeToAddToName,"void");
    }
  }
  else {
    /* There will be no discrimation on the return type, if the return type
       is a function pointer */
    if (tfIsFuncPtr(tmp)) {
      flagIsFnPtr = theTRUE;
      str_tmp = GenCppFunc_RetFnPtr(tmp);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,str_tmp);
    }
    else {
      str_tmp = AbSynToCpp(tfGetExpr(tmp));
      if (!str_tmp) { strFree(result); return NULL; }
      if (discriminationReturnType) {
	typeToAddToName = InitStr();
	typeToAddToName = STRCAT(typeToAddToName,str_tmp);
      }
      result = STRCAT(result,str_tmp); result = STRCAT(result," ");
      if (AbSynIsUserDef(tfGetExpr(tmp))) {
	result = STRCAT(result,"*");
	if (discriminationReturnType) typeToAddToName = STRCAT(typeToAddToName,"*");
      }
      strFree(str_tmp);
    }
  }

  /* Function name */
  str_tmp = createFnIdForCPP(name,NULL);
  if (!str_tmp) result = STRCAT(result,name);
  else { result = STRCAT(result,name); strFree(str_tmp); }

  if (discriminationReturnType) {
    String ToAdd = transOperatorLike(typeToAddToName);
    result = STRCAT(result,"_");
    result = STRCAT(result,ToAdd);
    strFree(typeToAddToName);
    strFree(ToAdd);
  }

  /* Params */
  str_tmp = GenCppParams(tf,NULL,BL_WithType,BL_WithName,BL_IsStatic,!BL_Abstract,!BL_Export);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp); 
  strFree(str_tmp); 

  /* Parameters of the return type if the return type is a function pointer */
  if (flagIsFnPtr) {
    str_tmp = GenCppFunc_ParamsRetFnPtr(tmp,NULL,BL_IsStatic,!BL_Abstract,!BL_Export);
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  }

  /* Body */
  str_tmp = GenCppBodyForMethods(tf,name,NULL);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);
  
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

void GenFunction(AbSyn ab, FILE *as, FILE *cc) {
  String toOutputAS, toOutputCC;
  String name;
  TForm tf;

  /* --- Init --- */
  tf = abTUnique(abDefineLhs(ab));
  name = abIdStr(abDefineLhs(abDeclareId(ab)));
  CurrentClassAldor = "Global";
  CurrentClassCpp = "Global";

  /* --- Aldor --- */
  toOutputAS = GenAldorStubs(tf,name,NULL,BL_Global,TabZero);

  /* --- C++ --- */
  toOutputCC = GenCppFunc(tf,name,NULL,TabZero);

  CurrentClassAldor = NULL;
  CurrentClassCpp = NULL;

  if (toOutputAS && toOutputCC) { 
    fprintf(as,"%s",toOutputAS); 
    fprintf(cc,"%s",toOutputCC); 
    strFree(toOutputAS);
    strFree(toOutputCC);
    globalFuncCounter++;
  } else {
    if (toOutputAS) strFree(toOutputAS);
    if (toOutputCC) strFree(toOutputCC);
  }
}

/* ---------- END GLOBAL FUNCTIONS ---------- */

/* ---------- HEADER ---------- */

void GenCppConstructors(Class *cl, int tab, FILE *cc) {
  printTab(cc,tab);
  fprintf(cc,"%s() { ptr = 0; }\n",cl->id.basic);
  printTab(cc,tab);
  fprintf(cc,"%s(%s",cl->id.basic,cl->id.typeCpp);
  fprintf(cc," *orig) { ptr = orig; }\n");
  printTab(cc,tab);
  fprintf(cc,"%s(void *orig) { ptr = reinterpret_cast<%s",cl->id.basic,cl->id.typeCpp);
  fprintf(cc," *>(orig); }\n");
}

void GenCppDestructor(String name, int tab, FILE *cc) {
  printTab(cc,tab);
  fprintf(cc,"virtual ~%s() {}\n",name);
}

void GenCppAccessToRealObj(Class *cl, int tab, FILE *cc) {
  printTab(cc,tab);
  fprintf(cc,"friend %s",cl->id.typeCpp);
  fprintf(cc," *realObject(%s",cl->id.typeCpp);
  fprintf(cc," *ob) { return ob->ptr; }\n");
}

void GenCppGiveType(Class *cl, int tab, FILE *cc) {
  printTab(cc,tab);
  fprintf(cc,"static void *givetype() { return ");
  fprintf(cc,"ALDOR_givetype_%s(",cl->id.export);
  if (cl->params) {
    int i;
    TForm tmp = cl->params;
    int paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
    if (paramNb == 1) {
      if (tfIsDeclare(tmp))
	fprintf(cc,"%s::givetype()",symString(tfDeclareId(tmp)));
    } 
    else 
      for (i=0;i<paramNb;i++) {
	TForm tmp2 = tfMultiArgN(tmp,i);
	if (tfIsDeclare(tmp2))
	  fprintf(cc,"%s::givetype()",symString(tfDeclareId(tmp2)));
	if (i < paramNb-1) fprintf(cc,",");
      }
  }
  fprintf(cc,"); }\n");
}

void GenHeaderForClass(Class *cl, FILE *cc) {
  TForm tmp;
  int paramNb;
  int i;

  /* template */
  if (cl->params) {
    fprintf(cc,"template <");
    tmp = cl->params;
    paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
    if (paramNb == 1) {
      if (tfIsDeclare(tmp)) 
	fprintf(cc,"class %s",symString(tfDeclareId(tmp)));
    }
    else 
      for (i=0;i<paramNb;i++) {
	TForm tmp2 = tfMultiArgN(tmp,i);
	if (tfIsDeclare(tmp2)) 
	  fprintf(cc,"class %s",symString(tfDeclareId(tmp2)));
	if (i < paramNb-1) fprintf(cc,",");
      }
    fprintf(cc,">\n");
  }

  /* class */
  fprintf(cc,"class %s",cl->id.basic);
  if (NotEmpty(cl->parents)) {
    fprintf(cc,": ");
    Rewind(cl->parents);
    while (!EOList(cl->parents)) {
      Ident *id = (Ident *)GetItem(cl->parents);
      fprintf(cc," virtual public %s",id->typeCpp);
      GotoNext(cl->parents);
      if (!EOList(cl->parents)) fprintf(cc,",");
    }
  }
 
  /* standard members */
  fprintf(cc," {\n");
  fprintf(cc,"\t%s *ptr;\n",cl->id.typeCpp);
  fprintf(cc,"public:\n");
  GenCppConstructors(cl,1,cc);
  GenCppDestructor(cl->id.basic,1,cc);
  GenCppAccessToRealObj(cl,1,cc);
  GenCppGiveType(cl,1,cc);
}

/* ---------- END HEADER ---------- */

/* ---------- CLASS ---------- */

void GenExtraClass(Class *cl, Class *extra, FILE *cc) {
  String str_tmp;
  String saveCurrent;
  SymeList meths;
  Syme meth;
 
  saveCurrent = CurrentClassCpp;
  CurrentClassCpp = "PercentType";

  /* parameters of the class */
  str_tmp = GCFC_HeaderTmpl(cl);
  if (!str_tmp) return; 
  fprintf(cc,"%s",str_tmp);
  strFree(str_tmp);

  /* header */
  fprintf(cc,"class %s",extra->id.basic);

  /* inheritance */
  str_tmp = GCFC_HeaderInher(extra);
  if (!str_tmp) return; 
  fprintf(cc,"%s",str_tmp);
  strFree(str_tmp);

  fprintf(cc," {\n");
  fprintf(cc,"public:\n");
  fprintf(cc,"\tvirtual ~%s() {}\n",extra->id.basic);
  
  /* methods */  

  if (!extra->methods) { 
    fprintf(cc,"};\n"); 
    CurrentClassCpp = saveCurrent; 
    return; 
  }

  extra->methCounter = 0;
  meths = tfThdExports(extra->methods) ? tfThdExports(extra->methods):tfParents(extra->methods);
  for (; meths; meths = meths->rest) {
    meth = meths->first;

    str_tmp = GCFC_Methods(extra,meth);
    if (!str_tmp) continue;
    fprintf(cc,"%s",str_tmp);
    strFree(str_tmp);

    extra->methCounter++;
  } 

  fprintf(cc,"};\n");
  CurrentClassCpp = saveCurrent;
}


void GenClassesForDomains(AbSyn ab, int isTmpl, FILE *as, FILE *cc) {
  
  /*
    Class generation
    ===================
    Creation of an intermediate "Class" structure
    Code generation
    Intermediate struct is freed
  */


  /*
    Regular types:
    -------------
    A: ACat with { .... } == add { ... }
    gets to
    class CategoryA: virtual public ACat {
    public:
    ....
    };
    class A: virtual public CategoryA {
    public:
    ...
    };
     
    Parameterized types:
    -------------------
    A(T: TCat): ACat(T) with { .... } == add { ... }
    gets to
    template <class T>
    class CategoryA: virtual public ACat {
    public:
    ....
    };
    template <class T>
    class A: virtual public CategoryA<T> {
    public:
    ...
    };
  */


  Class  *cl;
  SymeList meths;
  String str_tmp;

  cl =  BuildClass(ab,isTmpl);
  if (!cl) return;
  
  /* ----- Generation ------ */

  /* /// ALDOR stubs for methods /// */
  if (cl->methods) {
    cl->methCounter = 0;
    meths = tfThdExports(cl->methods) ? tfThdExports(cl->methods):tfParents(cl->methods);
    for (; meths; meths = meths->rest) {
      str_tmp = GenAldorStubs(symeType(meths->first),symString(symeId(meths->first)),cl,!BL_Global,TabZero);
      if (str_tmp) { fprintf(as,"%s",str_tmp); strFree(str_tmp); cl->methCounter++; }
    }
  }

  /* /// C++ classes /// */

  /* Extra Class Generation */

  if (NotEmpty(cl->extraClasses)) {
    for (Rewind(cl->extraClasses); !EOList(cl->extraClasses); GotoNext(cl->extraClasses))
      GenExtraClass(cl,(Class *)GetItem(cl->extraClasses),cc);
  }

  /* Header */
  GenHeaderForClass(cl,cc);

  /* methods */
  if (cl->methods)
    GenMethodsForClass(cl,cc);

  /* End of the class */
  fprintf(cc,"};\n");

  /* Free the memory */
  CurrentClassAldor = NULL;
  CurrentClassCpp = NULL;
  FreeClass(cl);
}


/*
   
   ----------------------------------------------------
   ----------- GENERATION ABS.CLASS/CLASS ------------- 
   ----------------------------------------------------
   
   */

String GCFC_HeaderTmpl(Class *cl) {
  String str_tmp, result = InitStr();
  TForm tmp = cl->params;
  int paramNb;

  /* To replace % in the declaration ... */
  result = STRCAT(result,"template <class PercentType"); 
  
  /* For parameterized types */
  if (tmp) {
    result = STRCAT(result,",");
    paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
    if (paramNb == 1) {
      if (tfIsDeclare(tmp)) {
	result = STRCAT(result,"class ");
	result = STRCAT(result,symString(tfDeclareId(tmp)));
      }
    }
    else {
      int i;
      for (i=0;i<paramNb;i++) {
	TForm tmp2 = tfMultiArgN(tmp,i);
	if (tfIsDeclare(tmp2)) {
	  result = STRCAT(result,"class ");
	  result = STRCAT(result,symString(tfDeclareId(tmp2)));
	}
	if (i < paramNb-1) result = STRCAT(result,",");
      }
    }
  }

  /* end of template part */
  result = STRCAT(result,">\n");

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GCFC_HeaderInher(Class *cl) {
  MyList *parents = cl->parents;
  String str_tmp, result = InitStr();

  if (NotEmpty(parents)) {
    result = STRCAT(result,": ");
    Rewind(parents);
    while (!EOList(parents)) {
      Parent *par = (Parent *)GetItem(parents);
      result = STRCAT(result," virtual public ");
      result = STRCAT(result,par->id.typeCpp);
      GotoNext(parents);
      if (!EOList(cl->parents)) result = STRCAT(result,",");
    }
  }

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GCFC_Methods(Class *cl, Syme method) {
  String str_tmp, result;
  int flagIsFnPtr = theFALSE;
  String typeToAddToName = NULL;
  int isStatic = theFALSE;
  TForm tmp;

  /* Check if method has to be output */
  if (!method) return NULL;
  if (strEqual(symString(symeId(method)),"%%")) return NULL;
  if (HasDependentTypes(symeType(method)))      return NULL;
  if (ToSkip(symeType(method)))                 return NULL;
  if (!tfIsMap(symeType(method)))               return NULL;
  
  /* Static or not ? */
  isStatic = StaticOrNot(tfMapArg(symeType(method)),cl);
  if (isStatic) return NULL;
  
  /* Return type */
  tmp = tfMapRet(symeType(method));

  if (tfIsExit(tmp))      return NULL;
  if (tfIsGenerator(tmp)) return NULL;

  result = InitStr();
  flagIsFnPtr = theFALSE;

  if (tfIsMulti(tmp)) {
    result = STRCAT(result,"\tvirtual void ");
    if (discriminationReturnType) {
      typeToAddToName = InitStr();
      typeToAddToName = STRCAT(typeToAddToName,"void");
    }
  }
  else if (tfIsFuncPtr(tmp)) {
    flagIsFnPtr = theTRUE;
    str_tmp = GenCppFunc_RetFnPtr(tmp);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,"\tvirtual "); 
    result = STRCAT(result,str_tmp);
  } 
  else {
    str_tmp = AbSynToCpp(tfGetExpr(tmp));
    if (!str_tmp) { strFree(result); return NULL; }
    if (discriminationReturnType) {
      typeToAddToName = InitStr();
      typeToAddToName = STRCAT(typeToAddToName,str_tmp);
    }
    result = STRCAT(result,"\tvirtual ");
    result = STRCAT(result,str_tmp);
    result = STRCAT(result," ");
    if (AbSynIsUserDef(tfGetExpr(tmp))) {
      result = STRCAT(result,"*");
      if (discriminationReturnType) typeToAddToName = STRCAT(typeToAddToName,"*");
    }
    strFree(str_tmp);
  }
  
  /* Function name */
  str_tmp = createFnIdForCPP(symString(symeId(method)),cl);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result," "); result = STRCAT(result,str_tmp);
  strFree(str_tmp);
  
  if (discriminationReturnType) {
    String ToAdd = transOperatorLike(typeToAddToName);
    result = STRCAT(result,"_");
    result = STRCAT(result,ToAdd);
    strFree(typeToAddToName);
    strFree(ToAdd);
  }
  
  /* Parameters */
  str_tmp = GenCppParams(symeType(method),cl,BL_WithType,!BL_WithName,isStatic,BL_Abstract,!BL_Export);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);
  
  /* Parameters of the return type if the return type is a function pointer */
  if (flagIsFnPtr) {
    str_tmp = GenCppFunc_ParamsRetFnPtr(tmp,cl,BL_IsStatic,BL_Abstract,!BL_Export);
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  }
  result = STRCAT(result," = 0;\n");

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

void GenClassForCategories(AbSyn ab, int isTmpl, FILE *as, FILE *cc) {
  
  /*
    Class generation
    ===================
    Creation of an intermediate "Class" structure
    Code generation
    Intermediate struct is freed
  */

  Class  *cl;
  SymeList meths;
  Syme meth;
  String str_tmp;

  /* ----- Creation of struct ----- */

  cl = BuildAbstractClass(ab,isTmpl);
  if (!cl) return;

  /* There should be no aldor code generated because, there is no function
     call and so no stub needed */

  /* C++ class generation */

  /* ---- HEADER */

  /* Header params */
  str_tmp = GCFC_HeaderTmpl(cl);
  if (!str_tmp) { 
    FreeClass(cl); 
    CurrentClassAldor = NULL; 
    CurrentClassCpp = NULL; 
    return; 
  }
  fprintf(cc,"%s",str_tmp);
  strFree(str_tmp);

  /* Header name of class */
  fprintf(cc,"class %s",cl->id.basic);

  /* Header inheritance */
  str_tmp = GCFC_HeaderInher(cl);
  if (!str_tmp) { 
    FreeClass(cl); 
    CurrentClassAldor = NULL; 
    CurrentClassCpp = NULL; 
    return; 
  }
  fprintf(cc,"%s",str_tmp);
  strFree(str_tmp);

  /* Header standard */
  fprintf(cc," {\n");
  fprintf(cc,"public:\n");
  fprintf(cc,"\tvirtual ~%s() {}\n",cl->id.basic);

  /* ---- METHODS */

  /* no method ? */
  if (!cl->methods) { 
    fprintf(cc,"};\n"); 
    CurrentClassAldor = NULL; 
    CurrentClassCpp = NULL; 
    FreeClass(cl); 
    return;
  }

  /* output methods */
  cl->methCounter = 0;
  meths = tfThdExports(cl->methods) ? tfThdExports(cl->methods):tfParents(cl->methods);
  for (; meths; meths = meths->rest) {
    meth = meths->first;
    
    str_tmp = GCFC_Methods(cl,meth);
    if (!str_tmp) continue;
    fprintf(cc,"%s",str_tmp);
    strFree(str_tmp);

    cl->methCounter++;
  } 

  /* the end */
  fprintf(cc,"};\n");
  CurrentClassAldor = NULL;
  CurrentClassCpp = NULL;
  FreeClass(cl);
}

/* ---------- END CLASS ---------- */

/* ---------- EXPORT / IMPORT ---------- */

void GenExportClass(AbSyn ab, int isTmpl, FILE *as, FILE *cc) {

  Class *cl;
  SymeList l;
  Syme item;
  String toOutputAS, toOutputCC;

  cl = BuildClass(ab,isTmpl);
  if (!cl) return;

  /* tfTag(cl->methods) => TF_Third */
  /* two cases: cl->methods->parents or cl->methods->thdExports */
  
  if (!(cl->methods)) l = NULL;
  else l = tfThdExports(cl->methods) ? tfThdExports(cl->methods):tfParents(cl->methods);
  
  for (; l; l = l->rest) {

    item = l->first;
    if (strEqual(symString(symeId(item)),"%%")) continue;
    
    /* --- ALDOR Export --- */
    toOutputAS = GenAldorFnSig(symeType(item),symString(symeId(item)),
			       cl,1,BL_Exported,!BL_UsePercent,BL_UseTmplParms);
    
    /* --- C++ extern --- */
    if (toOutputAS) 
      toOutputCC = GenCppFnSig(symeType(item),symString(symeId(item)),cl,1,BL_Exported);
    else
      toOutputCC = 0;
    
    if (toOutputAS && toOutputCC) {
      fprintf(as,"%s",toOutputAS); 
      fprintf(cc,"%s",toOutputCC); 
      strFree(toOutputAS);
      strFree(toOutputCC);
      cl->methCounter++;
    }
  }
  
  /* "givetype" function for the class */
  GenGiveTypeForExport(cl,as,cc);
  
  /* Memory stuff */
  FreeClass(cl);
}


void GenExportFunction(AbSyn ab, FILE *as, FILE *cc) {
  String toOutputAS, toOutputCC;
  TForm tf;
  String name;

  /* --- Init --- */
  tf = abTUnique(abDefineLhs(ab));
  name = abIdStr(abDefineLhs(abDeclareId(ab)));

  /* --- ALDOR Export --- */
  toOutputAS = GenAldorFnSig(tf,name,NULL,1,BL_Exported,!BL_UsePercent,BL_UseTmplParms);

  /* --- C++ extern --- */  
  toOutputCC = GenCppFnSig(tf,name,NULL,1,BL_Exported);

  if (toOutputAS && toOutputCC) { 
    fprintf(as,"%s",toOutputAS); 
    fprintf(cc,"%s",toOutputCC); 
    strFree(toOutputAS);
    strFree(toOutputCC);
    globalFuncCounter++;
  } else {
    if (toOutputAS) strFree(toOutputAS);
    if (toOutputCC) strFree(toOutputCC);
  } 
}

/* ---------- END EXPORT / IMPORT ---------- */

/* ---------- METHODS ---------- */

String GenCppBodyForMethods(TForm tf, String name, Class *cl) {
  int isStatic;
  int returnIsUsrDef;		/* if the return type is a user defined type then we need a 'new' */
  int returnIsFnPtr;
  AbSyn ab;
  String result,str_tmp;

  if (!tfIsMap(tf)) return NULL;

  result = InitStr();

  /* Static or not ? */
  if (!cl) isStatic = theTRUE;
  else isStatic = StaticOrNot(tfMapArg(tf),cl);

  /* body */
  ab = tfGetExpr(tfMapRet(tf));
  returnIsUsrDef = AbSynIsUserDef(ab);
  returnIsFnPtr = tfIsFuncPtr(tfMapRet(tf));
  result = STRCAT(result," { ");

  /* -- if multiple return, declare appropriate variables */
  if (tfIsMultiReturn(tfMapRet(tf))) {
    str_tmp = GenCppMultiRetLocalDecl(tf,BL_WithType);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,"\n");
    result = STRCAT(result,str_tmp);
    result = STRCAT(result,"\n");
  }

  /* -- call the stub */
  if (!AbSynIsVoid(ab) && !tfIsMultiReturn(tfMapRet(tf))) result = STRCAT(result,"return ");

  if (returnIsUsrDef) {
    str_tmp = AbSynToCpp(ab);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,"new "); result = STRCAT(result,str_tmp); result = STRCAT(result,"(");
    strFree(str_tmp);
  }
  else if (returnIsFnPtr) {
    str_tmp = outputCppFnPtrParms(tfMapRet(tf),cl,theTRUE,theFALSE,!BL_UsePercent,!BL_Export, int0);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,"(");
    result = STRCAT(result,str_tmp);
    result = STRCAT(result,")");
    strFree(str_tmp);
  }

  str_tmp = createFnIdExternC(name,cl);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);

  if (!cl)
    str_tmp = GenCppParams_Global_Body(tf,name);
  else
    str_tmp = GenCppParams_Body(tf,name,cl,isStatic);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);

  if (returnIsUsrDef) result = STRCAT(result,")");

  result = STRCAT(result,"; ");
  
  /* -- Multiple return: assign real variables */
  if (tfIsMultiReturn(tfMapRet(tf))) {
    str_tmp = GenCppMultiRetAssign(tf);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
  }

  result = sprintTab(result,1);
  result = STRCAT(result,"}\n");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenCppBodyStaticPercentForMethods(Syme meth, Class *cl, int posPercent, int export) {
  int returnIsUsrDef;		/* if the return type is a user defined type then we need a 'new' */
  AbSyn ab;
  String result,str_tmp;
  TForm tf = symeType(meth);
  String name = symString(symeId(meth));

  if (!tfIsMap(tf)) return NULL;

  result = InitStr();

  /* body */
  ab = tfGetExpr(tfMapRet(tf));
  returnIsUsrDef = AbSynIsUserDef(ab);
  result = STRCAT(result," { ");

  /* -- if multiple return, declare appropriate variables */
  if (tfIsMultiReturn(tfMapRet(tf))) {
    str_tmp = GenCppMultiRetLocalDecl(tf,BL_WithType);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,"\n");
    result = STRCAT(result,str_tmp);
    result = STRCAT(result,"\n");
  }

  /* -- call the stub */
  if (!AbSynIsVoid(ab) && !tfIsMultiReturn(tfMapRet(tf))) result = STRCAT(result,"return ");

  if (export && returnIsUsrDef) {
    str_tmp = AbSynToCpp(ab);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,"new "); result = STRCAT(result,str_tmp); result = STRCAT(result,"(");
    strFree(str_tmp);
  }

  /* -- object */
  if (export) {
    str_tmp = createFnIdExternC(name,cl);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  } else {
    String str_tmp2 = createFnIdForCPP(name,cl);
    str_tmp = itoa_(posPercent-1);
    result = STRCAT(result,"parm");
    result = STRCAT(result,str_tmp);
    result = STRCAT(result,"->");
    result = STRCAT(result,str_tmp2);
    strFree(str_tmp2);
    strFree(str_tmp);
  }

  if (export)
    str_tmp = GenCppParamsBody_VirtualForStatic(tf,cl,posPercent);
  else
    str_tmp = GenCppParamsStaticPercent(tf,cl,!BL_WithType,BL_WithName,posPercent,export);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);

  if (export && returnIsUsrDef) result = STRCAT(result,")");

  result = STRCAT(result,"; ");
  
  /* -- Assign real variables */
  if (tfIsMultiReturn(tfMapRet(tf))) {
    str_tmp = GenCppMultiRetAssign(tf);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
  }

  result = sprintTab(result,1);
  result = STRCAT(result,"}\n");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenTwisterFunction(Class *cl, Syme meth, int posPercent) {
  String result, str_tmp;
  TForm tmp;

  result = InitStr();
  
  /* generation of the extra "twister" function */
  result = STRCAT(result,"protected:\n\tvirtual ");
  
  /* Return type */
  tmp = tfMapRet(symeType(meth));
  if (tfIsMulti(tmp)) result = STRCAT(result,"void ");
  else {
    str_tmp = AbSynToCpp(tfGetExpr(tmp));
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    result = STRCAT(result," ");
    if (AbSynIsUserDef(tfGetExpr(tmp)))
      result = STRCAT(result," *");
    strFree(str_tmp);
  }
  
  /* Function name */
  str_tmp = createFnIdForCPP(symString(symeId(meth)),cl);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result," "); result = STRCAT(result,str_tmp);
  strFree(str_tmp);
  
  /* Parameters */
  str_tmp = GenCppParamsStaticPercent(symeType(meth),cl,BL_WithType,BL_WithName,posPercent,!BL_Exported);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);

  /* Body */
  str_tmp = GenCppBodyStaticPercentForMethods(meth,cl,posPercent,BL_Exported);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);

  /* the end */
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}


void GenMethodsForClass(Class *cl, FILE *cc) {
  String str_tmp;
  SymeList meths;
  Syme meth;
  TForm tmp;
  String result, typeToAddToName = NULL;
  int isStatic, posPercent;
  int flagIsFnPtr = theFALSE;

  if (!cl->methods) return;

  result = InitStr();

  cl->methCounter = 0;
  meths = tfThdExports(cl->methods) ? tfThdExports(cl->methods):tfParents(cl->methods);
  for (; meths; meths = meths->rest) {
    meth = meths->first;

    if (strEqual(symString(symeId(meth)),"%%")) { *result = '\0'; continue; }
    if (HasDependentTypes(symeType(meth))) { *result = '\0'; continue; }
    if (ToSkip(symeType(meth))) { *result = '\0'; continue; }
    if (!tfIsMap(symeType(meth))) { *result = '\0'; continue; }

    /* Static or not ? */
    isStatic = StaticOrNot(tfMapArg(symeType(meth)),cl);
    posPercent = PercentsInParms(tfMapArg(symeType(meth)),cl);

    /* Static or virtual */
    result = STRCAT(result, isStatic ? "\tstatic " : "\tvirtual ");

    /* Return type */
    tmp = tfMapRet(symeType(meth));
    flagIsFnPtr = theFALSE;
    if (tfIsExit(tmp)) { *result = '\0'; continue; }
    else if (tfIsGenerator(tmp)) { *result = '\0'; continue; }
    else if (tfIsMulti(tmp)) {
      result = STRCAT(result,"void ");
      if (discriminationReturnType) {
	typeToAddToName = InitStr();
	typeToAddToName = STRCAT(typeToAddToName,"void");
      }
    }
    else {
      if (tfIsFuncPtr(tmp)) {
	flagIsFnPtr = theTRUE;
	str_tmp = GenCppFunc_RetFnPtr(tmp);
	if (!str_tmp) { *result = '\0'; continue; }
	result = STRCAT(result,str_tmp);
      } else {
	str_tmp = AbSynToCpp(tfGetExpr(tmp));
	if (!str_tmp) { *result = '\0'; continue; }
	if (discriminationReturnType) {
	  typeToAddToName = InitStr();
	  typeToAddToName = STRCAT(typeToAddToName,str_tmp);
	}
	result = STRCAT(result,str_tmp);
	result = STRCAT(result," ");
	if (AbSynIsUserDef(tfGetExpr(tmp))) {
	  result = STRCAT(result,"*");
	  if (discriminationReturnType) typeToAddToName = STRCAT(typeToAddToName,"*");
	}
	strFree(str_tmp);
      }
    }

    /* Function name */
    str_tmp = createFnIdForCPP(symString(symeId(meth)),cl);
    if (!str_tmp) { *result = '\0'; continue; }
    result = STRCAT(result," "); result = STRCAT(result,str_tmp);
    strFree(str_tmp);

    if (discriminationReturnType) {
      String ToAdd = transOperatorLike(typeToAddToName);
      result = STRCAT(result,"_");
      result = STRCAT(result,ToAdd);
      strFree(typeToAddToName);
      strFree(ToAdd);
    }

    /* Parameters */
    str_tmp = GenCppParams(symeType(meth),cl,BL_WithType,BL_WithName,isStatic,BL_Abstract,!BL_Export);
    if (!str_tmp) { *result = '\0'; continue; }
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);

    /* Parameters of the return type if the return type is a function pointer */
    if (flagIsFnPtr) {
      str_tmp = GenCppFunc_ParamsRetFnPtr(tmp,NULL,BL_IsStatic,!BL_Abstract,!BL_Export);
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
    }

    /* Body */
    if (!isStatic || !posPercent) {
      str_tmp = GenCppBodyForMethods(symeType(meth),symString(symeId(meth)),cl);
      if (!str_tmp) { *result = '\0'; continue; }
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
    } else {
      /* body for static function with a % at position posPercent */
      str_tmp = GenCppBodyStaticPercentForMethods(meth,cl,posPercent,!BL_Exported);
      if (!str_tmp) { *result = '\0'; continue; }
      result = STRCAT(result,str_tmp);
      result = STRCAT(result,"\n");
      strFree(str_tmp);

      /* twister function */
      str_tmp = GenTwisterFunction(cl,meth,posPercent);
      if (!str_tmp) { *result = '\0'; continue; }
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      result = STRCAT(result,"\npublic:\n");
    }

    fprintf(cc,"%s",result);
    *result = '\0';
    cl->methCounter++;
  }
  strFree(result);
}

/* ---------- END METHODS ---------- */

/* ---------- MULTIPLE RETURN ---------- */

String GenCppMultiRetLocalDecl(TForm tf, int withType) {
  int i;
  AbSyn tmp;
  String str_tmp,str_tmp2;
  String result;
  int cnt = 0;
  
  if (!tfIsMultiReturn(tfMapRet(tf))) return NULL;

  result = InitStr();

  tf = tfMapRet(tf);

  for (i=0; i < tfMultiArgc(tf); i++) {

    tmp = tfGetExpr(tfMultiArgN(tf,i));

    switch(abTag(tmp)) {
    case AB_Declare:
      if (!abHasTag(abDeclareType(tmp),AB_Id)) { strFree(result); return NULL; }
      str_tmp = RealTypeString(abDeclareType(tmp),!BL_Export);
      if (!strEqual(str_tmp,"void *")) str_tmp[strlen(str_tmp) - 1] = '\0';
      if (!str_tmp) { strFree(result); return NULL; }
      str_tmp2 = itoa_(cnt);
      if (!withType) {
	result = STRCAT(result,"&local");
	result = STRCAT(result,str_tmp2);
      } else {
	result = STRCAT(result,str_tmp);
	result = STRCAT(result," local");
	result = STRCAT(result,str_tmp2);
      }
      strFree(str_tmp2);
      cnt++;
      strFree(str_tmp);
      break;
    case AB_Id:
      str_tmp = RealTypeString(tmp,!BL_Export);
      if (!strEqual(str_tmp,"void *")) str_tmp[strlen(str_tmp) - 1] = '\0';
      if (!str_tmp) { strFree(result); return NULL; }
      str_tmp2 = itoa_(cnt);
      if (!withType) {
	result = STRCAT(result,"&local");
	result = STRCAT(result,str_tmp2);
      } else {
	result = STRCAT(result,str_tmp);
	result = STRCAT(result," local");
	result = STRCAT(result,str_tmp2);
      }
      strFree(str_tmp2);
      cnt++;
      strFree(str_tmp);
      break;
    case AB_Apply:
      str_tmp2 = itoa_(cnt);
      if (!withType) {
	result = STRCAT(result,"&local");
	result = STRCAT(result,str_tmp2);
      } else {
	result = STRCAT(result,"void *local");
	result = STRCAT(result,str_tmp2);
      }
      strFree(str_tmp2);
      cnt++;
      break;
    default:
      fprintf(stderr,"GenCppMultiRetLocalDecl - Wrong tag - %d\n",abTag(tmp));
      strFree(result); return NULL;
      break;
    }
    if (!withType && (i < tfMultiArgc(tf) - 1)) result = STRCAT(result,",");
    else if (withType) result = STRCAT(result,";");
  }

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenCppMultiRetAssign(TForm tf) {
  int i, isUsrDef;
  AbSyn tmp;
  String str_tmp,str_tmp2,str_tmp3;
  String result;
  int cnt = 0;
    
  
  if (!tfIsMultiReturn(tfMapRet(tf))) return NULL;
  
  result = InitStr();

  result = STRCAT(result,"\n");
  
  cnt = tfMapArgc(tf) - 1;	/* we give numbers to the params by beginning with 0 */
     
  tf = tfMapRet(tf);
  
  for (i=0; i < tfMultiArgc(tf); i++,cnt++) {

    tmp = tfGetExpr(tfMultiArgN(tf,i));
    str_tmp3 = itoa_(i);    
    switch(abTag(tmp)) {
    case AB_Declare:
      if (!abHasTag(abDeclareType(tmp),AB_Id)) { strFree(result); return NULL; }
      isUsrDef = AbSynIsUserDef(abDeclareType(tmp));
      str_tmp2 = itoa_(cnt);
      if (!isUsrDef) {
	result = STRCAT(result,"*parm");
	result = STRCAT(result,str_tmp2);
	result = STRCAT(result," = local");
	result = STRCAT(result,str_tmp3);
	result = STRCAT(result,"; ");
      } else {      
	str_tmp = AbSynToCpp(abDeclareType(tmp));
	if (!str_tmp) { strFree(result); return NULL; }
	result = STRCAT(result,"*parm");
	result = STRCAT(result,str_tmp2);
	result = STRCAT(result," = new ");
	result = STRCAT(result,str_tmp);
	result = STRCAT(result,"(local");
	result = STRCAT(result,str_tmp3);
	result = STRCAT(result,"); ");
	strFree(str_tmp);
      }
      strFree(str_tmp2);
      strFree(str_tmp3);
      break;
    case AB_Id:
      isUsrDef = AbSynIsUserDef(tmp);
      str_tmp2 = itoa_(cnt);
      if (!isUsrDef) {
	result = STRCAT(result,"*parm");
	result = STRCAT(result,str_tmp2);
	result = STRCAT(result," = local");
	result = STRCAT(result,str_tmp3);
	result = STRCAT(result,"; ");
      } else {      
	str_tmp = AbSynToCpp(tmp);
	if (!str_tmp) { strFree(result); return NULL; }
	result = STRCAT(result,"*parm");
	result = STRCAT(result,str_tmp2);
	result = STRCAT(result," = new ");
	result = STRCAT(result,str_tmp);
	result = STRCAT(result,"(local");
	result = STRCAT(result,str_tmp3);
	result = STRCAT(result,"); ");
	strFree(str_tmp);
      }
      strFree(str_tmp2);
      strFree(str_tmp3);
      break;
    case AB_Apply:
      fprintf(stderr,"Function pointers in a multiple return value are not handled ...\n");
      strFree(result); return NULL;
      break;
    default:
      fprintf(stderr,"GenCppMultiRetAssign - Wrong tag - %d\n",abTag(tmp));
      strFree(result); return NULL;
      break;
    }
  }

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}


String GenCppMultiRet(TForm tf, int withType, int withName, int cnt, int export) {
  int i;
  AbSyn tmp;
  String str_tmp,str_tmp2,str_tmp3;
  String result;
  
  result = InitStr();

  tf = tfMapRet(tf);

  for (i=0; i < tfMultiArgc(tf); i++) {

    tmp = tfGetExpr(tfMultiArgN(tf,i));
    str_tmp2 = itoa_(cnt);
    str_tmp3 = itoa_(i);
    switch(abTag(tmp)) {
    case AB_Declare:
      if (!abHasTag(abDeclareType(tmp),AB_Id)) { strFree(result); return NULL; }
      str_tmp = RealTypeString(abDeclareType(tmp),export);
      if (!str_tmp) { strFree(result); return NULL; }

      if (withName && withType) {
	result = STRCAT(result,str_tmp);
	result = STRCAT(result," parm");
	result = STRCAT(result,str_tmp2);
	strFree(str_tmp2);
	cnt++;
      } else if (withName) {
	result = STRCAT(result,"parm");
	result = STRCAT(result,str_tmp2);
	strFree(str_tmp2);
	cnt++;
      } else if (withType)
	result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      break;
    case AB_Id:
      str_tmp = RealTypeString(tmp,export);
      if (!str_tmp) { strFree(result); return NULL; }

      if (withName && withType) {
	result = STRCAT(result,str_tmp);
	result = STRCAT(result," parm");
	result = STRCAT(result,str_tmp2);
	strFree(str_tmp2);
	cnt++;
      } else if (withName) {
	result = STRCAT(result,"parm");
	result = STRCAT(result,str_tmp2);
	strFree(str_tmp2);
	cnt++;
      } else if (withType)
	result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      break;
    case AB_Apply:

      if (withName && withType) {
	result = STRCAT(result,"void *parm");
	result = STRCAT(result,str_tmp2);
	strFree(str_tmp2);
	cnt++;
      } else if (withName) {
	result = STRCAT(result,"parm");
	result = STRCAT(result,str_tmp2);
	strFree(str_tmp2);
	cnt++;
      } else if (withType)
	result = STRCAT(result,"void *");
      break;
    default:
      fprintf(stderr,"GenCppMultiRet - Wrong tag - %d\n",abTag(tmp));
      strFree(result); return NULL;
      break;
    }
    if (i < (tfMultiArgc(tf) - 1)) result = STRCAT(result,",");
  }
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

/* ---------- END MULTIPLE RETURN ---------- */

/* ---------- PARAMS HANDLING ---------- */

/* -- Common Code for Aldor -- */

String outputAldorTmplParms(TForm tf, int usePercent) {
  String result, str_tmp;

  /* check */
  if (!tfIsDeclare(tf)) return NULL;

  /* parameter */ 
  result = InitStr();
  result = STRCAT(result,symString(tfDeclareId(tf)));
  result = STRCAT(result,": ");
  str_tmp = TFormToAldor(tfDeclareType(tf),usePercent);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String outputAldorRegParms(TForm tf, int withType, int withName, int usePercent, String cnt, String result) {
  /* return: 0 - no problem / 1 - error */
  String str_tmp;

  /* Type -> String */
  if (tfIsGeneral(tf)) 
    str_tmp = TFormToAldor(tf,usePercent);
  else if (tfIsDeclare(tf)) 
    str_tmp = TFormToAldor(tfDeclareType(tf),usePercent);
  else { 
    strFree(result); 
    return NULL; 
  }

  /* output */
  if (!str_tmp) { strFree(result); return NULL; }

  if (withName && withType) { 
    result = STRCAT(result,"parm");
    result = STRCAT(result,cnt);
    result = STRCAT(result,": "); 
    result = STRCAT(result,str_tmp); 
  }
  else if (withName) {
    result = STRCAT(result,"parm");
    result = STRCAT(result,cnt);
  }
  else if (withType) 
    result = STRCAT(result,str_tmp);

  strFree(str_tmp);
  return result;
}

/* -- End Common Code for Aldor -- */

/* -- Aldor Params -- */

String GenAldorParams(TForm tf, Class *cl, int withType, int withName, int inBody, int usePercent, int useTmplParms) {
  String str_tmp;
  String result,str_tmp2;
  int cnt;
  int paramNb;
  TForm tmp = NULL;
  TForm tmp2 = NULL;
  int i;

  result = InitStr();

  result = STRCAT(result,"(");

  /* Template params */
  if (cl && useTmplParms) {
    tmp = cl->params;
    if (tmp && !inBody) {
      int i;
      paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
      if (paramNb == 1)	{ /* TF_Declare */
	str_tmp = outputAldorTmplParms(tmp, usePercent);
	if (!str_tmp) { strFree(result); return NULL; }
	result = STRCAT(result, str_tmp);
	strFree(str_tmp);
      }
      else			/* TF_Multiple */
	for (i=0;i<paramNb;i++) {
	  tmp2 = tfMultiArgN(tmp,i);
	  str_tmp = outputAldorTmplParms(tmp2, usePercent);
	  if (!str_tmp) { strFree(result); return NULL; }
	  result = STRCAT(result, str_tmp);
	  strFree(str_tmp);
	  if (i < paramNb-1) result = STRCAT(result,",");
	}
    }
  }

  /* Regular params */
  tf = tfMapArg(tf);

  paramNb = tfIsMulti(tf) ? tfMultiArgc(tf) : 1;
  if (paramNb && useTmplParms && cl && tmp && !inBody) result = STRCAT(result,",");
  if (paramNb == 1) {		/* TF_Declare ou TF_General */
    result = outputAldorRegParms(tf,withType,withName,usePercent,"0",result);
    if (!result) return NULL;
  }
  else 
    for (i=0,cnt = 0; i < paramNb; i++, cnt++) {
      tmp = tfMultiArgN(tf,i);
      str_tmp2 = itoa_(cnt);
      result = outputAldorRegParms(tmp,withType,withName,usePercent,str_tmp2,result);
      strFree(str_tmp2);
      if (!result) return NULL;
      if (i < paramNb-1) result = STRCAT(result,",");
    }

  result = STRCAT(result,")");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenAldorOneParam(TForm tf, int withType, int withName, int cnt, int usePercent) {
  String result = NULL, str_tmp;

  if (tfIsGeneral(tf)) str_tmp = TFormToAldor(tf,usePercent);
  else if (tfIsDeclare(tf)) str_tmp = TFormToAldor(tfDeclareType(tf),usePercent);
  else return NULL;

  if (!str_tmp) return NULL;

  if (withName && withType) result = strPrintf("parm%d: %s",cnt,str_tmp);
  else if (withName) result = strPrintf("parm%d",cnt);
  else if (withType) result = strCopy(str_tmp);
  
  strFree(str_tmp);

  return result;
}

/* -- End Aldor Params -- */

/* -- Common Code for C++ -- */

String outputCppFnPtrParms(TForm tf, Class *cl, int withType, int withName, int usePercent, int export, String cnt) {
  String result, str_tmp;
  
  result = InitStr();

  if (withName && withType) {
    str_tmp = GenCppFunc_RetFnPtr(tf);
    result = STRCAT(result,str_tmp);
    result = STRCAT(result," parm");
    result = STRCAT(result,cnt);
    strFree(str_tmp);
    str_tmp = GenCppFunc_ParamsRetFnPtr(tf,cl,!usePercent,!BL_Abstract, export); 
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  }
  else if (withName){
    result = STRCAT(result,"parm");
    result = STRCAT(result,cnt);
  } 
  else if (withType) {
    str_tmp = GenCppFunc_RetFnPtr(tf);
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
    str_tmp = GenCppFunc_ParamsRetFnPtr(tf,cl,!usePercent,!BL_Abstract, export);
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  } 

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}


String outputCppRegParms(TForm tf, Class *cl, int withType, int withName, int usePercent, int export, String cnt, String result) {
  /* return: 0 - no problem / 1 - error */
  String str_tmp, str_tmp2;
  int isUsrDef;
  int flagFnPtr = theFALSE;

  if (tfIsGeneral(tf)) {
    if (tfIsFuncPtr(tf)) {
      flagFnPtr = theTRUE;
      str_tmp2 = outputCppFnPtrParms(tf, cl, withType, withName, usePercent, export, cnt);
    }
    else
      str_tmp2 = AbSynToCpp(tfGetExpr(tf));
    isUsrDef = AbSynIsUserDef(tfGetExpr(tf));
  } else if (tfIsDeclare(tf)) {
    if (tfIsFuncPtr(tfDeclareType(tf))) {
      flagFnPtr = theTRUE;
      str_tmp2 = outputCppFnPtrParms(tfDeclareType(tf), cl, withType, withName, usePercent, export, cnt);
    }
    else
      str_tmp2 = AbSynToCpp(tfGetExpr(tfDeclareType(tf)));
    isUsrDef = AbSynIsUserDef(tfGetExpr(tfDeclareType(tf)));
  } else { 
    strFree(result); 
    return NULL; 
  }
  
  if (!str_tmp2) { strFree(result); return NULL; }
  
  if (flagFnPtr) 
    str_tmp = strCopy(str_tmp2);
  else {
    if (!isUsrDef) str_tmp = strCopy(str_tmp2);
    else str_tmp = export ? strCopy("void *") : strPrintf("%s *",str_tmp2);
  }

  strFree(str_tmp2);
  
  /* output param */
  if (!str_tmp) { strFree(result); return NULL; }

  if (flagFnPtr)
    result = STRCAT(result,str_tmp);
  else {
    if (withName && withType) {
      result = STRCAT(result,str_tmp);
      result = STRCAT(result," parm");
      result = STRCAT(result,cnt);
    }
    else if (withName){
      result = STRCAT(result,"parm");
      result = STRCAT(result,cnt);
    } else if (withType)
      result = STRCAT(result,str_tmp);
  }

  strFree(str_tmp);

  return result;
}

/* -- End Common Code for C++ -- */

/* -- C++ Params -- */

String GenCppParams(TForm tf, Class *cl, int withType, int withName, int isStatic, int abstract, int export) {

  String str_tmp, str_tmp2;
  String result;
  int cnt = 0;
  int paramNb,start;
  TForm tmp = NULL, tmp2;

  result = InitStr();

  result = STRCAT(result,"(");

  /* Template params */
  if (cl) {
    tmp = cl->params;
    if (tmp && export) {
      int i;
      paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
      for (i=0;i<paramNb;i++) {
	result = STRCAT(result,"void *");
	if (i < paramNb-1) result = STRCAT(result,",");
      }
    }
  }
    
  /* Regular params */
  tmp2 = tfMapArg(tf);
  paramNb = tfIsMulti(tmp2) ? tfMultiArgc(tmp2) : 1;
  start = 0;

  /* First parameter is special */
  if (paramNb) {
    /* If the function is a method (i.e not static) then if we are in class the first parameter is not output */
    if (!export && !isStatic) start++;

    /* If some parameters have been output before (for templates) then we need a comma */
    if (cl && tmp && export) result = STRCAT(result,",");
  }

  if ((paramNb == 1) && (!start)) {
    result = outputCppRegParms(tmp2,cl,withType,withName,!BL_UsePercent, export, "0", result);
    if (!result) return NULL;
  } else if (paramNb > 1) {
    for (cnt = 0; start < paramNb; start++,cnt++) {
      tmp = tfMultiArgN(tmp2,start);
      if (tfIsExit(tmp)) { strFree(result); return NULL; }
      else if (tfIsGenerator(tmp)) { strFree(result); return NULL; }
      str_tmp2 = itoa_(cnt);
      result = outputCppRegParms(tmp,cl,withType,withName,!BL_UsePercent, export, str_tmp2, result);
      strFree(str_tmp2);
      if (!result) return NULL;
      if (start < paramNb - 1) result = STRCAT(result,",");
    }
  }
  
  if (tfIsMultiReturn(tfMapRet(tf))) {
    if (paramNb) result = STRCAT(result,",");
    str_tmp = GenCppMultiRet(tf,withType,withName,cnt,export);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  }
  result = STRCAT(result,")");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenCppParamsBody_VirtualForStatic(TForm tf, Class *cl, int posPercent) {

  String str_tmp;
  String result;
  int isUsrDef;
  int paramNb,start;
  TForm tmp = NULL, tmp2;

  result = InitStr();

  result = STRCAT(result,"(");

  /* Template params */
  if (cl->params) {
    int i;
    tmp = cl->params;
    paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
    if (paramNb == 1) {
      if (tfIsDeclare(tmp)) { 
	result = STRCAT(result,symString(tfDeclareId(tmp)));
	result = STRCAT(result,"::givetype()");
      } else { fprintf(stderr,"Error in GenGivetype (tmpl params): %d\n",tfTag(tmp)); }
    } 
    else 
      for (i=0;i<paramNb;i++) {
	TForm tmp2 = tfMultiArgN(tmp,i);
	if (tfIsDeclare(tmp2)) { 
	  result = STRCAT(result,symString(tfDeclareId(tmp2)));
	  result = STRCAT(result,"::givetype()");
	} else { fprintf(stderr,"Error in GenGivetype (tmpl params): %d\n",tfTag(tmp)); }
	if (i < paramNb-1) result = STRCAT(result,",");
      }
  }

  /* Regular params */
  tmp = NULL;
  tmp2 = tfMapArg(tf);
  paramNb = tfIsMulti(tmp2) ? tfMultiArgc(tmp2) : 1;
    
  if (paramNb && (cl->params)) result = STRCAT(result,",");

  /* paramNb > 1 because we call this function for the body of the extra protected virtual method
     created if we have a function with a % not in first position */


  for (start = 0; start < paramNb; start++) {      
    
    /* Is it the position of the first % parameter ? */
    if (start == posPercent - 1) {
      if (start < (paramNb-1)) result = STRCAT(result,"ptr,");
      else result = STRCAT(result,",ptr");
      continue;
    }
      
    /* get the parameter */
    tmp = tfMultiArgN(tmp2,start);
      
    if (tfIsExit(tmp)) { strFree(result); return NULL; }
    if (tfIsGenerator(tmp)) { strFree(result); return NULL; }
      
    isUsrDef = AbSynIsUserDef(tfGetExpr(tmp));
    
    /* output param */
    str_tmp = itoa_(start);
    if (isUsrDef) {
      result = STRCAT(result,"realObject(parm");
      result = STRCAT(result,str_tmp);
      result = STRCAT(result,")");
    } else {
      result = STRCAT(result,"parm");
      result = STRCAT(result,str_tmp);
    }
    strFree(str_tmp);
      
    if (start < (paramNb-2)) result = STRCAT(result,",");
    else if ((start == (paramNb-2)) && (posPercent != paramNb)) result = STRCAT(result,",");
  }
  
  if (tfIsMultiReturn(tfMapRet(tf))) {
    if (paramNb) result = STRCAT(result,",");
    str_tmp = GenCppMultiRet(tf,!BL_WithType,BL_WithName,start,!BL_Exported);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  }
  result = STRCAT(result,")");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenCppParamsStaticPercent(TForm tf, Class *cl, int withType, int withName, int posPercent, int export) {

  String str_tmp, str_tmp2;
  String result;
  int isUsrDef;
  int paramNb,start;
  TForm tmp = NULL, tmp2;

  result = InitStr();

  result = STRCAT(result,"(");

  /* Regular params */
  tmp2 = tfMapArg(tf);
  paramNb = tfIsMulti(tmp2) ? tfMultiArgc(tmp2) : 1;
  start = 0;

  if (posPercent == 1) start++;

  if ((paramNb == 1) && (!start)) {
    if (tfIsGeneral(tmp2)) {
      str_tmp2 = AbSynToCpp(tfGetExpr(tmp2));
      isUsrDef = AbSynIsUserDef(tfGetExpr(tmp2));
    } else if (tfIsDeclare(tmp2)) {
      str_tmp2 = AbSynToCpp(tfGetExpr(tfDeclareType(tmp2)));
      isUsrDef = AbSynIsUserDef(tfGetExpr(tfDeclareType(tmp2)));
    } else { 
      strFree(result); 
      return NULL; 
    }

    if (!str_tmp2) { strFree(result); return NULL; }

    str_tmp =  (isUsrDef) ? strPrintf("%s *",str_tmp2) : strCopy(str_tmp2);

    if (withName && withType) { result = STRCAT(result,str_tmp); result = STRCAT(result," parm0");  }
    else if (withName) { result = STRCAT(result,"parm0"); }
    else if (withType) { result = STRCAT(result,str_tmp); }
    strFree(str_tmp);
    strFree(str_tmp2);
  } else if (paramNb > 1) {
    for (; start < paramNb; start++) {      
	
      if (start == posPercent - 1) {
	if (export) {
	  if (start < (paramNb-1)) result = STRCAT(result,"ptr,");
	  else result = STRCAT(result,",ptr");
	}
	continue;
      }

      tmp = tfMultiArgN(tmp2,start);
	
      if (tfIsExit(tmp)) { strFree(result); return NULL; }
      if (tfIsGenerator(tmp)) { strFree(result); return NULL; }

      str_tmp2 = AbSynToCpp(tfGetExpr(tmp));

      if (!str_tmp2) { strFree(result); return NULL; }
      isUsrDef = AbSynIsUserDef(tfGetExpr(tmp));
      
      /* init type name */
      str_tmp = isUsrDef ? strPrintf("%s *",str_tmp2) : strCopy(str_tmp2);
      
      strFree(str_tmp2);

      str_tmp2 = itoa_(start);

      /* output param */
      if (withName && withType) {
	result = STRCAT(result,str_tmp);
	result = STRCAT(result," parm");
	result = STRCAT(result,str_tmp2);
	strFree(str_tmp2);
      }
      else if (withName){
	result = STRCAT(result,"parm");
	result = STRCAT(result,str_tmp2);
	strFree(str_tmp2);
      }
      else if (withType)
	result = STRCAT(result,str_tmp);
	
      strFree(str_tmp);
	
      if (start < (paramNb-2)) result = STRCAT(result,",");
      else if ((start == (paramNb-2)) && (posPercent != paramNb)) result = STRCAT(result,",");
    }
  }

  if (tfIsMultiReturn(tfMapRet(tf))) {
    if (paramNb) result = STRCAT(result,",");
    str_tmp = GenCppMultiRet(tf,withType,withName,start,!BL_Exported);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  }
  result = STRCAT(result,")");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenCppParams_Body(TForm tf, String name, Class *cl, int isStatic) {
  TForm tmp;
  int cnt = 0;
  int start;
  int paramNb;
  String result, str_tmp;
  
  if (!tfIsMap(tf)) return NULL;

  result = InitStr();

  result = STRCAT(result,"(");

  /* Template params */
  if (cl->params) {
    int i;
    tmp = cl->params;
    paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
    if (paramNb == 1) {
      if (tfIsDeclare(tmp)) { 
	result = STRCAT(result,symString(tfDeclareId(tmp)));
	result = STRCAT(result,"::givetype()");
      } else { fprintf(stderr,"Error in GenGivetype (tmpl params): %d\n",tfTag(tmp)); }
    } 
    else 
      for (i=0;i<paramNb;i++) {
	TForm tmp2 = tfMultiArgN(tmp,i);
	if (tfIsDeclare(tmp2)) { 
	  result = STRCAT(result,symString(tfDeclareId(tmp2)));
	  result = STRCAT(result,"::givetype()");
	} else { fprintf(stderr,"Error in GenGivetype (tmpl params): %d\n",tfTag(tmp)); }
	if (i < paramNb-1) result = STRCAT(result,",");
      }
  }

  /* First parameter is special */
  tmp = tfMapArg(tf);
  paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
  start = 0;
  if (paramNb) {
    if (cl->params) result = STRCAT(result,",");
    if (!isStatic) {
      result = STRCAT(result,"ptr");
      start = 1;
      if (paramNb > 1) result = STRCAT(result,",");
    }
  }

  if ((paramNb == 1) && (start == 0)) {
    AbSyn ab = tfGetExpr(tmp);
    int usrdef;
      
    /* user type or not ? if yes => get the real object */
    if ((usrdef=AbSynIsUserDef(ab))) {
      str_tmp = AbSynToCpp(ab);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,"realObject(");
      strFree(str_tmp);
    }
    
    /* output param */
    str_tmp = itoa_(cnt);
    result = STRCAT(result,"parm");
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
    cnt++;
      
    if (usrdef) result = STRCAT(result,")");
  } 
  else if (paramNb > 1) {
    for (; start < paramNb; start++) {
      int usrdef;
      TForm tmp2 = tfMultiArgN(tmp,start);
      AbSyn ab = tfGetExpr(tmp2);
			       
      /* user type or not ? if yes => get the real object */
      if ((usrdef=AbSynIsUserDef(ab))) {
	str_tmp = AbSynToCpp(ab);
	if (!str_tmp) { strFree(result); return NULL; }
	result = STRCAT(result,"realObject(");
	strFree(str_tmp);
      }
      
      /* output param */
      str_tmp = itoa_(cnt);
      result = STRCAT(result,"parm");
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      cnt++;
      
      if (usrdef) result = STRCAT(result,")");
	
      if (start < paramNb-1) result = STRCAT(result,",");
    }
  }

  /* Extra parameters if multiple return */
  if (tfIsMultiReturn(tfMapRet(tf))) {
    if (paramNb) result = STRCAT(result,","); 
    str_tmp = GenCppMultiRetLocalDecl(tf,!BL_WithType);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
  }

  result = STRCAT(result,")");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GenCppParams_Global_Body(TForm tf, String name) {
  int cnt = 0;
  String result, str_tmp;
  
  if (!tfIsMap(tf)) return NULL;
  
  result = InitStr();

  tf = tfMapArg(tf);

  result = STRCAT(result,"(");

  if (tfIsDeclare(tf) || tfIsGeneral(tf)) {
    int usrdef;
    AbSyn ab = tfGetExpr(tf);

    /* user type or not ? if yes => get the real object */
    if ((usrdef=AbSynIsUserDef(ab))) result = STRCAT(result,"realObject(");
    
    /* output param */
    str_tmp = itoa_(cnt);
    result = STRCAT(result,"parm");
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
    cnt++;
    
    if (usrdef) result = STRCAT(result,")");
  } 
  else if (!tfIsMulti(tf)) { strFree(result); return NULL; }
  else if (tfMultiArgc(tf)) {
    int i;
    int paramNb = tfMultiArgc(tf);
    
    for (i=0; i<paramNb; i++) {
      int usrdef;
      TForm tmp = tfMultiArgN(tf,i);
      AbSyn ab = tfGetExpr(tmp);

      /* user type or not ? if yes => get the real object */
      if ((usrdef=AbSynIsUserDef(ab))) result = STRCAT(result,"realObject(");
      
      /* output param */
      str_tmp = itoa_(cnt);
      result = STRCAT(result,"parm");
      result = STRCAT(result,str_tmp);
      cnt++;
      
      if (usrdef) result = STRCAT(result,")");
      
      if (i < paramNb - 1) result = STRCAT(result,",");
    }
  }

  result = STRCAT(result,")");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}
/* -- End C++ Params -- */

/* ---------- END PARAMS ---------- */

/* ---------- TYPES ---------- */

/* -- Basic -- */

int IsBasicType(String s) {
  int i;
  for (i = 0; i < bAT_size; i++)
    if (strEqual(s,basicAldorTypes[i])) return theTRUE;
  return theFALSE;
}

String MappingTypes(String s) {
  int i;
  
  for (i = 0; i < bAT_size; i++)
    if (strEqual(s,basicAldorTypes[i])) return strCopy(basicCppTypes[i]);
  return NULL;
}

String MachineToCpp(AbSyn ab) {
  if (!abHasTag(ab,AB_Id)) return NULL;
  return (strEqual(abIdStr(ab),"XByte")) ? strPrintf("FiByte") : strPrintf("Fi%s",abIdStr(ab));
}

/* -- End Basic -- */

/* -- AbSyn -- */

/* Deals with items like A->B->C ... */ 
String FunctionPtrToAldor(AbSyn ab, int usePercent) {
  /* When the operator of an apply is "->", argc is 2 */
  String s1, s2, str_tmp, result;
  
  s1 = AbSynToAldor(abApplyArg(ab,(int) 0), usePercent); /* Parameter of the function */
  if (!s1) return NULL;
  s2 = AbSynToAldor(abApplyArg(ab,1), usePercent); /* Return type of the function */
  if (!s2) { strFree(s1); return NULL; }

  result = InitStr();

  /* s1 -> s2 */
  result = STRCAT(result,"(");
  result = STRCAT(result,s1);
  result = STRCAT(result," -> ");
  result = STRCAT(result,s2);
  result = STRCAT(result,")");

  /* to avoid wasting of memory */
  str_tmp =  strCopy(result);
  strFree(result);
 
  /* result */
  return str_tmp;
}

String ApplyToAldor(AbSyn ab, int usePercent) {
  /* l should be initialised */
  /* abTag(ab) == AB_Apply */ 
  int i;
  String result, str_tmp;
  int useName = theFALSE;

  if (strEqual(abIdStr(abApplyOp(ab)),"->")) return FunctionPtrToAldor(ab, usePercent);

  result = InitStr();
  result = STRCAT(result,abIdStr(abApplyOp(ab)));
  if (strEqual(abIdStr(abApplyOp(ab)),"Record")) useName = theTRUE;
  result = STRCAT(result,"(");

  for (i = 0; i < abApplyArgc(ab); i++) {
    AbSyn arg = abApplyArg(ab,i);
    switch (abTag(arg)) {
    case AB_Id:
      str_tmp = AbSynToAldor(arg,usePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      break;
    case AB_Apply:
      str_tmp = ApplyToAldor(abApplyArg(ab,i),usePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      break;
    case AB_Declare:
      if (useName) {
	result = STRCAT(result,abIdStr(abDeclareId(arg)));
	result = STRCAT(result,": ");
      }
      str_tmp = AbSynToAldor(abDeclareType(arg),usePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      break;
    default:
      strFree(result);
      return NULL;
      break;
    }
    if (i < abApplyArgc(ab) - 1) result = STRCAT(result,",");
  }
  result = STRCAT(result,")");
    
  str_tmp = strCopy(result);
  strFree(result);
    
  return str_tmp;
}

String CommaToAldor(AbSyn ab, int usePercent) {
  AbSyn tmp;
  String result, str_tmp;
  int i;
  
  if (!ab->abHdr.argc) 
    return strCopy("()");

  /* create string */
  result = InitStr();
  result = STRCAT(result,"(");
  for (i=0; i < abArgc(ab); i++) {
    tmp = abCommaArg(ab,i);
    switch (abTag(tmp)) {
    case AB_Id:
    case AB_Declare:
      str_tmp = AbSynToAldor(tmp,usePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      break;
    case AB_Apply:
      str_tmp = ApplyToAldor(ab,usePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      break;
    default:
      fprintf(stderr,"CommaToAldor - Bad Tag - %d\n",abTag(tmp));
      break;
    }
    if (i < abArgc(ab) - 1) result = STRCAT(result,",");
  }
  result = STRCAT(result,")");

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String QualifyToString(AbSyn ab, int usePercent) {
  String result,str_tmp;

  if (!abHasTag(ab->abQualify.what,AB_Id)) return NULL;
  result = InitStr();
  result = STRCAT(result,abIdStr(ab->abQualify.what));
  result = STRCAT(result,"$");
  str_tmp = AbSynToAldor(ab->abQualify.origin,usePercent);
  if (!str_tmp) { strFree(result); return NULL; }
  result = STRCAT(result,str_tmp);
  strFree(str_tmp);
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String AbSynToAldor(AbSyn ab, int usePercent) {

  if (!ab) return NULL;

  switch (abTag(ab)) {
  case AB_Id:
    if (IsBasicType(abIdStr(ab))) return strCopy(abIdStr(ab));
    else {		/* Usr type ... */
      if (strEqual(abIdStr(ab),"%") && !usePercent)
	return OutputDomNameAldor(CurrentClassAldor);
      else if (strEqual(abIdStr(ab),"%"))
	return strCopy("%%");	/* to get a real % because of fprintf ... */
      else
	return strCopy(abIdStr(ab));
    }
    break;
  case AB_Apply:
    if (strEqual(abIdStr(abApplyOp(ab)),"Enumeration")) return NULL;
    return ApplyToAldor(ab,usePercent);
  case AB_Nothing:
    return strCopy("()");
  case AB_Comma:
    return CommaToAldor(ab,usePercent);
  case AB_Qualify:
    return QualifyToString(ab,usePercent);
  case AB_Declare:
    return AbSynToAldor(abDeclareType(ab),usePercent);
  default:
    fprintf(stderr,"AbSynToAldor - Don't deal with abTag(ab) == %d\n",abTag(ab));
    return NULL;
  }    
}

String ApplyArgToTmplCpp(AbSyn ab) {
  int paramNb, i;
  String result, str_tmp;

  if (!ab) return NULL;
  if (!abIsApply(ab)) return NULL;
  paramNb = abApplyArgc(ab);
  result = InitStr();
  result = STRCAT(result,"<");
  for (i = 0; i < paramNb; i++) {
    str_tmp = AbSynToCpp(abApplyArg(ab,i));
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
    if (i < (paramNb-1)) result = STRCAT(result,",");
  }
  result = STRCAT(result,">");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String AbSynToCpp(AbSyn ab) {
  String str_tmp, result;

  if (!ab) return NULL;

  switch (abTag(ab)) {
  case AB_Id:
    str_tmp = MappingTypes(abIdStr(ab));
    if (!str_tmp) {		/* Usr type ... */
      if (strEqual(abIdStr(ab),"%")) return strCopy(CurrentClassCpp);
      return strCopy(abIdStr(ab));
    }
    return str_tmp;
  case AB_Apply:
    if (strEqual(abIdStr(abApplyOp(ab)),"->")) return NULL;
    if (strEqual(abIdStr(abApplyOp(ab)),"Enumeration")) return NULL;
    /* assuming we have now parameterized types */
    result = InitStr();
    str_tmp = strCopy(abIdStr(abApplyOp(ab)));
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
    str_tmp = ApplyArgToTmplCpp(ab);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    strFree(str_tmp);
    str_tmp = strCopy(result);
    strFree(result);
    return str_tmp;
  case AB_Nothing:
    return strCopy("void");
  case AB_Comma:
    if (!abArgc(ab)) return strCopy("void");
    fprintf(stderr,"Function pointers with multiple return are not handled\n");
    return NULL;
  case AB_Define:
    return AbSynToCpp(tfGetExpr(abTUnique(abDefineLhs(ab))));
  case AB_Declare:
    return AbSynToCpp(abDeclareType(ab));
  case AB_Qualify:
    if (abHasTag(ab->abQualify.origin,AB_Id) && (strEqual(abIdStr(ab->abQualify.origin),"Machine")))
      return MachineToCpp(ab->abQualify.what);
    return strCopy("void *");
  default:
    fprintf(stderr,"AbSynToCpp - Don't deal with abTag(ab) == %d\n",abTag(ab));
    return NULL;
  }
}


int AbSynIsUserDef(AbSyn ab) {
  /* Tells whether the type is a basic one or a user defined ... */
  /* Deal only with AB_Id */

  if (!ab) return theFALSE;

  switch (abTag(ab)) {
  case AB_Id:
    return !(IsBasicType(abIdStr(ab)));
  case AB_Apply:
    if (strEqual(abIdStr(abApplyOp(ab)),"->")) return theFALSE;
    if (strEqual(abIdStr(abApplyOp(ab)),"Enumeration")) return theFALSE;
    return theTRUE;
  case AB_Nothing:
    return theFALSE;
  case AB_Comma:
    return theFALSE;		/* To be completed ! */
  case AB_Qualify:
    return theFALSE;
  case AB_Declare:
    return AbSynIsUserDef(abDeclareType(ab));
  case AB_Define:
    return AbSynIsUserDef(tfGetExpr(abTUnique(abDefineLhs(ab))));
  default:
    fprintf(stderr,"AbSynIsUserDef - Don't deal with abTag(ab) == %d\n",abTag(ab));
    return theFALSE;
  }
}

int AbSynIsVoid(AbSyn ab) {
  /* Tells whether the type corresponds to 'void' */
  String str_tmp;

  if (!ab) return theFALSE;

  switch (abTag(ab)) {
  case AB_Id:
    str_tmp = MappingTypes(abIdStr(ab));
    if (!str_tmp) return theFALSE;
    if (!strEqual(str_tmp,"void")) {
      strFree(str_tmp);
      return theFALSE;
    }
    strFree(str_tmp);
    return theTRUE;    
  case AB_Apply:
    return theFALSE;
  case AB_Nothing:
    return theTRUE;
  case AB_Comma:
    return (!ab->abHdr.argc);	/* We don't deal with multiple return values */
  case AB_Define:
    return AbSynIsVoid(tfGetExpr(abTUnique(abDefineLhs(ab))));
  case AB_Qualify:
    return theFALSE;
  default:
    fprintf(stderr,"AbSynIsVoid - Don't deal with abTag(ab) == %d\n",abTag(ab));
    return theFALSE;
  }
}

/* -- End AbSyn -- */

/* -- TForm -- */

String TFormToAldor(TForm tf, int usePercent) {
  String str_tmp,result;
  AbSyn ab = tfGetExpr(tf);
  int i;
  int lg;

  if (tfIsExit(tf)) return NULL;
  if (tfIsGenerator(tf)) return NULL;

  if (ab) return AbSynToAldor(ab,usePercent);
  
  /* Function pointer ? */
  if (tfIsMap(tf)) {
    result = InitStr();
    str_tmp = TFormToAldor(tfMapArg(tf),usePercent);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,"(");
    result = STRCAT(result,str_tmp);
    result = STRCAT(result," -> ");
    strFree(str_tmp);
    str_tmp = TFormToAldor(tfMapRet(tf),usePercent);
    if (!str_tmp) { strFree(result); return NULL; }
    result = STRCAT(result,str_tmp);
    result = STRCAT(result,")");
    strFree(str_tmp);
    str_tmp = strCopy(result);
    strFree(result);
    return str_tmp;
  }
  
  /* Multiple value ? */
  if (tfIsMulti(tf)) {
    lg = tfMultiArgc(tf);
    result = InitStr();
    result = STRCAT(result,"(");
    for (i = 0; i < lg; i++) {
      str_tmp = TFormToAldor(tfMultiArgN(tf,i),usePercent);
      if (!str_tmp) { strFree(result); return NULL; }
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      if (i < lg-1) result = STRCAT(result,",");
    }
    result = STRCAT(result,")");
    str_tmp = strCopy(result);
    strFree(result);
    return str_tmp;
  }

  return NULL;
}

TForm tfMakeSubst(TForm tf) {
  TForm tmp = tfSubst(tfSubstSigma(tf),tf);
  if (!tfIsSubst(tmp)) return tmp;
  tmp = tfSubstArg(tf);
  /* I don't remember why this is here, I replace by 'return tmp;'
     if (!tfIsThird(tmp)) return tmp;
     tmp = tfThirdRestrictions(tmp);
     if (!tfIsWith(tmp)) return tf;
     return tfTUnique(tmp);
  */
  return tmp;
}

TForm tfFollowDefDecl(TForm tf) {
  if (tfIsSubst(tf)) tf = tfMakeSubst(tf);
  if (!tfIsDefine(tf) && !tfIsForward(tf)) {
    if (tfIsSubst(tf)) tf = tfMakeSubst(tf);
    return tf;
  }
  while (tfIsForward(tf)) tfFollow(tf);
  if (tfIsSubst(tf)) tf = tfMakeSubst(tf);
  if (!tfIsDefine(tf)) {
    if (tfIsSubst(tf)) tf = tfMakeSubst(tf);
    return tf;
  }
  tf = tfDeclareType(tfDefineDecl(tf));
  if (tfIsSubst(tf)) tf = tfMakeSubst(tf);
  while (tfIsForward(tf)) tfFollow(tf);
  if (tfIsSubst(tf)) tf = tfMakeSubst(tf);
  return tf;
}

/* -- End TForm -- */


/* -- Import/Export -- */

String GiveTypeAldorExport(Class *cl, int tab) {
  String result,str_tmp;
  TForm tmp,tmp2;
  int paramNb;
  int several = theFALSE;
  int cnt, cnt2;
  SymeList meths, meths2;
  Syme meth;
  
  result = InitStr();

  /* header */
  result = sprintTab(result,tab);
  str_tmp = addUnderScore(cl->id.export);
  result = STRCAT(result,"ALDOR__givetype__");
  result = STRCAT(result,str_tmp);
  result = STRCAT(result,"(");
  strFree(str_tmp);

  /* template params */
  if (cl->params) {
    int i;
    tmp = cl->params;
    paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
    if (paramNb == 1) {
      if (!tfIsDeclare(tmp)) { strFree(result); return NULL; }
      result = STRCAT(result,symString(tfDeclareId(tmp)));
      result = STRCAT(result,": ");
      str_tmp = TFormToAldor(tmp,!BL_UsePercent);
      if (!str_tmp) result = STRCAT(result,"Pointer");
      else result = STRCAT(result,str_tmp);
    } 
    else 
      for (i=0;i<paramNb;i++) {
	tmp2 = tfMultiArgN(tmp,i);
	if (!tfIsDeclare(tmp2)) { strFree(result); return NULL; }
	result = STRCAT(result,symString(tfDeclareId(tmp2)));
	result = STRCAT(result,": ");
	str_tmp = TFormToAldor(tmp2,!BL_UsePercent);
	if (!str_tmp) result = STRCAT(result,"Pointer");
	else result = STRCAT(result,str_tmp);
	if (i < paramNb-1) result = STRCAT(result,",");
      }
  }

  result = STRCAT(result,"): ");

  /* parents */
  
  for (Rewind(cl->extraClasses); !EOList(cl->extraClasses); GotoNext(cl->extraClasses)) {
    /* type of domain is of kind: Cat with ... or Join(Cat1,Cat2) with ... */
    Class *extra = (Class *) GetItem(cl->extraClasses);
	  
    /* several parents ? */
    Rewind(extra->parents);
    if (!EOList(extra->parents)) {
      GotoNext(extra->parents);
      if (!EOList(extra->parents)) { /* means several parents ... */
	result = STRCAT(result,"Join(");
	several = theTRUE;
      }
    }
	  
    /* output parents */
    Rewind(extra->parents);
    while (!EOList(extra->parents)) {
      Parent *p = (Parent *)GetItem(extra->parents);
      str_tmp = OutputDomNameAldor(p->id.typeAldor);
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      GotoNext(extra->parents);
      if (!EOList(extra->parents)) result = STRCAT(result,",");
    }
    
    /* end parents - begin body */
    if (several) result = STRCAT(result,") with {\n");
    else result = STRCAT(result," with {\n ");
    
    /* methods from extra class */
    cl->methCounter = 0;
    meths = tfThdExports(cl->methods) ? tfThdExports(cl->methods):tfParents(cl->methods);
    meths2 = meths;
    /* get the number of '%%' to be able to output only the last set of funtions 
       the rest is taken care of by the inheritance */
    for (cnt = 0; meths; meths = meths->rest) {
      meth = meths->first;
      if (!strcmp(symString(symeId(meth)),"%%")) cnt++;
    }
    /* go to the last '%%' */
    meths = meths2;
    for (cnt2 = 0; cnt2 < cnt; meths = meths->rest) {
      meth = meths->first;
      if (!strcmp(symString(symeId(meth)),"%%")) cnt2++;
    }
    /* output the remaining functions */
    for (; meths; meths = meths->rest) {
      meth = meths->first;
      str_tmp = GenAldorFnSig(symeType(meth),symString(symeId(meth)),extra,2,!BL_Exported,BL_UsePercent,!BL_UseTmplParms);
      if (!str_tmp) continue;
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
    }

    result = sprintTab(result,tab);
    result = STRCAT(result,"}");
  }

  if (!cl->extraClasses) {
    Rewind(cl->parents);
    if (!EOList(cl->parents)) {
      GotoNext(cl->parents);
      if (!EOList(cl->parents)) { /* means several parents ... */
	result = STRCAT(result,"Join(");
	several = theTRUE;
      }
    }
    Rewind(cl->parents);
    while (!EOList(cl->parents)) {
      Ident *id = (Ident *)GetItem(cl->parents);
      str_tmp = OutputDomNameAldor(id->typeAldor);
      result = STRCAT(result,str_tmp);
      strFree(str_tmp);
      GotoNext(cl->parents);
      if (!EOList(cl->parents)) result = STRCAT(result,",");
    }
    
    /* end parents - begin body */
    if (several) result = STRCAT(result,")");
    
  }

  result = STRCAT(result," == ");

  /* body == domain */
  str_tmp = OutputDomNameAldor(cl->id.typeAldor);  
  result = STRCAT(result,str_tmp); result = STRCAT(result,";\n");
  strFree(str_tmp);

  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

String GiveTypeCppExtern(Class *cl, int tab) {
  String result,str_tmp;
  TForm tmp,tmp2;
  int paramNb;

  result = InitStr();
  result = sprintTab(result,tab);

  result = STRCAT(result,"void *ALDOR_givetype_");
  result = STRCAT(result,cl->id.export);
  result = STRCAT(result,"(");

  /* template params */
  if (cl->params) {
    int i;
    tmp = cl->params;
    paramNb = tfIsMulti(tmp) ? tfMultiArgc(tmp) : 1;
    if (paramNb == 1) {
      if (!tfIsDeclare(tmp)) { strFree(result); return NULL; }
      result = STRCAT(result,"void *");
    } 
    else 
      for (i=0;i<paramNb;i++) {
	tmp2 = tfMultiArgN(tmp,i);
	if (!tfIsDeclare(tmp2)) { strFree(result); return NULL; }
	result = STRCAT(result,"void *");
	if (i < paramNb - 1) result = STRCAT(result,",");
      }
  }

  result = STRCAT(result,");\n");
  str_tmp = strCopy(result);
  strFree(result);
  return str_tmp;
}

void GenGiveTypeForExport(Class *cl, FILE *as, FILE *cc) {
  String toOutputAS = NULL, toOutputCC = NULL;

  toOutputAS = GiveTypeAldorExport(cl,1);
  if (toOutputAS) toOutputCC = GiveTypeCppExtern(cl,1);

  if (toOutputAS && toOutputCC) { 
    fprintf(as,"%s",toOutputAS); 
    fprintf(cc,"%s",toOutputCC); 
    strFree(toOutputAS);
    strFree(toOutputCC);
  } else {
    if (toOutputAS) strFree(toOutputAS);
    if (toOutputCC) strFree(toOutputCC);
  }
}

/* -- End Import/Export -- */

/* ---------- END TYPES ---------- */

/* Bugs and to do:
   . If SInt is specified without '$Machine' (using 'import from Machine;' for example), then
     the program doesn't translate SInt to FiSInt
   . Multiple return with a % in the list of returned things => just crash I think
   . overloading discrimine sur type de retour
   . function pointers
   . default parameters
   . value parameters for domains (like SI or SF)
   . global variable
   . Overloading resolved only on return type is not handled.
   */

/* ---------- UTILS ---------- */

/* -- String -- */

void printTab(FILE *f, int tab) {
  int i;
  for (i = 0; i < tab; i++)
    fprintf(f,"\t");
}


String sprintTab(String s, int tab) {
  int i;
  for (i = 0; i < tab; i++)
    s = STRCAT(s,"\t");
  return s;
}


String itoa_(int i) {
  return strPrintf("%d",i);
}


String OutputDomNameAldor(String id) {
  Length lg = strLength(id);
  int i;
  String tmp = strCopy(id);
  
  for (i=0; i < lg; i++)
    if (id[i] == '<') tmp[i] = '(';
    else if (id[i] == '>') tmp[i] = ')';
  
  return tmp;
}


String addUnderScore(String s) {
  Length j = strLength(s);
  String t = InitStr();
  int i,k;
  String res;
  
  for (i=0,k=0;i<j;i++,k++)
    if (s[i] != '_') t[k]=s[i];
    else {
      t[k]='_';
      k++;
      t[k]='_';
    }
  t[k] = '\0';
  
  res = strCopy(t);
  strFree(t);
  return res;
}


String transOperatorLike(String s) {
  Length j = strLength(s);
  String t = InitStr();
  int i;
  String res;

  for (i=0 ; i<j ; i++) {
    if ((res = GetStringForSpecialSymbol(s[i],theTRUE)))
      t = STRCAT(t,res);
    else {
      Length m = strLength(t);
      t[m] = s[i];
      t[m+1] = '\0';
    }
  }

  res = strCopy(t);
  strFree(t);
  return res;
}


String AldorifySpeSym(String s) {
  /* For each 'special symbol' an _ is placed before */

  Length lg = strLength(s);
  String t = InitStr();
  int i,k;
  String res;

  for (i=0,k=0;i<lg;i++,k++) {
    if (IsSpecialSymbol(s[i],theFALSE)) {
      t[k]='_'; 
      k++;
    }
    t[k]=s[i]; 
  }
  t[k]='\0';

  res = strCopy(t);
  strFree(t);
  return res;
}


String RealTypeString(AbSyn ab, int export) {
  String str_tmp, str_tmp2;
  int isUsrDef;

  str_tmp2 = AbSynToCpp(ab);
  isUsrDef = AbSynIsUserDef(ab);

  if (!isUsrDef) str_tmp = strPrintf("%s *",str_tmp2);
  else if (export) str_tmp = strPrintf("void *");
  else str_tmp = strPrintf("%s **",str_tmp2);

  strFree(str_tmp2);
  return str_tmp;
}

int abIsFuncPtr(AbSyn ab) {
  if (!ab) return theFALSE;
  if (!abIsApply(ab)) return theFALSE;
  return (strEqual(abIdStr(abApplyOp(ab)),"->"));
}

int tfIsFuncPtr(TForm tf) {
  if (!tf) return theFALSE;
  return (tfIsMap(tf));
}

int IsCommonOperator(String s) {
  int i;
  for (i=0 ; i<CO_size ; i++)
    if (strEqual(commonOperators[i],s)) return theTRUE;
  return theFALSE;
}

int IsInfixOperator(String s) {
  int i;
  for (i=0 ; i<IO_size ; i++)
    if (strEqual(infixOps[i],s)) return theTRUE;
  return theFALSE;
}

int IsSpecialSymbol(char c, int all) {
  /* the option 'all' is to be set when communicating with C (export) */
  if (GetStringForSpecialSymbol(c,all) == NULL)
    return theFALSE;
  else 
    return theTRUE;
}

String GetStringForSpecialSymbol(char c, int all) {
  /* the option 'all' is to be set when communicating with C (export) */
  int i;

  if (all)
    for (i=0 ; ccSpecCharIdTable[i].ch != 0 ; i++) {
      if (c == ccSpecCharIdTable[i].ch) 
	return ccSpecCharIdTable[i].str;
    }
  else
    for (i=0 ; ccSpecCharIdTable[i].ch != 0 ; i++) {
      if ((c == ccSpecCharIdTable[i].ch) 
	  && (c != BANG)
	  && (c != QMARK))
	return ccSpecCharIdTable[i].str;
    }

  return NULL;
}

/* -- End String -- */

/* -- Test -- */

int PercentsInParms(TForm tf, Class *cl) {
  /* 
   * Returns the position of the % in the list of params
   * position is starting from 1
   * returns 0 if no %
   */
  if (!tfIsMulti(tf)) {
    if (tfIsDeclare(tf)) {
      if (strEqual(symString(tfDeclareId(tf)),"%")) return 1;
      if (strEqual(symString(tfDeclareId(tf)),cl->id.basic)) return 1;
    } else if (tfIsGeneral(tf)) {
      AbSyn ab = tfGetExpr(tf);
      String s = AbSynToAldor(ab,BL_UsePercent);
      if (!s) return 0;
      if (strEqual(s,"%%")) return 1;
      if (strEqual(s,cl->id.basic)) return 1;
      strFree(s);
    }
  } else {  /* tfIsMulti(tf) */
    int i,j;
    if ((i=tfMultiArgc(tf))) {
      for (j = 0; j < i; j++) {
	TForm tmp = tfMultiArgN(tf,j);
	if (tfIsDeclare(tmp)) {
	  if (strEqual(symString(tfDeclareId(tmp)),"%")) return j+1;
	  if (strEqual(symString(tfDeclareId(tmp)),cl->id.basic)) return j+1;
	} 
	else if (tfIsGeneral(tmp)) {
	  AbSyn ab = tfGetExpr(tmp);
	  String s = AbSynToAldor(ab,BL_UsePercent);
	  if (s) {
	    if (strEqual(s,"%%")) return j+1;
	    if (strEqual(s,cl->id.basic)) return j+1;
	    strFree(s);
	  }
	} 
      }
    }
  }
  return 0;
}


int StaticOrNot(TForm tf, Class *cl) {
  if (!tfIsMulti(tf)) {
    if (tfIsDeclare(tf)) {
      if (strEqual(symString(tfDeclareId(tf)),"%")) return theFALSE;
      else return !strEqual(symString(tfDeclareId(tf)),cl->id.basic);
    } else if (tfIsGeneral(tf)) {
      AbSyn ab = tfGetExpr(tf);
      if (!ab) return theTRUE;
      if (!abHasTag(ab,AB_Id)) return theTRUE;
      if (strEqual(abIdStr(ab),"%")) return theFALSE;
      return !strEqual(abIdStr(ab),cl->id.basic);
    } else return theTRUE;
  } else {
    if (!tfMultiArgc(tf)) return theTRUE;
    else {
      TForm tmp = tfMultiArgN(tf,(Length) 0);
      if (tfIsDeclare(tmp)) {
	if (strEqual(symString(tfDeclareId(tmp)),"%")) return theFALSE;
	return !strEqual(symString(tfDeclareId(tmp)),cl->id.basic);
      } 
      else if (tfIsGeneral(tmp)) {
	AbSyn ab = tfGetExpr(tmp);
	if (!ab) return theTRUE;
	if (!abHasTag(ab,AB_Id)) return theTRUE;
	if (strEqual(abIdStr(ab),"%")) return theFALSE;
	return !strEqual(abIdStr(ab),cl->id.basic);
      } else return theTRUE;
    }
  }
}


int SpecialCppFunc(String s) {
  if (strEqual(s,"char")) return theTRUE;
  return theFALSE;
}

/* -- End Test -- */

/* -- ID Handling -- */


String operatorForC(String func_id) {
  /* Get for the "extern C" a suitable string representing 
     the operator */
  /* pre: func_id[0] is a Special Symbol */

  String s;
  String res;

  if (!(IsCommonOperator(func_id)))
    return transOperatorLike(func_id);

  s = InitStr();
  s = STRCAT(s,"Op");
  s = STRCAT(s,GetStringForSpecialSymbol(func_id[0],theTRUE));
  res = strCopy(s);
  strFree(s);
  return res;
}


String createFnIdExternC(String s, Class *cl) {
  String str_tmp1,str_tmp2,str_tmp3;
  const char ALSTR[]="ALDOR_";
  int cnt;
  int status;
  String func_id = s;
  int i;
  Length k;

  /* Check if operator */
  status = REGULAR;
 
  if (IsCommonOperator(func_id)) status = OPERATOR;
  else {	/* try to see if SPECIAL_SYM */
    k = strLength(func_id);
    for (i=0 ; i<k ; i++) {
      if (IsSpecialSymbol(func_id[i],theTRUE)) {
	status = SPECIAL_SYM;
	break;
      }
    }
  }

  /* Format: ALDORMeth_ClassCnt */  
  cnt      = cl ? cl->methCounter : globalFuncCounter;
  str_tmp1 = strCopy(cl ? cl->id.export : "Global");
  str_tmp2 = (status != REGULAR) ? operatorForC(func_id) : strCopy(func_id);
  str_tmp3 = strPrintf("%s%s_%s%d",ALSTR,str_tmp2,str_tmp1,cnt);
  strFree(str_tmp2);
  strFree(str_tmp1);

  return str_tmp3;
}


String createFnIdExportC(String s, Class *cl) {
  String str_tmp  = createFnIdExternC(s,cl);
  String str_tmp2 = addUnderScore(str_tmp);
  strFree(str_tmp);
  return str_tmp2;
}


String createFnIdForCPP(String func_id, Class *cl) {
  String s,res;
  OpState status;
  int i;
  Length k;

  /* Check if operator */
  status = REGULAR;
 
  if (IsCommonOperator(func_id)) status = OPERATOR;
  else {	/* try to see if SPECIAL_SYM */
    k = strLength(func_id);
    for (i=0 ; i<k ; i++) {
      if (IsSpecialSymbol(func_id[i],theTRUE)) {
	status = SPECIAL_SYM;
	break;
      }
    }
  }

  /* Output */
  s = InitStr();
  if (status == OPERATOR) { s = STRCAT(s,"operator"); s = STRCAT(s,func_id); }
  else if (status == SPECIAL_SYM) {
    String t = operatorForC(func_id);
    s = STRCAT(s,t);
    strFree(t);
  } else if (SpecialCppFunc(func_id)) {
    s = STRCAT(s,func_id); s = STRCAT(s,cl->id.basic);
  }
  else s = STRCAT(s,func_id);

  if (scanForBelongsTo(cppKeywords,CK_size,s)) {
    res = InitStr();
    res = STRCAT(res,"_");
    res = STRCAT(res,s);
    res = STRCAT(res,"_");
    s = strCopy(res);
    strFree(res);
  }

  res = strCopy(s);
  strFree(s);
  return res;
}

String GenAldorFuncNameReg(String s) {
  /* this function will use AldorifySpeSym to ensure any function id
     to be in the right format */

  if (IsCommonOperator(s)) return strCopy(s);
  if (IsInfixOperator(s)) return strCopy(s); /* to deal with /\ for example */
  return AldorifySpeSym(s);  /* *+ -> _*_+, round+ -> round_+ */
}


String GenAldorFuncName(String s, Class *cl, int isExported) {
  /* this function assumes that CurrentClass is set if isExported is theTRUE */
  return (isExported ? createFnIdExportC(s,cl) : GenAldorFuncNameReg(s));
}


String GetIdFromAbSyn(AbSyn ab) {
  if (abTag(ab) == AB_Define) return abIdStr(abDeclareId(abDefineLhs(ab)));
  else return NULL;
}

/* -- End ID Handling -- */

/* ---------- END UTILS ---------- */

/* -------------------- INIT FOR ALL STRUCTS ----------------------- */
/* InitList is in the "List management" section ... */

Function *InitFunc() {
  Function *fn = alloc(Function);
  fn->id       = NULL;
  fn->params   = NULL;
  fn->ret_typ  = NULL;
  fn->isStatic = theTRUE;
  return fn;
}

Class *InitClass() {
  Class *cl = alloc(Class);
  cl->parents = InitList();
  cl->extraClasses = InitList();
  cl->methods = NULL;
  cl->params  = NULL;
  cl->methCounter = 0;
  cl->classCounter = 0;
  return cl;
}

String STRCAT(String s1, String s2) {
  String s = strConcat(s1,s2);
  strFree(s1);
  return s;
}

/* -------------------- LIST MANAGEMENT -------------------------- */

MyList *InitList() {
  MyList *lst;

  lst = alloc(MyList);
  lst->head    = NULL;
  lst->tail    = NULL;
  lst->current = NULL;

  return lst;
}

void Append(MyList *lst, CellType ct, void *elem) {
  Cell *tmp;

  if (!lst) { fprintf(stderr, "Append: List not initialized\n"); exit((int) 0); }

  /* Creation of the cell to append */
  tmp = alloc(Cell);
  tmp->Item = elem;
  tmp->ct = ct;
  tmp->next = NULL;

  /* Append the cell to the end of the list */
  if (!(lst->head)) {
    lst->tail    = tmp;
    lst->current = tmp;
    lst->head    = tmp;
  } else {
    lst->tail->next = tmp;
    lst->tail       = tmp;
  }
}

void Rewind(MyList *l) {
  if (!l) return;
  l->current = l->head;
}

void *GetItem(MyList *l) {
  if (!l) return NULL;
  if (!l->current) return NULL;
  return l->current->Item;
}

void GotoNext(MyList *l) {
  if (!l) return;
  if (!l->current) return;
  l->current = l->current->next;
}

void HeadToNext(MyList *l) {
  if (!l) return;
  if (!l->current) return;
  l->head = l->current->next;
}

int Empty(MyList *l) {
  return (!l || !l->head);
}

int NotEmpty(MyList *l) {
  return (l && l->head);
}

int EOList(MyList *l) {
  if (!l) return theTRUE;
  return !(l->current);
}

/* -------------------- MEMORY MANAGEMENT -------------------------- */

void FreeList(MyList *l) {
  if (!l) return;
  if (Empty(l)) { stoFree(l); return; }
  Rewind(l);
  while (!EOList(l)) {
    HeadToNext(l);
    stoFree(l->current);
    Rewind(l);
  }
  stoFree(l);
}

void FreeFunction(Function *fn) {
  if (!fn) return;
}

void FreeClass(Class *cl) {
  if (!cl) return;

  FreeList(cl->extraClasses);
  FreeList(cl->parents);
  stoFree(cl);  
}
