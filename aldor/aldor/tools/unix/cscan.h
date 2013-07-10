/*
 * cscan -- a scanner for C.
 */

#ifndef _CSCAN_H_
#define _CSCAN_H_

typedef enum ctokTag {
	CTOK_String,
	CTOK_CharacterConstant,
	CTOK_IntegerConstant,
	CTOK_FloatingConstant,
	CTOK_EnumerationConstant,
	CTOK_TypedefName,
	CTOK_Identifier,

	CTOK_OParen,	  /* (	  */
	CTOK_CParen,	  /* )	  */
	CTOK_OBrack,	  /* [	  */
	CTOK_CBrack,	  /* ]	  */
	CTOK_OCurly,	  /* {	  */
	CTOK_CCurly,	  /* }	  */

	CTOK_LAnd,	  /* &&   */
	CTOK_LOr,	  /* ||	  */
	CTOK_LNot,	  /* !	  */
	CTOK_And,	  /* &	  */
	CTOK_Or,	  /* |	  */
	CTOK_Xor,	  /* ^	  */
	CTOK_Not,	  /* ~	  */
	CTOK_Star,	  /* *	  */
	CTOK_Div,	  /* /	  */
	CTOK_Mod,	  /* %	  */
	CTOK_Plus,	  /* +	  */
	CTOK_Minus,	  /* -	  */
	CTOK_USh,	  /* <<	  */
	CTOK_DSh,	  /* >>	  */
	CTOK_Inc,	  /* ++	  */
	CTOK_Dec,	  /* --	  */
	CTOK_PointsTo,	  /* ->	  */
	CTOK_Dot,	  /* .	  */

	CTOK_EQ,	  /* ==	  */
	CTOK_NE,	  /* !=	  */
	CTOK_LT,	  /* <	  */
	CTOK_LE,	  /* <=	  */
	CTOK_GT,	  /* >	  */
	CTOK_GE,	  /* >=	  */

	CTOK_Asst,	  /*  =	  */
	CTOK_AndAsst,     /* &=	  */
	CTOK_OrAsst,	  /* |=	  */
	CTOK_XorAsst,     /* ^=	  */
	CTOK_StarAsst,	  /* *=	  */
	CTOK_DivAsst,	  /* /=	  */
	CTOK_ModAsst,	  /* %=	  */
	CTOK_PlusAsst,	  /* +=	  */
	CTOK_MinusAsst,	  /* -=	  */
	CTOK_UShAsst,     /* <<=  */
	CTOK_DShAsst,     /* >>=  */

	CTOK_Question,	  /* ?	  */
	CTOK_Colon,	  /* :	  */
	CTOK_Semi,	  /* ;	  */
	CTOK_Comma,	  /* ,	  */
	CTOK_DDDot,	  /* ...  */

	CTOK_Auto,	  /* auto	*/
	CTOK_Break,	  /* break	*/
	CTOK_Case,	  /* case	*/
	CTOK_Char,	  /* char	*/
	CTOK_Const,	  /* const	*/
	CTOK_Continue,	  /* continue	*/
	CTOK_Default,	  /* default	*/
	CTOK_Do,	  /* do 	*/
	CTOK_Double,	  /* double	*/
	CTOK_Else,	  /* else	*/
	CTOK_Enum,	  /* enum	*/
	CTOK_Extern,	  /* extern	*/
	CTOK_Float,	  /* float	*/
	CTOK_For,	  /* for	*/
	CTOK_Goto,	  /* goto	*/
	CTOK_If,	  /* if 	*/
	CTOK_Int,	  /* int	*/
	CTOK_Long,	  /* long	*/
	CTOK_Register,	  /* register	*/
	CTOK_Return,	  /* return	*/
	CTOK_Short,	  /* short	*/
	CTOK_Signed,	  /* signed	*/
	CTOK_Sizeof,	  /* sizeof	*/
	CTOK_Static,	  /* static	*/
	CTOK_Struct,	  /* struct	*/
	CTOK_Switch,	  /* switch	*/
	CTOK_Typedef,	  /* typedef	*/
	CTOK_Union,	  /* union	*/
	CTOK_Unsigned,	  /* unsigned	*/
	CTOK_Void,	  /* void	*/
	CTOK_Volatile,	  /* volatile	*/
	CTOK_While,	  /* while	*/

	CTOK_Error,	  /* Syntax error  */
	CTOK_EOF	  /* End of file */
} CTokTag ;

struct ctok {
	enum ctokTag	kind;
	char		*neutral;	/* white space, #... lines etc */
	char		*string;	/* the text of the token */
};

struct cspecial {
	enum ctokTag	kind;
	char		*string;
	struct cspecial	*next;
};

/*
 * cscanInit(buf, bufsiz, inname, instream, init_specials);
 *
 * Initialized the scanner.
 */
extern void		cscanInit (/* char *buf, int bufsiz, char * name, FILE *instr, struct cspecial *init_specials */);

/*
 * ptok = cscan();
 *
 * This returns a pointer to a static structure for the next token. 
 * The "buf" argument is used to hold the "neutral" and "string" text.
 */
extern struct ctok *	cscan (/* void */);

/*
 * cscanLineNumber;
 * cscanFileName;
 *
 * Gives the current position.
 */
extern char		*cscanFileName;
extern int		cscanLineNumber;

/*
 * cscanSpecial(p);
 * cscanSpecials;
 *
 * Tells the scanner that p->string should be classified specially as p->kind.
 * cscanSpecials is a variable is a list of all specials declared so far.
 */
extern void		cscanSpecial (/* struct cspecial* */);
extern struct cspecial	*cscanSpecials;

#endif
