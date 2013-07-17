/*****************************************************************************
 *
 * ccode_t.c: Test C code manipulation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#if !defined(TEST_CCODE) && !defined(TEST_ALL)

void testCCode(void) { }

#else

#include "axlgen.h"
#include "ccode.h"

char	floatBuf[MAX_FLOAT_SIZE];

void
testCCode(void)
{
	CCode	cco, cco1, ccov[11], ccouu[10], ccovv[10];

	printf("Parenthesization: \n");
	cco = ccoAsst(
		ccoStarAsst(
			ccoIdOf("d"),
			ccoIdOf("e")),
		ccoPlusAsst(
			ccoIdOf("f"),
			ccoStar(
				ccoIdOf("g"),
				ccoPlus(
					ccoPlus(ccoIdOf("h"),ccoIdOf("i")),
					ccoPlus(ccoIdOf("j"),ccoIdOf("k")) ))));
	ccoPrint(osStdout, cco, CCOM_StandardC);
	printf("\n");

	printf("Copy:\n");
	cco1 = ccoCopy(cco);
	ccoPrint(osStdout, cco1, CCOM_StandardC);
	printf("\n");
	ccoFree(cco1);

	printf("Original:\n");
	ccoPrint(osStdout, cco, CCOM_StandardC);
	printf("\n");
	ccoFree(cco);

	/*
	 * extern int	main(int, char *[]);
	 *
	 * int
	 * main(int argc, char *argv[])
	 * {
	 *	static int i;
	 *	const char *fmt = " %s";
	 *
	 *	for (i = 0; i < argc; i++)
	 *		switch (i) {
	 *		case 0:
	 *			printf("The arguments are:");
	 *			break;
	 *		default:
	 *			printf(fmt,
	 *			argv[i]);
	 *		}
	 *	printf("\n");
	 *	return 0;
	 * }
	 */
	cco = ccoUnit(ccoMany2(
	  ccoDecl(ccoMany2(ccoExtern(), ccoInt()),
		ccoFCall(ccoIdOf("main"), ccoMany2(
		  ccoParam(NULL, ccoInt(), NULL),
		  ccoParam(NULL, ccoChar(), ccoPreStar(ccoARef(NULL, NULL))) )) ),

	  ccoFDef(
		ccoInt(),

		ccoIdOf("main"),

		ccoMany2(
		  ccoParam(ccoIdOf("argc"), ccoInt(),
			ccoIdOf("argc")),
		  ccoParam(ccoIdOf("argv"), ccoChar(),
			ccoPreStar(ccoARef(ccoIdOf("argv"), NULL))) ),

		ccoCompound(ccoMany5(
		  ccoDecl(ccoMany2(ccoStatic(), ccoInt()),
			ccoIdOf("i")),
		  ccoDecl(ccoQual(ccoConst(), ccoChar()),
			ccoAsst(ccoPreStar(ccoIdOf("fmt")),
				ccoStringOf(" %s"))),
		  ccoFor(
			ccoAsst(ccoIdOf("i"), ccoIntOf(int0)),
			ccoLT  (ccoIdOf("i"), ccoIdOf("argc")),
			ccoPostInc(ccoIdOf("i")),

			ccoSwitch(ccoIdOf("i"), ccoCompound(ccoMany3(
			  ccoCase(ccoIntOf(int0),
				ccoStat(ccoFCall(ccoIdOf("printf"),
					ccoStringOf("The arguments are:")))),
				ccoBreak(),
			  ccoDefault(
				ccoStat(ccoFCall(ccoIdOf("printf"), ccoMany2(
				     ccoIdOf("fmt"),
					ccoARef(ccoIdOf("argv"),
						ccoIdOf("i")) ))) ) ))) ),
		  ccoStat(ccoFCall(ccoIdOf("printf"), ccoStringOf("\n"))),
		  ccoReturn(ccoIntOf(int0)) )) )));

	printf("An entire program:\n");
	ccoPrint(osStdout, cco, CCOM_StandardC);
	printf("\n");

	printf("And now in K+R C:\n");
	ccoPrint(osStdout, cco, CCOM_OldC);
	printf("\n");

	ccoFree(cco);

	/*
	 * extern int main(int , char *[]);
	 * 
	 * typedef enum Object {
	 *         Obj_Name = OBJ_LIMIT,
	 *         Obj_Type,
	 *         Obj_Code,
	 *         LIMIT
	 * };
	 * 
	 * typedef union u_ref {
	 *         float fval;
	 *         double dval;
	 * };
	 * 
	 * typedef struct symtab {
	 *         short n;
	 *         unsigned int bit : 1;
	 *         union u_ref u;
	 * };
	 * 
	 * typedef register long Length;
	 * 
	 * struct symtab *sym[10];
	 * 
	 * enum Object obj;
	 */

	ccov[0] =
	  ccoDecl(ccoMany2(ccoExtern(), ccoInt()),
		ccoFCall(ccoIdOf("main"), ccoMany2(
		  ccoParam(NULL, ccoInt(), NULL),
		  ccoParam(NULL, ccoChar(), ccoPreStar(ccoARef(NULL, NULL))) )) );

	ccov[1] =
	  ccoDecl(ccoMany2(ccoTypedef(), ccoEnumDef(ccoIdOf("Object"),
	     ccoMany4(ccoAsst(ccoIdOf("Obj_Name"), ccoIdOf("OBJ_LIMIT")),
		      ccoIdOf("Obj_Type"),
		      ccoIdOf("Obj_Code"),
		      ccoIdOf("LIMIT")))), NULL);

	ccov[2] =
	  ccoDecl(ccoMany2(ccoTypedef(), ccoUnionDef(ccoIdOf("u_ref"),
	      ccoMany2(
		   ccoDecl(ccoFloat(),ccoIdOf("fval")),
		   ccoDecl(ccoDouble(),ccoIdOf("dval"))))), NULL);

	ccov[3] =
	  ccoDecl(ccoMany2(ccoTypedef(), ccoStructDef(ccoIdOf("symtab"),
	     ccoMany3(
		  ccoDecl(ccoShort(),ccoIdOf("n")),
		  ccoDecl(ccoMany2(ccoUnsigned(),ccoInt()), 
			  ccoBitField(ccoIdOf("bit"),ccoIntOf(1))),
		  ccoDecl(ccoUnionRef(ccoIdOf("u_ref")),ccoIdOf("u"))))), NULL);

	ccov[4] =
	  ccoDecl(ccoMany3(ccoTypedef(), ccoRegister(), ccoLong()),
			ccoIdOf("Length"));

	ccov[5] =
	  ccoDecl(ccoStructRef(ccoIdOf("symtab")),
			ccoPreStar(ccoARef(ccoIdOf("sym"), ccoIntOf(10))));

	ccov[6] =
	  ccoDecl(ccoEnumRef(ccoIdOf("Object")), ccoIdOf("obj"));

	/*
	 * void 
	 * error(char *fmt, ...)
	 * {
	 * 	va_list args;
	 * 
	 * 	va_start(args, fmt);
	 * 	fprintf(stderr, "error: ");
	 * 	vprintf(stderr, fmt, args);
	 * 	fprintf(stderr, "\n");
	 * 	va_end(args);
	 * 	exit(1);
	 * }
	 *
	 */

	ccov[7] =
	  ccoFDef(
		ccoVoid(),

		ccoIdOf("error"),

		ccoMany2(
		  ccoParam(ccoIdOf("fmt"), ccoChar(),
			ccoPreStar(ccoIdOf("fmt"))),
		  ccoVAParam()),

		ccoCompound(ccoMany8(
		  ccoDecl(ccoTypedefId(ccoIdOf("va_list")), ccoIdOf("args")),
		  NULL,
		  ccoStat(ccoFCall(ccoIdOf("va_start"), ccoMany2(
			    ccoIdOf("args"), ccoIdOf("fmt")))),
		  ccoStat(ccoFCall(ccoIdOf("fprintf"),
				   ccoMany2(ccoIdOf("stderr"),
					    ccoStringOf("error: ")))),
		  ccoStat(ccoFCall(ccoIdOf("vprintf"),
				   ccoMany3(ccoIdOf("stderr"),
					    ccoIdOf("fmt"),ccoIdOf("args")))),
		  ccoStat(ccoFCall(ccoIdOf("fprintf"),
				   ccoMany2(ccoIdOf("stderr"),
					    ccoStringOf("\n")))),
		  ccoStat(ccoFCall(ccoIdOf("va_end"), ccoIdOf("args"))),
		  ccoStat(ccoFCall(ccoIdOf("exit"), ccoIntOf(1))) )));

	/*
	 * void 
	 * fxn(short flag, char c)
	 * {
	 * 	auto unsigned x;
	 *	signed y;
	 *	Length length;
	 *	
	 *	flag |= sizeof(char );
	 *	length >>= +flag;
	 *	x -= x & ~1;
	 *	y <<= (x | y ? x << -y : x >> y);
	 *	flag &= x ^ y;
	 * }
	 */

	ccov[8] =
	  ccoFDef(
		ccoVoid(),

		ccoIdOf("fxn"),

		ccoMany2(
		  ccoParam(ccoIdOf("flag"), ccoShort(),
			ccoIdOf("flag")),
		  ccoParam(ccoIdOf("c"), ccoChar(), ccoIdOf("c"))),

		ccoCompound(ccoMany9(
		  ccoDecl(ccoMany2(ccoAuto(), ccoUnsigned()), ccoIdOf("x")),
		  ccoDecl(ccoSigned(),ccoIdOf("y")),
		  ccoDecl(ccoTypedefId(ccoIdOf("Length")),ccoIdOf("length")),
		  NULL,
  		  ccoStat(ccoOrAsst(ccoIdOf("flag"),
				    ccoSizeof(ccoType(ccoChar(),NULL)))),
  		  ccoStat(ccoDShAsst(ccoIdOf("length"), 
				    ccoPrePlus(ccoIdOf("flag")))),
  		  ccoStat(ccoMinusAsst(ccoIdOf("x"),ccoAnd(ccoIdOf("x"),
				    ccoNot(ccoIntOf(1))))),
	          ccoStat(ccoUShAsst(ccoIdOf("y"), ccoParen(ccoQuest(
			    ccoOr(ccoIdOf("x"),ccoIdOf("y")),
			    ccoUSh(ccoIdOf("x"),ccoPreMinus(ccoIdOf("y"))),
			    ccoDSh(ccoIdOf("x"),ccoIdOf("y")))))),
	          ccoStat(ccoAndAsst(
		     ccoIdOf("flag"),ccoXor(ccoIdOf("x"),ccoIdOf("y")))) )));

	/*
	 * int 
	 * calc(int num, char *vals[])
	 * {
	 * 	float f1, f2;
	 *	double d;
	 *	volatile short flag;
         *
	 *	fxn(&flag, vals[--num]);
	 *	f1 /= sym[num--]->(u.fval);
	 *	if (d > (double)3.141500)  {
	 *		d = sym[num - 1]->(u.dval);
	 *		d = d/(double)f1;
	 *		continue;
	 *	}
	 *	else  {
	 *		flag %= (short)f2;
	 *		flag ^= 1;
	 *	}
	 *	return (int)flag;
	 * }
	 */
	ccovv[0] = ccoDecl(ccoFloat(),ccoMany2(ccoIdOf("f1"),ccoIdOf("f2")));
	ccovv[1] = ccoDecl(ccoDouble(),ccoIdOf("d"));
	ccovv[2] = ccoDecl(ccoQual(ccoVolatile(),ccoShort()),ccoIdOf("flag"));
	ccovv[3] = 0;
	ccovv[4] = ccoStat(ccoFCall(ccoIdOf("fxn"), ccoMany2(
			    ccoPreAnd(ccoIdOf("flag")),
			    ccoARef(ccoIdOf("vals"),
				    ccoPreDec(ccoIdOf("num"))))));
	ccovv[5] = ccoStat(ccoDivAsst(ccoIdOf("f1"),ccoPointsTo(
				  ccoARef(ccoIdOf("sym"),
					  ccoPostDec(ccoIdOf("num"))),
				    ccoDot(ccoIdOf("u"),ccoIdOf("fval")) )));
	ccovv[6] = ccoIf(ccoGT(ccoIdOf("d"), ccoCast(ccoIdOf("double"),
			    ccoFloatOf(floatBuf, 3.1415))),
		        ccoCompound(ccoMany3(
				 ccoStat(ccoAsst(ccoIdOf("d"),ccoPointsTo(
				   ccoARef(ccoIdOf("sym"),
					   ccoMinus(ccoIdOf("num"),
						    ccoIntOf(1))),
				   ccoDot(ccoIdOf("u"),ccoIdOf("dval")) ))),
				 ccoStat(ccoAsst(ccoIdOf("d"),
						 ccoDiv(ccoIdOf("d"),
				   ccoCast(ccoIdOf("double"),ccoIdOf("f1"))))),
				 ccoContinue())),
		        ccoCompound(ccoMany2(
			     ccoStat(ccoModAsst(ccoIdOf("flag"),
				ccoCast(ccoIdOf("short"), ccoIdOf("f2")))),
			     ccoStat(ccoXorAsst(ccoIdOf("flag"),
				ccoIntOf(1))) )));
	ccovv[7] = ccoReturn(ccoCast(ccoIdOf("int"),ccoIdOf("flag")));

	ccov[9] =
	  ccoFDef(
		ccoInt(),

		ccoIdOf("calc"),

		ccoMany2(
		  ccoParam(ccoIdOf("num"), ccoInt(),
			ccoIdOf("num")),
		  ccoParam(ccoIdOf("vals"), ccoChar(),
			ccoPreStar(ccoARef(ccoIdOf("vals"), NULL))) ),

		ccoCompound(ccoMany8(
		  ccovv[0], ccovv[1], ccovv[2], ccovv[3], ccovv[4],
		  ccovv[5], ccovv[6], ccovv[7])) );

	/*
	 * int 
	 * main(int argc, char *argv[])
	 * {
	 *	static int i, j, id;
	 *	const char *fmt = " %s";
	 *	char ll[] =  {
	 *		'a',
	 *		'b'
	 *	};
	 *
	 * here: 
	 *	printf("Start here.\n");
	 *	id = calc(argc, argv);
	 *	while (i < argc)  {
	 *		do  {
	 *			if (i >= OBJ_LIMIT) 
	 *				printf("\t\t");
	 *			else  {
	 *				++i;
	 *				printf("\t\n");
	 *			}
	 *			printf("\n\n");
	 *		} while (i < id);
	 *		if (i != 0 && !(i == i % 2)) 
	 *			goto here;
	 *		else 
	 *			printf(fmt, 
	 *			argv[i]);
	 *		if (i == 1 || i != 0) {
	 *			for (j = 0; j < id; j++) 
	 *				if (i > id) 
	 *					printf("A nested if.");
	 *		}
	 *		else 
	 *			printf("\n");
	 *		if (i > 2) 
	 *			printf("i is > 2");
	 *		else  {
	 *			printf("\n");
	 *			printf(fmt, argv[i]);
	 *		}
	 *		if (i <= 3)  {
	 *			printf("\n");
	 *			printf("i is <= 3");
	 *		}
	 *		else  {
	 *			printf("\n");
	 *			printf(fmt, argv[i]);
	 *		}
	 *	}
	 *	printf("\n");
	 *	return 0;
	 * }
	 */

	ccouu[0] = ccoDecl(ccoMany2(ccoStatic(), ccoInt()),
			ccoMany3(ccoIdOf("i"),ccoIdOf("j"),ccoIdOf("id")));
	ccouu[1] = ccoDecl(ccoQual(ccoConst(), ccoChar()),
			ccoAsst(ccoPreStar(ccoIdOf("fmt")),
				ccoStringOf(" %s")));
	ccouu[2] = ccoDecl(ccoChar(), ccoAsst(ccoARef(ccoIdOf("ll"),NULL),
				     ccoInit(ccoMany2(ccoCharOf('a'),
						      ccoCharOf('b')))));
	ccouu[3] = 0;
	ccouu[4] = ccoLabel(ccoIdOf("here"),
			   ccoStat(ccoFCall(ccoIdOf("printf"),
				    ccoStringOf("Start here.\n"))));
	ccouu[5] = ccoStat(ccoAsst(ccoIdOf("id"),
			  ccoFCall(ccoIdOf("calc"),ccoMany2(ccoIdOf("argc"),
			  ccoIdOf("argv")))));

	  ccovv[0] = ccoDo(ccoCompound(ccoMany2(
		ccoIf(ccoGE(ccoIdOf("i"), ccoEnumOf("OBJ_LIMIT")),
		  ccoStat(ccoFCall(ccoIdOf("printf"), ccoStringOf("\t\t"))),
		  ccoCompound(ccoMany2(
		    ccoStat(ccoPreInc(ccoIdOf("i"))),
		    ccoStat(ccoFCall(ccoIdOf("printf"),ccoStringOf("\t\n")))))),
		ccoStat(ccoFCall(ccoIdOf("printf"), ccoStringOf("\n\n"))))),
	     ccoLT(ccoIdOf("i"), ccoIdOf("id")));

	  ccovv[1] = ccoIf(ccoLAnd(ccoNE(ccoIdOf("i"),ccoIntOf(int0)),
		        ccoLNot(ccoEQ(ccoIdOf("i"),
		           ccoMod(ccoIdOf("i"), ccoIntOf(2))))),
	       ccoGoto(ccoIdOf("here")),
	       ccoStat(ccoFCall(ccoIdOf("printf"), ccoMany2(
			 ccoIdOf("fmt"),
		         ccoARef(ccoIdOf("argv"), ccoIdOf("i"))) )));

	  ccovv[2] = ccoIf(ccoLOr(ccoEQ(ccoIdOf("i"),ccoIntOf(1)),
		      ccoNE(ccoIdOf("i"),ccoIntOf(int0))),
	       ccoFor(
		 ccoAsst(ccoIdOf("j"), ccoIntOf(int0)),
		 ccoLT(ccoIdOf("j"), ccoIdOf("id")),
		 ccoPostInc(ccoIdOf("j")),

		 ccoIf(ccoGT(ccoIdOf("i"), ccoIdOf("id")),
		       ccoStat(ccoFCall( ccoIdOf("printf"),
			 ccoStringOf("A nested if."))),
		       NULL)),

	       ccoStat(ccoFCall(ccoIdOf("printf"), ccoStringOf("\n"))));

	  ccovv[3] = ccoIf(ccoGT(ccoIdOf("i"), ccoIntOf(2)),
	       ccoStat(ccoFCall(ccoIdOf("printf"), ccoStringOf("i is > 2"))),
	       ccoCompound(ccoMany2(
		  ccoStat(ccoFCall(ccoIdOf("printf"), ccoStringOf("\n"))),
		  ccoStat(ccoFCall(ccoIdOf("printf"), ccoMany2(
			ccoIdOf("fmt"),
		        ccoARef(ccoIdOf("argv"), ccoIdOf("i")) ))))));

	  ccovv[4] = ccoIf(ccoLE(ccoIdOf("i"), ccoIntOf(3)),
	       ccoCompound(ccoMany2(
		  ccoStat(ccoFCall(ccoIdOf("printf"),
			ccoStringOf("\n"))),
		  ccoStat(ccoFCall(ccoIdOf("printf"),
			ccoStringOf("i is <= 3"))) )),
				    
	       ccoCompound(ccoMany2(
		 ccoStat(ccoFCall(ccoIdOf("printf"), ccoStringOf("\n"))),
		 ccoStat(ccoFCall(ccoIdOf("printf"), ccoMany2(
			  ccoIdOf("fmt"),
			  ccoARef(ccoIdOf("argv"), ccoIdOf("i"))))))));
	ccouu[6] = ccoWhile(
			ccoLT(ccoIdOf("i"), ccoIdOf("argc")),
	   		ccoCompound(ccoMany5(
			   ccovv[0], ccovv[1], ccovv[2], ccovv[3], ccovv[4])));

	ccouu[7] = ccoStat(ccoFCall(ccoIdOf("printf"), ccoStringOf("\n")));

	ccouu[8] = ccoReturn(ccoIntOf(int0));

	ccov[10] =
	  ccoFDef(
		ccoInt(),

		ccoIdOf("main"),

		ccoMany2(
		  ccoParam(ccoIdOf("argc"), ccoInt(),
			ccoIdOf("argc")),
		  ccoParam(ccoIdOf("argv"), ccoChar(),
			ccoPreStar(ccoARef(ccoIdOf("argv"), NULL))) ),

		ccoCompound(ccoMany9(
		  ccouu[0], ccouu[1], ccouu[2], ccouu[3], ccouu[4],
		  ccouu[5], ccouu[6], ccouu[7], ccouu[8])) );

	cco = ccoUnit(ccoNew(CCO_Many,11,
		ccov[0], ccov[1], ccov[2], ccov[3], ccov[4], ccov[5],
		ccov[6], ccov[7], ccov[8], ccov[9], ccov[10]));

       	printf("An entire program:\n");
	ccoPrint(osStdout, cco, CCOM_StandardC);
	printf("\n");

	printf("And now in K+R C:\n");
	ccoPrint(osStdout, cco, CCOM_OldC);
	printf("\n");

	printf("Now free it: ");
	ccoFree(cco);
	printf("DONE");
}

#endif
