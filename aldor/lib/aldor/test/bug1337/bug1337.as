--* From hemmecke@risc.uni-linz.ac.at  Sun Apr 28 22:51:52 2002
--* Received: from welly-4.star.net.uk (welly-4.star.net.uk [195.216.16.162])
--* 	by nag.co.uk (8.9.3/8.9.3) with SMTP id WAA21826
--* 	for <ax-bugs@nag.co.uk>; Sun, 28 Apr 2002 22:51:43 +0100 (BST)
--* From: hemmecke@risc.uni-linz.ac.at
--* Received: (qmail 16527 invoked from network); 28 Apr 2002 21:51:11 -0000
--* Received: from 6.star-private-mail-12.star.net.uk (HELO smtp-in-6.star.net.uk) (10.200.12.6)
--*   by 204.star-private-mail-4.star.net.uk with SMTP; 28 Apr 2002 21:51:11 -0000
--* Received: (qmail 18131 invoked from network); 28 Apr 2002 21:51:11 -0000
--* Received: from mail17.messagelabs.com (HELO server-21.tower-17.messagelabs.com) (62.231.131.67)
--*   by smtp-in-6.star.net.uk with SMTP; 28 Apr 2002 21:51:11 -0000
--* X-VirusChecked: Checked
--* Received: (qmail 27503 invoked from network); 28 Apr 2002 21:50:59 -0000
--* Received: from kernel.risc.uni-linz.ac.at (193.170.37.225)
--*   by server-21.tower-17.messagelabs.com with SMTP; 28 Apr 2002 21:50:59 -0000
--* Received: from enceladus.risc.uni-linz.ac.at (enceladus.risc.uni-linz.ac.at [193.170.37.81])
--* 	by kernel.risc.uni-linz.ac.at (8.11.6/8.11.6) with ESMTP id g3SLp5O22650;
--* 	Sun, 28 Apr 2002 23:51:05 +0200
--* Message-ID: <XFMail.20020428235105.hemmecke@risc.uni-linz.ac.at>
--* X-Mailer: XFMail 1.4.4 on Linux
--* X-Priority: 3 (Normal)
--* Content-Type: text/plain; charset=us-ascii
--* Content-Transfer-Encoding: 8bit
--* MIME-Version: 1.0
--* In-Reply-To: <XFMail.20010919130402.hemmecke@risc.uni-linz.ac.at>
--* Date: Sun, 28 Apr 2002 23:51:05 +0200 (CEST)
--* Sender: hemmecke@risc.uni-linz.ac.at
--* To: ax-bugs@nag.co.uk
--* Subject: [4]Problem with THIS domain qualification

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: aldor -V -DMOD -fao=mod.ao xxx.as; aldor -DPOL xxx.as
-- Version: Aldor version 1.0.0 for LINUX(gcc-2_96)
-- Original bug file name: xxx.as

-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- Date: 27-APR-2002
-- Aldor version 1.0.0 for LINUX(gcc-2_96)
-- Subject: [4]Problem with THIS domain qualification

-- The code actually consist of 2 files. Putting everything in one
-- file does not give any error message. So
--   aldor -V -DMOD -DPOL xxx.as
-- will compile just fine.

-- The problem occurs when compiling with
--   aldor -V -DMOD -fao=mod.ao xxx.as; aldor -V -DPOL xxx.as
-- The error message is 

--:"xxx.as", line 81:         local vari(s: String): % == variable s;
--"                   ....................................^
--:[L81 C37] #1 (Error) No one possible return type satisfies the
--:context type.
--:  These possible return types were rejected:
--:          -- %
--:  The context requires an expression of type %.

-- and is not very helpful.

-- Interestingly, the code does compile with
--   aldor -V -DMOD -DC1 -fao=mod.ao xxx.as; aldor -V -DPOL xxx.as
-- and
--   aldor -V -DMOD -DC2 -fao=mod.ao xxx.as; aldor -V -DPOL xxx.as

-- In my understanding `variable' and `variable $ %' should have the
-- same meaning in this context. At least the compiler should say
-- something about the correct use of `$ %' qualification.

-- Of course the question arises: what happens when for some reason
-- one must qualify a function by `$ %' since there is some other
-- function with exactly the same signature visible in that context.
  
#if MOD
-- First file
#include "aldor"

CalixExpressionReader(
    P: Type,
    embed: String -> P
): with {
        get: List String -> List P;
} == add {
        get(s: List String): List P == [];
}

CalixExpressionReader(
    A: with {variable: String -> %}
): with {
        get: List String -> List A;
} == CalixExpressionReader(A, variable $ A) add;

define CalixPolynomialCategory(
    R: Type,
    T: Type
): Category == with {
        0: %;
        coerce: List String -> List %;
        variable: String -> %;
    default {
        import from R, T;
        variable(s: String): % == 0;
        coerce(ls: List String): List % == {
#if C1
                get(ls) $ CalixExpressionReader(%);
#elseif C2
                get(ls) $ CalixExpressionReader(%, variable);
#else
                get(ls) $ CalixExpressionReader(%, variable $ %);
#endif
        }
    }
}

#endif

#if POL
-------------------------------------------------------------------
-- Second file
-------------------------------------------------------------------
#if MOD
#else
#include "aldor"
#library MOD "mod.ao"
import from MOD;
#endif

CalixPolynomials(
    R: Type,
    T: Type
): CalixPolynomialCategory(R, T) with {} == add {
        Rep == List T;
        import from Rep, R, T;
        0: % == per [];
        local vari(s: String): % == variable s;

} -- end CalixPolynomials

#endif


