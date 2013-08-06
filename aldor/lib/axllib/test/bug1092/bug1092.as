--* Received: from fred.rz.uni-leipzig.de by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA23780; Fri, 16 Aug 96 19:25:44 BST
--* Received: (from mam89aha@localhost) by fred.rz.uni-leipzig.de (8.6.12/8.6.9) id UAA08479 for ax-bugs@nag.co.uk; Fri, 16 Aug 1996 20:19:18 +0200
--* Date: Fri, 16 Aug 1996 20:19:18 +0200
--* From: "Ralf_Hemmecke_Sekt._Mathe" <mam89aha@fred.rz.uni-leipzig.de>
--* Message-Id: <199608161819.UAA08479@fred.rz.uni-leipzig.de>
--* To: ax-bugs
--* Subject: [1] Compiler option -V

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -fo idebug.as
-- Version: AXIOM-XL version 1.1.7 for LINUX
-- Original bug file name: idebug.as

-----------------------------------------------------------------------
-- 16-AUG-96
-----------------------------------------------------------------------
-- Author: Ralf Hemmecke, University of Leipzig
-- Date: 16-AUG-96
-- AXIOM-XL version 1.1.7 for LINUX
-- Subject: Compiler option -V

-- Compile this file with
--   axiomxl -fo idebug.as
-- and
--   axiomxl -V -fo idebug.as

-- Whereas the first way results in

--:Program fault (segmentation violation).#1 (Error) Program fault (segmentation violation).
--:#2 (Warning) Removing file `idebug.c'.

-- the second compilation works fine.

-- Another strange behaviour is: if one deletes or comments 
-- the lines between ====== , the program fault does not
-- occur. 

-- It will be important to cut at the line below. If one includes 
-- the text above, the fault does not occur. Well, may not occur.
-- It changed with the lenght (or something else -- I do not know)
-- of the text that explains the fault. (I changed it more than once.)

----- cut here (important) ----------------------------------------
-- This package is an extended version of the DebugPackage of Axiom-XL
-- Version 1.1.3
--
-- Author: Ralf Hemmecke, University of Leipzig

#include "axllib"
-- NB: Do not add-inherit from this package

macro {
  SI == SingleInteger;
  S  == String;
  B  == Boolean;
}

+++ InteractiveDebugPackage stops the program (when compiled with -Wdebug)
+++ after each entry and exit of a function and each assignment.
+++ One can skip a whole function if one presses "r" (return) at the prompt.
+++ This traces the program quietly until the current function exits.
+++

InteractiveDebugPackage: with {
  -- give an abbreviation function for output of long type names
  debugActivate: ()->();
--=====================================
  setAbbreviationFunction: (S->S) -> ()
--=====================================
} == add {
  import from S, List DomainName;
  import {fiSetDebugVar: Pointer -> ()} from Foreign;  -- from gf_rtime.
  import from StandardIO, InFile,FileName;
  DbgTypes ==> 'assign,entry,exit,dominit';
  readln ==> rightTrim(readline! stdin,newline);

  -- local state
  activated := false;
  inDebug   := false;
  output: TextWriter := print;
  dbgOut: TextWriter := print;
  oufName:S := ""; -- initially we have stdout
  ouf: OutFile; -- there goes the output
  functionDepth:SI := 0;
    -- So we know where to stop
  canOutput := true;
    -- only when that is true will the IDP prompt appear
  abbreviationTable: HashTable(S,S) := table();

  returnTo:B := false; -- if true do not collect the stack
  runTo:B := false; -- if true, the runToFunction will be used
  runToFunction:S := "";

  functionStack:List S := ["MAIN"];
  id(s:S):S == s;
  userAbbreviation:(S->S) := id;

--=====================================
  setAbbreviationFunction(abbr:S->S):() == {
    free userAbbreviation := abbr;
    dbgOut << "incorporating user abbreviations" << newline;
  }
--=====================================

  abbreviate(s:S):S == {
    s := userAbbreviation s;
    (b,s) := search(abbreviationTable,s,s);
    s
  }

  tName(T) ==> dName(typeName T);
  dName(dn: DomainName):S == {
    default s:S;
    type dn = ID => abbreviate name dn;
    type dn = OTHER => "#";
    if type dn = APPLY 
      then {s:=concat(dName(first args dn),"("); t:=rest args dn}
      else {s:="(";                              t:=tuple dn};
    empty? t => concat(s,")");
    while not empty? rest t repeat {
      s := concat(s,dName first t,",");
      t := rest t;
    }
    abbreviate concat(s,dName first t,")")
  }

  local l(i:SI):S == { -- formatted output of i (leading blanks)
    free functionStack;

    f(i:SI,width:SI):S == { -- i will be converted and right adjusted,
      s:S := new(10); -- we assume 10 chars are enough
      format(i,s,1)$Format;
      i := if #s >= width then 0 else width-#s;
      concat(new(i,char(" ")$Character),s)
    }

    concat(f(i,3)," ",f(#functionStack,3));
  }

--------------------------------------------------------------------
  debugActivate(): () == {
    free activated;
    -- only do this once.
    if (not activated) then fiSetDebugVar(doDebug pretend Pointer);
    activated := true;
  }

  -- The real thing
  --
  doDebug(kind:DbgTypes,name:S,line:SI,T:Type,v:Pointer,f:Pointer):() == {
    free  inDebug, canOutput;
    inDebug     => return;
    inDebug := true;
    if      kind=entry  then doEntry (name,line,T,v pretend B)
    else if kind=exit   then doExit  (name,line,T,v pretend T,f pretend B)
    else if kind=assign then doAssign(name,line,T,v pretend T,f pretend B)
    else output << "unknown debug type" << newline;
    if canOutput then doPause(kind,name,line,T,v,f);
    inDebug := false;
    return;
  }

----------------------------------------------------------------
  handleAbbreviation(help:B,
    kind:DbgTypes,name:S,line:SI,T:Type,v:Pointer,f:Pointer):B == {
    if help then {
      dbgOut << "a (abbreviate)" << newline
             << "  Modifiy constructor abbreviations." << newline;
    }else{
    ------------------------------------------------------------  
      if (kind=exit  and (f pretend B)) or kind=assign  or 
         (kind=entry and (v pretend B)) then {
        dbgOut << "Abbreviate the name" << newline
          << tName(T) << newline
          << "or enter another name:" << newline;
        s := readln; if s = "" then s := tName(T);
        dbgOut 
          << "Abbreviate this by (type a single __ if you want to delete): " 
          << newline;
        t:S := readln;
        if t = "__" then {
          (b,t) := search(abbreviationTable,s,s);
          if not b then {
            dbgOut << "There is no abbreviation for" << newline
              << s << newline
              << "that could be deleted." << newline
              << "Possible values are:" << newline;
            for c in abbreviationTable repeat {-- (key,value)
              (t,s) := c;
              dbgOut << t << newline; -- the key
            }
          }else{
            drop!(abbreviationTable,s);
          }
        }else if t ~= "" then abbreviationTable.(s) := t
        else dbgOut << "nothing done" << newline;
      }else{
        dbgOut << "This option is currently not available." << newline;
      }
    }
    true;
  }

----------------------------------------------------------------
  handleGoto(help:B,
    kind:DbgTypes,name:S,line:SI,T:Type,v:Pointer,f:Pointer):B == {
    free oufName,ouf,output,runTo,runToFunction,canOutput;
    if help then {
      dbgOut << "g (goto)" << newline
             << "  Runs the program till entering a named function." 
             << newline;
    }else{
      dbgOut << "Enter the function's name where to stop." << newline;
      if (s := readln) ~= "" then {
        runToFunction := s;
        runTo := true;
        canOutput := false;
      }
    }
    false;
  }
----------------------------------------------------------------
  handleOutput(help:B,
    kind:DbgTypes,name:S,line:SI,T:Type,v:Pointer,f:Pointer):B == {
    free oufName,ouf,output;
    if help then {
      dbgOut << "o (output)" << newline
             << "  Redirect output streams." << newline;
    }else{
    ------------------------------------------------------------  
      dbgOut << "output file: ";
      if (s:=readln) = "" then 
        dbgOut << "nothing done" << newline
      else{
        if oufName ~= "" then close(ouf);
        oufName := s;
        ouf := open filename oufName;
        output := writer ouf;
      }
    }
    true
  }

----------------------------------------------------------------
  handleQuit(help:B,
    kind:DbgTypes,name:S,line:SI,T:Type,v:Pointer,f:Pointer):B == {
    if help then {
      dbgOut << "q (quit)" << newline;
    }else{
    ------------------------------------------------------------  
      dbgOut << "Type exit if you really want to quit. ";
      s := readln;
      if s="exit" then error "program aborted by user";
    }
    true
  }

----------------------------------------------------------------
  handleReturn(help:B,
    kind:DbgTypes,name:S,line:SI,T:Type,v:Pointer,f:Pointer):B == {
    free returnTo, canOutput, functionDepth;
    if help then {
      dbgOut << "r (return)" << newline
             << "  Stops tracing until current function returns." <<newline;
    }else{
    ------------------------------------------------------------  
      functionDepth := 1;
      returnTo := true;
      canOutput := false;
    }
    false;
  }
----------------------------------------------------------------
  handleStack(help:B,
    kind:DbgTypes,name:S,line:SI,T:Type,v:Pointer,f:Pointer):B == {
    free functionStack;
    if help then {
      dbgOut << "s (stack)" << newline
             << "  Prints call stack." <<newline;
    }else{
    ------------------------------------------------------------  
      for x in reverse functionStack repeat dbgOut << x << newline;
    }
    true
  }

  doPause(kind:DbgTypes,name:S,line:SI,T:Type,v:Pointer,f:Pointer): () == {
    import from Character;
    options := "agoqrs?";
    functions:List ((B,DbgTypes,S,SI,Type,Pointer,Pointer)->B) :=
      [handleAbbreviation,
       handleGoto,
       handleOutput,
       handleQuit,
       handleReturn,
       handleStack];
    local i:SI:=1; 
    len := #options;
    while (
      dbgOut << ">> ";
      s := readln;
      s="" => false;
      for free i in 1..len repeat {if options.i=s.1 then break};
      if i=len then {-- help
        dbgOut << "? (help)" <<newline
               << "h (help)" <<newline;
        for fn in functions repeat 
          fn(true,kind,name,line,T,v,f); --print help info
        true
      }else{
        fn := functions.i;
        fn(false,kind,name,line,T,v,f);
      }
    ) repeat {}
  }

  -- f is true for parameter initialisations,
  -- false for ordinary assignments.
  doAssign(name:S, line:SI, T:Type, v:T, f:B):() == {
    free canOutput;
    if canOutput then {
      name := if f then concat("! ",name) else concat(": ", name);
      output << l(line) << name << ": " << tName(T) << " := ";
      outputValue(T, v);
      output << newline;
    }
  }

  -- v indicates if the exporter was valid
  -- T is the exporter, if v is true.
  doEntry(name:S, line:SI, T:Type, v:B): () == {
    free canOutput, functionDepth, functionStack, returnTo;
    free runTo, runToFunction;
    if runTo and runToFunction=name then {canOutput:=true; runTo:=false}
    if not returnTo then {
      if v then name := concat(name," $ ",tName(T));
      functionStack := cons(name,functionStack);
      if canOutput then output << l(line) << "{ " << name << newline;
    }
    functionDepth := functionDepth+1;
  }

  -- f false indicates that T and v are not bone fide objects
  -- doExit1 is necessary as we don't know if T is a type at this point.
  doExit(name:S, line:SI, T:Type, v:T, f:B):()=={
    free canOutput, functionDepth, functionStack, returnTo;
    if returnTo then {
      not (canOutput := ((functionDepth := functionDepth -1) = 0)) => return;
      returnTo := false;
    }
    if canOutput then {
      if empty? functionStack then 
        functionStack := ["function stack underflow"];
      output << l(line) << "} " << first(functionStack);
      if f then doExit1(T, v);
      output << newline;
    }
    functionStack := rest functionStack;
  }

  doExit1(T: Type, v: T): () == {
    output << ": " << tName(T) << " is: ";
    outputValue(T, v);
  }

  -- finding an appropriate output routine
  outputValue(T: Type, v: T): () == {
    T has (with { dbgPrint: ((TextWriter, %) ->TextWriter) })
      => dbgPrint(output, v);
    T has BasicType => output << v;
    output << "??";
  }
}

