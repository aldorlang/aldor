--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Aug 23 10:32:52 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA24564; Tue, 23 Aug 1994 10:32:52 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 7095; Tue, 23 Aug 94 10:32:54 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.TEKE.NOTE.YKTVMV.9083.Aug.23.10:32:53.-0400>
--*           for asbugs@watson; Tue, 23 Aug 94 10:32:54 -0400
--* Received: from piger.matematik.su.se by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Tue, 23 Aug 94 10:32:53 EDT
--* Received: by piger.matematik.su.se (AIX 3.2/UCB 5.64/4.03)
--*           id AA10120; Tue, 23 Aug 1994 16:16:58 -0500
--* Date: Tue, 23 Aug 1994 16:16:58 -0500
--* From: teke@piger.matematik.su.se (Torsten Ekedahl)
--* Message-Id: <9408232116.AA10120@piger.matematik.su.se>
--* To: asbugs@watson.ibm.com
--* Subject: [5] Overriding doen't work

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: none
-- Version: 0.36.5
-- Original bug file name: hest.as

--+ The exported function name is called twice, once through
--+ another function and once directly. When overridden the direct
--+ call calls the overridden function and the indirect doesn't. Output
--+ of program is
--+ ff
--+ ff
--+ ff
--+ gg
--+ and should be
--+ ff
--+ gg
--+ ff
--+ gg
#include "axllib"

ff: with { p : TextWriter; name: () -> String;} ==
 add {
   name():String == "ff";
   p : TextWriter == print << name() << newline;
};

gg:  with { p : TextWriter; name: () ->String; } ==
ff add {  name():String == "gg";  }

p$ff;
p$gg;
print << name()$ff << newline;
print << name()$gg << newline;

