--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Aug 23 12:07:57 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA20079; Tue, 23 Aug 1994 12:07:57 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 1845; Tue, 23 Aug 94 12:08:01 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.TEKE.NOTE.YKTVMV.2495.Aug.23.12:08:00.-0400>
--*           for asbugs@watson; Tue, 23 Aug 94 12:08:00 -0400
--* Received: from piger.matematik.su.se by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Tue, 23 Aug 94 12:07:57 EDT
--* Received: by piger.matematik.su.se (AIX 3.2/UCB 5.64/4.03)
--*           id AA10194; Tue, 23 Aug 1994 17:52:02 -0500
--* Date: Tue, 23 Aug 1994 17:52:02 -0500
--* From: teke@piger.matematik.su.se (Torsten Ekedahl)
--* Message-Id: <9408232252.AA10194@piger.matematik.su.se>
--* To: asbugs@watson.ibm.com
--* Subject: [5] Incorrect inheritance

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: none
-- Version: 0.36.5
-- Original bug file name: hest.as

--+ The program prints out
--+ list()
--+ which shows that FM inherits << from List E instead of from the
#include "axllib"



FC(E:BasicType): Category == BasicType with
{
    default
    {
   (p : TextWriter) << (x:%) : TextWriter ==
   {
        import from Integer;
        p << 1;
        }
  }
}



FM(E:BasicType): FC(E) == List E add;

MO ==> FM String;

import from MO;
import from SingleInteger;
import from Integer;
x : MO := sample;
print << x << newline;
