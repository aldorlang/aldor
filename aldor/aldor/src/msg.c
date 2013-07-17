/*****************************************************************************
 *
 * msg.c: Message catalog manipulation.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

#include "axlgen.h"
#include "file.h"

/*
 * In-core structures to keep track of messages.
 */
struct msgHdr {
        FILE            *file;
        unsigned short  setcount;
        unsigned short	setmax;
        struct msgSet   *set;
};

struct msgSet {
        unsigned short	setno;
        unsigned short	msgcount;
        unsigned long	position;	/* offset of msg set info in file */
        struct msgMsg	*msg;		/* 0 => lazy */
};

struct msgMsg {
	unsigned short	msgno;
	unsigned short	length;
        unsigned long   position;       /* offset of msg text in file */
        char    	*text; 		/* 0 => lazy */
};


/*
 * Local macros and function declarations
 */
#define arrayNew(T,n)	((T *) stoAlloc((int) OB_Other,(n)*sizeof(T)))
#define msgHdrNew(n) 	arrayNew(struct msgHdr, n)
#define msgSetNew(n) 	arrayNew(struct msgSet, n)
#define msgMsgNew(n) 	arrayNew(struct msgMsg, n)

static struct msgHdr	*msgLoadHdr 	(FILE *);
static struct msgSet	*msgLoadSets	(FILE *, long postion, unsigned n);
static struct msgMsg	*msgLoadMsgs	(FILE *, long postion, unsigned n);
static char		*msgLoadText	(FILE *, long postion, unsigned n);


/*
 * The state of the message database.
 */
static struct msgHdr	*fcache    = 0;
static struct msgInfo	*defaults  = 0;


/*
 * Exported operations
 */
void
msgDefaults(struct msgInfo *d)
{
	defaults = d;
}

void
msgOpen(FileName fname)
{
	FILE	*file;
	
	file    = fileRbOpen(fname);
	fcache  = msgLoadHdr(file);
	if (!fcache) { fclose(file); return; }

	fcache->set = msgLoadSets(file, ftell(file), fcache->setcount);
}

Msg
msgByName(MsgSet setno, String name)
{
	struct msgInfo	*m;

	if (defaults)
		for (m = defaults; m->setno != -1; m++) {
			if (m->setno != setno) continue;
			if (strEqual(m->name, name)) return m->msgno;
		}
	
	return MSG_NOT_FOUND;
}

Msg
msgByAName(MsgSet setno, String name)
{
	struct msgInfo	*m;

	if (defaults)
		for (m = defaults; m->setno != -1; m++) {
			if (m->setno != setno) continue;
			if (strAEqual(m->name, name)) return m->msgno;
		}
	
	return MSG_NOT_FOUND;
}

String
msgName(MsgSet setno, Msg msg)
{
	struct msgInfo	*m;

	if (defaults)
		for (m = defaults; m->setno != -1; m++) {
			if (m->setno != setno) continue;
			if (msg == m->msgno) return m->name;
		}
	
	return "MSG_NOT_FOUND";
}

int
msgNumber(MsgSet setno, Msg msgno)
{
	return msgno;
}

int
msgMax(MsgSet setno)
{
	int		mmax = 0;
	struct msgInfo	*m;

	if (defaults)
		for (m = defaults; m->setno != -1; m++) {
			if (m->setno != setno) continue;
			if (m->msgno > mmax)  mmax = m->msgno;
		}
	return mmax;
}

String
msgGet(MsgSet setno, Msg msgno)
{
	/* If there is an opened file, look there first. */
	if (fcache && 0 <= setno && setno <= fcache->setmax) {
		int		i;
		struct msgSet	*set;
		struct msgMsg	*msg;
		
		for (i=0, set=fcache->set; i < fcache->setcount; i++, set++)
			if (set->setno == setno) break;
		if (i == fcache->setcount) goto useDefaults;

		if (!set->msg) set->msg = msgLoadMsgs(
			fcache->file, set->position, set->msgcount
		);

		for (i=0, msg=set->msg; i < set->msgcount; i++, msg++)
			if (msg->msgno == msgno) break;
		if (i == set->msgcount) goto useDefaults;

		if (!msg->text) msg->text = msgLoadText(
			fcache->file, msg->position, msg->length
		);

		return msg->text;
	}
useDefaults:
	if (defaults) {
		struct msgInfo	*d;

		for (d = defaults; d->setno != -1; d++)
			if (d->msgno == msgno && d->setno == setno)
				return d->text;
	}
	return "[Message not found]";
}

void
msgClear(void)
{
	int		i, j;
	struct msgSet	*set;
	struct msgMsg	*msg;

	if (!fcache) return;

	for (i = 0, set = fcache->set; i < fcache->setcount; i++, set++) {
		if (!set->msg) continue;
		for (j = 0, msg = set->msg; j < set->msgcount; j++, msg++) {
			if (msg->text) {
				stoFree((Pointer) msg->text);
				msg->text = 0;
			}
		}
	}
}

void
msgClose(void)
{
	int		i, j;
	struct msgSet	*set;
	struct msgMsg	*msg;

	if (!fcache) return;

	fclose(fcache->file);

	for (i = 0, set = fcache->set; i < fcache->setcount; i++, set++) {
		if (!set->msg) continue;
		for (j = 0, msg = set->msg; j < set->msgcount; j++, msg++)
			if (msg->text) stoFree((Pointer) msg->text);
		stoFree((Pointer) set->msg);
	}
	stoFree((Pointer) fcache->set);
	stoFree((Pointer) fcache);

	fcache = 0;
}

int
msgFPrintf(FILE *fout, MsgSet setno, Msg msgno, ...)
{
	int	cc;
	va_list	argp;

	va_start(argp, msgno);
	cc = msgVFPrintf(fout, setno, msgno, argp);
	va_end(argp);

	return cc;
}

int
msgVFPrintf(FILE *fout, MsgSet setno, Msg msgno, va_list argp)
{
	return vfprintf(fout, msgGet(setno, msgno), argp);
}

/****************************************************************************
 * 
 * X/Open message catalog file format:
 *
 *
 *  msg0hdr
 *    msg0set	++ 1
 *      msg0msg -- 1.1
 *      msg0msg -- 1.2
 *    msg0set   ++ 2
 *      msg0msg -- 2.1
 *      msg0msg -- 2.2
 *      msg0msg -- 2.3
 *   ...
 *  message texts
 */

#define MSG0_MAGIC 505

struct msg0hdr {
	unsigned long 		magic;
	unsigned short 		setcount;
	unsigned short		setmax;
	char 			filler[20];
};

struct msg0set {
	unsigned short		setno;
	unsigned short		msgcount;
};

struct msg0msg {
	unsigned short 		msgno;
	unsigned short		length;
	unsigned long		position;
};

static struct msgHdr *
msgLoadHdr(FILE *file)
{
	struct msg0hdr	hdr0;
	struct msgHdr	*hdr;

	fseek(file, long0, SEEK_SET);
        IgnoreResult(fread(&hdr0, sizeof(hdr0), 1, file));

	if (hdr0.magic != MSG0_MAGIC) return 0;

	hdr           = msgHdrNew(1);
	hdr->file     = file;
	hdr->setcount = hdr0.setcount;
	hdr->setmax   = hdr0.setmax;

	return hdr;
}

static struct msgSet *
msgLoadSets(FILE *file, long offset, unsigned count)
{
	struct msg0set	set0;
	struct msgSet	*set;
	int		i;

	fseek(file, offset, SEEK_SET);
	set = msgSetNew(count);

	for (i = 0; i < count; i++) {
		IgnoreResult(fread(&set0, sizeof(set0), 1, file));
		set[i].setno    = set0.setno;
		set[i].msgcount = set0.msgcount;
		set[i].msg      = 0;

		/* Save position of msg info, then skip over it. */
		set[i].position = ftell(file);
		fseek(file, set0.msgcount * sizeof(struct msg0msg), SEEK_CUR);
	}
	return set;
}

static struct msgMsg *
msgLoadMsgs(FILE *file, long offset, unsigned count)
{
	struct msg0msg	msg0;
	struct msgMsg	*msg;
	int		i;

	fseek(file, offset, SEEK_SET);
	msg = msgMsgNew(count);

	for (i = 0; i < count; i++) {
		IgnoreResult(fread(&msg0, sizeof(msg0), 1, file));
		msg[i].msgno    = msg0.msgno;
		msg[i].length   = msg0.length;
		msg[i].position = msg0.position;
		msg[i].text     = 0;
	}
	return msg;
}

static char *
msgLoadText(FILE *file, long offset, unsigned count)
{
	char	*s;
	IgnoreResult(fseek(file, offset, SEEK_SET));
	s = strAlloc(count);
	IgnoreResult(fread(s, sizeof(char), count, file));
	return s;
}
