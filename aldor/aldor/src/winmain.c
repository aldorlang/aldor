/*****************************************************************************
 *
 * winmain.c: Main program for Win32(s) version.
 *
 * Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
 *
 ****************************************************************************/

    /* Uncomment next line if you want go thru with the debugger. This way the
     * compiler will not stop waiting for syncronization with the shell.
     */
/* #define WINAXL_DEBUG */

#include "cmdline.h"
#include "util.h"
#include "emit.h"
#include "test.h"
#include <windows.h>
#include "pfc.h"
#include "winmain.h"

#if defined(__BORLANDC__)
#       include <dir.h>
#else
#       include <direct.h>
#endif

/*****************************************************************************
 * :: Local Variables
 *****************************************************************************/

static  LPSTR       cmdLine;
static  HWND        hParent         = NULL;
static  int         idEditWindow    = 0;

static  char *      tmpDir  = NULL;
static JmpBuf       winExitBuf;

/*****************************************************************************
 * :: Exported Variables
 *****************************************************************************/

FILE * win32out;
FILE * win32in;
FILE * win32err;

/*****************************************************************************
 * :: Local Prototypes
 *****************************************************************************/

static void         openStreams     (void);
static void         closeStreams     (void);

static void         editUndo        (void);
static void         editCopy        (void);
static void         editCut         (void);
static void         editPaste       (void);
static void         editClear       (void);

local void wglStart             (HWND, int, char **);

/*****************************************************************************
 * :: External Prototypes
 *****************************************************************************/

extern compCmd      (int, char **);

extern void     compGLoopEval   (FILE *, FILE *, EmitInfo);
extern void     compGLoopInit   (int, char **, FILE *, FileName *, EmitInfo *);
extern int      compGLoopFinish (FileName, EmitInfo);

/*****************************************************************************
 *
 * :: Local Definitions
 *
 *****************************************************************************/

/*****************************************************************************
 * :: openStreams(), closeStreams()
 *****************************************************************************/
/* Compiler Input/Output is redirected to the AXLIN, AXLOUT and AXLERR files.
 * Windows does not support tty i/o, so we redirect everything to these files
 * and the compiler shell will display them.
 */

static void
openStreams()
{
    LPSTR outFile = strConcat(tmpDir, AXLOUT);
    LPSTR inFile  = strConcat(tmpDir, AXLIN);
    LPSTR errFile = strConcat(tmpDir, AXLERR);    

    win32out = fopen(outFile, "w+");
    win32in  = fopen(inFile, "r");
    win32err = fopen(errFile, "w");    
    
    if (!win32out || !win32err)
        FatalAppExit(0, "Cannot open output streams...");

    osSetStreams(win32in, win32out, win32err);
    
    stoFree(outFile);
    stoFree(inFile);
    stoFree(errFile);    
}

static void
closeStreams()
{
    fclose(win32out);
    fclose(win32in);    
    fclose(win32err);    
}

/*****************************************************************************
 * :: winaxlExit()
 *****************************************************************************/
/* winaxlExit() must always be called before exiting. */
void
winaxlExit(int n)
{
        closeStreams();
        PostMessage(hParent, wm_Compiler_End, 0, 0L);
        SetActiveWindow(hParent);
        if (n != EXIT_SUCCESS) {
            PostQuitMessage(n);
            LongJmp(winExitBuf, 1);
        }
        else
            PostQuitMessage(n);
}

/*****************************************************************************
 * :: testSelf()
 *****************************************************************************/
/* Called by cmdline.c. We are not linking with *_t.c files, to make the
 * executable smaller.
 */
void
testSelf()
{}
/*****************************************************************************
 * :: callCompiler()
 *****************************************************************************/
/* Call the Aldor compiler passing the right arguments. */
void
callCompiler(void)
{
    int         argc;
    String *    argv;

    hParent = (HWND) pfcLParam();
    argv = &cmdLine;
    /* Windows pass arguments in a single string. osFixCmdLine() breaks
     * them and sets "argc", "argv".
     */
    osFixCmdLine(&argc, &argv);
    openStreams();

    if (!SetJmp(winExitBuf))
        compCmd(argc, argv);    /* <------- */
    else
        return;
        
    if (cmdHasInteractiveOption(argc, argv))
           wglStart(pfcHMainWnd(), argc, argv);
    else
           winaxlExit(0);
}

/*****************************************************************************
 * :: Interactive Loop
 *****************************************************************************/

static EmitInfo    finfo;
static FileName    fn;
static int         prevLastLine = 0;

#define BUF_SIZE        1024

local void
wglShowOutput(int editBox)
{
    LPSTR   outFile = strConcat(tmpDir, AXLOUT);
    char    buf [BUF_SIZE + 1];

    pfcAppendText(editBox, "\r\r\n");
    fseek(win32out, 0L, SEEK_SET);
    while (TRUE) {
        fgets(buf, BUF_SIZE, win32out);
        if (feof(win32out)) break;      /* <---- */
        if (buf[0] != 0)
            buf[pfcStrLen(buf) - 1] = 0; /* Remove 0x0A character */
         
        pfcAppendText(editBox, buf);
        pfcAppendText(editBox, "\r\r\n");
    }
    fclose(win32out);
    win32out = fopen(outFile, "w+");
    fseek(win32out, 0L, SEEK_SET);
    
    stoFree(outFile);
}

/* Evalute all lines in the edit box "id"  b/ween "fromLine" and "toLine" */
local void
wglEvalLines(int editBox, int fromLine, int toLine)
{
    int     i;
    LPSTR   lineStr;
    Buffer  evalBuf = bufNew();
    
    for (i = fromLine; i < toLine; i++) {
        lineStr = pfcGetLine(editBox, i);
        if (lineStr[0] == NULL) continue;  /* Skip blank lines */
        bufPuts(evalBuf, lineStr);
        bufPutc(evalBuf, '\n');
    }

    bufPutc(evalBuf, EOF);    
    bufStart(evalBuf);
    osSetStdinBuffer(evalBuf);
    
    compGLoopEval(osStdin, osStdout, finfo);
    
    wglShowOutput(editBox);
    bufFree(evalBuf);

}

local void
wglEvalNewLines(int editBox)
{
    HCURSOR oldCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
    int toLine = (int) SendMessage(pfcGetHWnd(editBox), EM_LINEFROMCHAR,
                                    -1, 0L) + 1;
    wglEvalLines(editBox, prevLastLine, toLine);
    prevLastLine = pfcGetLineCount(editBox) - 1;
    SetCursor(oldCursor);
}

local void
wglEval()
{
    wglEvalNewLines(idEditWindow);
    pfcSetFocus(idEditWindow);
}

local void
wglQuit()
{
    LPSTR   outFile = strConcat(tmpDir, AXLOUT);
    
    compGLoopFinish(fn, finfo);
    
    /* Clean output file */
    fclose(win32out);
    unlink(outFile);
    win32out = fopen(outFile, "w");
    stoFree(outFile);
    winaxlExit(0);
}

local void
wglStart(HWND hwnd, int argc, char ** argv)
{
    ShowWindow(pfcHMainWnd(), SW_SHOWMAXIMIZED);

    pfcMenu("&File");
      pfcMenuItem("New session", NULL);
      pfcMenuItem("Open session", NULL);
      pfcMenuItem("Save session", NULL);
      pfcMenuSeparator();
      pfcMenuItem("E&xit", wglQuit);
    pfcMenu("E&dit");
      pfcMenuItem("&Undo"            , editUndo);    
      pfcMenuItem("&Copy    Ctrl-Ins", editCopy);
      pfcMenuItem("C&ut     Shift-Del", editCut);
      pfcMenuItem("P&aste   Shift-Ins", editPaste);
      pfcMenuItem("C&lear   Del", editClear);      
      
    pfcMenuSingleItem("&Eval", wglEval);

    pfcMenu("&Help");    
      pfcMenuItem("&About", NULL);        
    
    compGLoopInit(argc, argv, osStdout, &fn, &finfo);
    
    idEditWindow = pfcCreateControl(PFC_EditBox, "edit", "",
               WS_CHILD | WS_VISIBLE | ES_MULTILINE | 
               ES_AUTOVSCROLL | WS_VSCROLL,
               0, 0, pfcMaxX(), pfcMaxY() - 16);
    SendMessage(pfcGetHWnd(idEditWindow), WM_SETFONT,
		(WORD) GetStockObject(ANSI_FIXED_FONT), FALSE);
    pfcSetFocus(idEditWindow);
}

/*****************************************************************************
 *
 * :: Menu Handlers
 *
 *****************************************************************************/
void
editUndo()
{
    pfcUndoEdit(idEditWindow);
}
void
editCopy()
{
    pfcCopySelection(idEditWindow);
}
void
editCut()
{
    pfcCutSelection(idEditWindow);
}
void
editPaste()
{
    pfcPasteSelection(idEditWindow);
}
void
editClear()
{
    pfcClearSelection(idEditWindow);
}

/*****************************************************************************
 *
 * :: Main Event Handlers
 *
 *****************************************************************************/

void
wmCreate()
{
    /* ShowWindow(pfcHMainWnd(), SW_SHOWMINNOACTIVE);	*/ /* Show as icon */
    tmpDir = osGetEnv("TMP");
    if (tmpDir == NULL)
            tmpDir = osGetEnv("TEMP");

    if (tmpDir == NULL)
            pfcFatalError("You need to set TMP or TEMP environment variable");
            
#if 1 /* WINAXL_DEBUG*/
    pfcMessageBox("Using debug version");  
    PostMessage(pfcHMainWnd(), wm_Compiler_Start, 0, (LPARAM)pfcHMainWnd());
#endif /* WINAXL_DEBUG */   
}

static void
wmSize()
{
    HWND hEditWindow;
    if (idEditWindow == 0) return;
    /* Resize the edit window */
    hEditWindow = pfcGetHWnd(idEditWindow);
    MoveWindow(hEditWindow, 0,0, pfcMaxX(), pfcMaxY() - 16, TRUE);
}

static void
wmDestroy()
{
    winaxlExit(0);
}

/* This is the entry point of every PFC application.
 * Main event handlers should be declared here.
 */
void
pfcMain()
{
    pfcSetApplicationName(COMPILER_NAME);

    /* Multiple instances not allowed */
    pfcSetMultipleInstances(FALSE);
    
    cmdLine = pfcCommandLine();
    
                /* Messages             Handlers */
    pfcMainEvent(wm_Compiler_Start,     callCompiler);
    pfcMainEvent(wm_Destroy,            wmDestroy);
    pfcMainEvent(wm_Close,              wmDestroy);
    pfcMainEvent(wm_Create,             wmCreate);
    pfcMainEvent(wm_Size,               wmSize);        
}


