/*
 * cscan -- a scanner for C.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cscan.h"

#define FNAMEMAX	1024

static int	scanGetc(), scanUngetc(), scanPeekc();
static void	scanSeeCppLine(), scanError();
static int 	isLineDirective();


static struct {
	CTokTag	kind;
	char	*text;
} keyword[] = {
	{ CTOK_Auto,	  "auto" },
	{ CTOK_Break,	  "break" },
	{ CTOK_Case,	  "case" },
	{ CTOK_Char,	  "char" },
	{ CTOK_Const,	  "const" },
	{ CTOK_Continue,  "continue" },
	{ CTOK_Default,	  "default" },
	{ CTOK_Do,	  "do" },
	{ CTOK_Double,	  "double" },
	{ CTOK_Else,	  "else" },
	{ CTOK_Enum,	  "enum" },
	{ CTOK_Extern,	  "extern" },
	{ CTOK_Float,	  "float" },
	{ CTOK_For,	  "for" },
	{ CTOK_Goto,	  "goto" },
	{ CTOK_If,	  "if" },
	{ CTOK_Int,	  "int" },
	{ CTOK_Long,	  "long" },
	{ CTOK_Register,  "register" },
	{ CTOK_Return,	  "return" },
	{ CTOK_Short,	  "short" },
	{ CTOK_Signed,	  "signed" },
	{ CTOK_Sizeof,	  "sizeof" },
	{ CTOK_Static,	  "static" },
	{ CTOK_Struct,	  "struct" },
	{ CTOK_Switch,	  "switch" },
	{ CTOK_Typedef,	  "typedef" },
	{ CTOK_Union,	  "union" },
	{ CTOK_Unsigned,  "unsigned" },
	{ CTOK_Void,	  "void" },
	{ CTOK_Volatile,  "volatile" },
	{ CTOK_While,	  "while" },
	{ 0,              0 }
};

/*
 * Scanner state:
 */
char			cscanFileNameBuf[FNAMEMAX];
char			*cscanFileName = cscanFileNameBuf;
int			cscanLineNumber;

static int		cscanIsStartOfLine, cscanHadEOF;
static int		cscanLastChar, cscanCurrChar;

static FILE *		cscanFile;
static char *		cscanBuf;
static int		cscanBufSize;

static struct ctok	cscanTok;
struct cspecial		*cscanSpecials;


/*
 * Scanner initialization:
 */
void
cscanInit(buf, bufsiz, inname, instr, sp)
	char		*buf;
	int		bufsiz;
	char		*inname;
	FILE		*instr;
	struct cspecial	*sp;
{
	/* Set so first scan will be on new line 1. */
	cscanLineNumber    = 0;
	cscanIsStartOfLine = 0;
	cscanHadEOF        = 0;
	cscanCurrChar      = '\n';

	strcpy(cscanFileName, inname);
	cscanFile          = instr;
	cscanBuf           = buf;
	cscanBufSize       = bufsiz;
	cscanSpecials      = sp;
}

struct ctok *
cscan()
{
	char		*buf   = cscanBuf;
	int		bufsiz = cscanBufSize;
	FILE		*instr = cscanFile;
	int		c, nextc, i, bix;
	enum ctokTag	tk;

	bix = 0;
	cscanTok.neutral = buf;

	c = scanGetc(instr);

	for (;;) {
		if (isspace(c)) {
			do {
				if (bix == bufsiz-1) scanError(buf,bix);
				buf[bix++] = c;
				c = scanGetc(instr);
			} while (isspace(c));
		}
		if (c == '#' && cscanIsStartOfLine) {
			char	*cppline = buf + bix;
			scanUngetc(c, instr);
			while (c != '\n' && c != EOF) {
				c = scanGetc(instr);
				if (bix == bufsiz-1) scanError(buf,bix);
				buf[bix++] = c;
				if (c == '\\') {
					c = scanGetc(instr);
					if (bix == bufsiz-1) scanError(buf,bix);
					buf[bix++] = c;
				}
			}
			buf[bix] = 0;
			scanSeeCppLine(cppline);
			c = scanGetc(instr);
			continue;
		}
		if (c == '/') {
			int lastc;
			if (scanPeekc(instr) != '*') break;

			if (bix == bufsiz-1) scanError(buf,bix);
			buf[bix++] = c;

			do {
				lastc  = c;
				c = scanGetc(instr);
				if (bix == bufsiz-1) scanError(buf,bix);
				buf[bix++] = c;
			} while (lastc != '*' || c != '/');
			c = scanGetc(instr);
			continue;
		}
		break;
	}

	buf[bix++]       = 0;
	buf             += bix;
	bufsiz          -= bix;
	cscanTok.string  = buf;

	if (c == EOF) {
		if (cscanHadEOF) return 0;

		buf[0] = 0;
		cscanTok.kind = CTOK_EOF;
		cscanHadEOF = 1;
		return &cscanTok;
	}

	if (c == '0' && ((nextc = scanPeekc(instr)) == 'x' || nextc == 'X')) {
		/*
		 * Hex integer:
		 *    0[xX][0-9a-fA-F]+[uUlL]?
		 */
		for (i = 0; isalnum(c); i++) {
			buf[i] = c;
			if (i == bufsiz-1) scanError(buf, i);
			c = scanGetc(instr);
		}
		scanUngetc(c, instr);
		buf[i] = 0;
		cscanTok.kind = CTOK_IntegerConstant;
		return &cscanTok;
	}
	if (isdigit(c) || (c == '.' && isdigit(scanPeekc(instr)) )) {
		/*
		 * Decimal or octal integer or decimal float:
		 *    [0-7]+[uUlL]?
		 *    int.frac{[eE][-+]?exp}[fFlL]?
		 *    (floats: int xor frac can be missing. Likewise . xor exp)
		 */
		int dot = 0, exp = 0, suf = 0;
		for (i = 0; isdigit(c); i++) {
			buf[i] = c;
			if (i == bufsiz-1) scanError(buf, i);
			c = scanGetc(instr);
		}
		if (c == '.') {
			dot = c;
			do {
				buf[i] = c;
				if (i == bufsiz-1) scanError(buf, i);
				c = scanGetc(instr);
				i++;
			} while (isdigit(c));
		}
		if (c == 'e' || c == 'E') {
			exp = c;
			buf[i] = c;
			if (i == bufsiz-1) scanError(buf, i);
			i++;
			c = scanGetc(instr);

			if (c == '+' || c == '-') {
				buf[i] = c;
				if (i == bufsiz-1) scanError(buf, i);
				i++;
				c = scanGetc(instr);
			}
			while (isdigit(c)) {
				buf[i] = c;
				if (i == bufsiz-1) scanError(buf, i);
				c = scanGetc(instr);
				i++;
			}
		}
		/* Liberal */
		while (c=='f'||c=='F'||c=='l'||c=='L'||c=='u'||c=='U') {
			buf[i] = c;
			if (i == bufsiz-1) scanError(buf, i);
			c = scanGetc(instr);
			i++;
		}
		scanUngetc(c, instr);
		buf[i] = 0;
		cscanTok.kind = (dot || exp)
				? CTOK_FloatingConstant
				: CTOK_IntegerConstant;
		return &cscanTok;
	}

	if (isalpha(c) || c == '_') {
		struct cspecial	*t;

		for (i = 0; isalnum(c) || c == '_'; i++) {
			buf[i] = c;
			if (i == bufsiz-1) scanError(buf, i);
			c = scanGetc(instr);
		}
		scanUngetc(c, instr);
		buf[i] = 0;

		for (i = 0; keyword[i].text; i++) {
			if (buf[0] == keyword[i].text[0] &&
			    !strcmp(buf, keyword[i].text))
			{
				cscanTok.kind = keyword[i].kind;
				return &cscanTok;
			}
		}
		for (t = cscanSpecials; t; t = t->next) {
			if (buf[0] == t->string[0] &&
			    !strcmp(buf, t->string))
			{
				cscanTok.kind = t->kind;
				return &cscanTok;
			}
		}
		
		cscanTok.kind = CTOK_Identifier;
		return &cscanTok;
	}

	if ( c == '"' || c == '\'' ||
	    (c == 'L' && scanPeekc(instr) == '"') ||
	    (c == 'L' && scanPeekc(instr) == '\'') )
	{
		int	c0, lastc;

		i = 0;
		if (c == 'L') {
			buf[i] = c;
			if (i == bufsiz-1) scanError(buf, i);
			c = scanGetc(instr);
			i++;
		}
			
		buf[i] = c;
		if (i == bufsiz-1) scanError(buf, i);
		i++;

		c0    = c;
		lastc = 0;
		for ( ; i < bufsiz-1; i++) {
			if (lastc == '\\' && c == '\\')
				lastc = 0;
			else
				lastc = c;
			c = scanGetc(instr);
			buf[i] = c;
			if (c == c0 && lastc != '\\') {
				buf[i+1] = 0;
				break;
			}
		}
		if (c != c0 && i == bufsiz-1) scanError(buf, i);
		cscanTok.kind = (c0 == '"') ? CTOK_String : CTOK_CharacterConstant;
		return &cscanTok;
	}

#define isnextc(ch) ((nextc==(ch))?(buf[i++]=nextc,nextc=scanGetc(instr),1):0)

	i        = 0;
	buf[i++] = c;
	nextc     = scanGetc(instr);

	switch (c) {
	case '.':
		if (isnextc('.')) {
			if (isnextc('.')) 
				tk = CTOK_DDDot; 	   /* ... */
			else 
				tk = CTOK_Error; 	   /* .. */
		}
		else
			tk = CTOK_Dot; 		   	   /* .	*/
		break;
	case '<':
		if (isnextc('<')) {
			if (isnextc('='))
				tk = CTOK_UShAsst;         /* <<= */
			else
				tk = CTOK_USh;             /* << */
		}
		else if (isnextc('='))
			tk = CTOK_LE;  		   	   /* <= */
		else
			tk = CTOK_LT;  		   	   /* <	*/
		break;
	case '>':
		if (isnextc('>')) {
			if (isnextc('='))
				tk = CTOK_DShAsst;         /* >>= */
			else
				tk = CTOK_DSh;  	   /* >> */
		}
		if (isnextc('='))
			tk = CTOK_GE;  		   	   /* >= */
		else
			tk = CTOK_GT;  		   	   /* >	*/
		break;
	case '-':
		if (isnextc('='))
			tk = CTOK_MinusAsst;  	   	   /* -= */
		else if (isnextc('-'))
			tk = CTOK_Dec;  		   /* -- */
		else if (isnextc('>'))
			tk = CTOK_PointsTo;  		   /* -> */
		else
			tk = CTOK_Minus;  		   /* -	*/
		break;
	case '+':
		if (isnextc('='))
			tk = CTOK_PlusAsst;  		   /* += */
		else if (isnextc('+'))
			tk = CTOK_Inc;  		   /* ++ */
		else
			tk = CTOK_Plus;  		   /* +	*/
		break;
	case '&':
		if (isnextc('='))
			tk = CTOK_AndAsst;  	   	   /* &= */
		else if (isnextc('&'))
			tk = CTOK_LAnd;  		   /* && */
		else
			tk = CTOK_And;  		   /* &	*/
		break;
	case '|':
		if (isnextc('='))
			tk = CTOK_OrAsst;  	   	   /* |= */
		else if (isnextc('|'))
			tk = CTOK_LOr;  		   /* || */
		else
			tk = CTOK_Or;  		   	   /* |	*/
		break;
	case '^':
		if(isnextc('='))
			tk = CTOK_XorAsst;  	   	   /* ^= */
		else
			tk = CTOK_Xor;  		   /* ^	*/
		break;
	case '*':
		if (isnextc('='))
			tk = CTOK_StarAsst;  		   /* *= */
		else
			tk = CTOK_Star;  		   /* *	*/
		break;
	case '/':
		if (isnextc('='))
			tk = CTOK_DivAsst;  		   /* /= */
		else
			tk = CTOK_Div;  		   /* /	*/
		break;
	case '%':
		if (isnextc('='))
			tk = CTOK_ModAsst;  		   /* %= */
		else
			tk = CTOK_Mod;  		   /* %	*/
		break;
	case '=':
		if (isnextc('='))
			tk = CTOK_EQ;  		   	   /* == */
		else
			tk = CTOK_Asst;  		   /* =	*/
		break;
	case '!':
		if (isnextc('='))
			tk = CTOK_NE;  		   	   /* != */
		else
			tk = CTOK_LNot;  		   /* !	*/
		break;
	case '(':
			tk = CTOK_OParen;  		   /* (	*/
		break;
	case ')':
		tk = CTOK_CParen;  			   /* )	*/
		break;
	case '[':
		tk = CTOK_OBrack;  			   /* [	*/
		break;
	case ']':
		tk = CTOK_CBrack;  			   /* ]	*/
		break;
	case '{':
		tk = CTOK_OCurly;  			   /* {	*/
		break;
	case '}':
		tk = CTOK_CCurly;  			   /* }	*/
		break;
	case ',':
		tk = CTOK_Comma;  			   /* ,	*/
		break;
	case ':':
		tk = CTOK_Colon;  			   /* :	*/
		break;
	case ';':
		tk = CTOK_Semi;  			   /* ;	*/
		break;
	case '?':
		tk = CTOK_Question;  			   /* ?	*/
		break;
	case '~':
		tk = CTOK_Not;  			   /* ~	*/
		break;
	default:
		tk = CTOK_Error;  			   /* .. */
		break;
	}
	buf[i] = 0;
	scanUngetc(nextc, instr);
	cscanTok.kind = tk;
	return &cscanTok;
}

void
cscanSpecial(s)
	struct cspecial *s;
{
	s->next = cscanSpecials;
	cscanSpecials = s;
}

static int
scanGetc(instr)
	FILE	*instr;
{
	if (cscanCurrChar == '\n') {
		cscanLineNumber++; 
		cscanIsStartOfLine = 1;
	}
	else {
		cscanIsStartOfLine = 0;
	}

	cscanLastChar = cscanCurrChar;
	cscanCurrChar = getc(instr);

	return cscanCurrChar;
}

static int
scanUngetc(c, instr)
	int	c;
	FILE	*instr;
{
	if (cscanLastChar == '\n') {
		cscanLineNumber--;
		cscanIsStartOfLine = 0;
	}
	cscanCurrChar = cscanLastChar;
	cscanLastChar = 0;
	return ungetc(c, instr);
}

static int
scanPeekc(instr)
	FILE	*instr;
{
	return ungetc(getc(instr), instr);
}

static void
scanError(buf, len)
	char	*buf;
	int	len;
{
	buf[len] = 0;
	fprintf(stderr, "Token too long (> %d): %s\n", len, buf);
	exit(1);
}

static void
scanSeeCppLine(s)
	char	*s;
{
	if (isLineDirective(s, cscanFileName, &cscanLineNumber))
		cscanLineNumber--;
}

/*
 * This function tests for a cpp line number directive.
 * If so, pPath and pLine are updated to contain the path and line number.
 */
static int
isLineDirective(line, pPath, pLine)
	char	*line, *pPath;
	int	*pLine;
{
	int	n;

	if (*line != '#') return 0;
	line++;

	/*
	 * Accept: # 2...  #2...  # line 2...   #line 2...
	 */
	while (isspace(*line))
		line++;
	if (!strncmp("line", line, 4))
		line += 4;
	while (isspace(*line))
		line++;
	
	if (!isdigit(*line)) return 0;
	n = *line - '0';
	line++;

	while (isdigit(*line)) {
		n *= 10;
		n += *line-'0';
		line++;
	}
	while (isspace(*line))
		line++;

	if (*line != '"') return 0;
	line++;

	while (*line && *line != '"')
		*pPath++ = *line++;

	*pLine = n;
	*pPath = 0;
	return 1;
}
