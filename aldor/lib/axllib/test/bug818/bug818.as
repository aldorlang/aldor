--* From postmaster%watson.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Aug 23 08:25:07 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA19346; Tue, 23 Aug 1994 08:25:07 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 2517; Tue, 23 Aug 94 08:25:11 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.TEKE.NOTE.YKTVMV.4451.Aug.23.08:25:10.-0400>
--*           for asbugs@watson; Tue, 23 Aug 94 08:25:11 -0400
--* Received: from piger.matematik.su.se by watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Tue, 23 Aug 94 08:25:10 EDT
--* Received: by piger.matematik.su.se (AIX 3.2/UCB 5.64/4.03)
--*           id AA09109; Tue, 23 Aug 1994 14:09:10 -0500
--* Date: Tue, 23 Aug 1994 14:09:10 -0500
--* From: teke@piger.matematik.su.se (Torsten Ekedahl)
--* Message-Id: <9408231909.AA09109@piger.matematik.su.se>
--* To: asbugs@watson.ibm.com
--* Subject: [5] Problem with Tuple as a type

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: asharp -G run hest.as
-- Version: 0.36.5 for AIX RS/6000
-- Original bug file name: hest.as

--+ $ asharp -G run hest.as
--+ Looking in skriv(a==<value>, f==<value>) for skr with code 673701613
--+ Export not found
#include "axllib"

import from Integer;

skriv(a : Tuple Type, f:a -> Integer): with
{
    skr : a -> Integer;
    }
== add
{
    skr(s:a):Integer == {
       f(s) + 1;
    }
}

import from SingleInteger;

SK ==> skriv((Integer,Integer),(m:Integer,n:Integer):Integer +-> { print << "m: " << m << " n: " << n << newline; 100*m + 10*n});

print << skr(22,1)$SK << newline;
