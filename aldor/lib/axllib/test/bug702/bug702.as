--* From SMWATT%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Thu Jun 16 19:15:03 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA16585; Thu, 16 Jun 1994 19:15:03 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 0843; Thu, 16 Jun 94 19:15:05 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.SMWATT.NOTE.VAGENT2.9773.Jun.16.19:15:04.-0400>
--*           for asbugs@watson; Thu, 16 Jun 94 19:15:04 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 9767; Thu, 16 Jun 1994 19:15:04 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com
--*    (IBM VM SMTP V2R3) with TCP; Thu, 16 Jun 94 19:15:03 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/900524)
--*           id AA20094; Thu, 16 Jun 1994 19:18:05 -0400
--* Date: Thu, 16 Jun 1994 19:18:05 -0400
--* From: smwatt@watson.ibm.com (Stephen Watt)
--* X-External-Networks: yes
--* Message-Id: <9406162318.AA20094@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [7] print << x case y << newline parses ase (print << x) case (y << newline) [union1.as][x]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


--> testcomp
--> testgen l
--> testgen c
--> testrun
#pile

#include "axllib"

macro
	I == SingleInteger
	F == SingleFloat
	U == Union(first : I, second : F)

import from Boolean
import from I
import from F
import from U

x : U := [ 3 ]
y : U := union( 2.71828 )

print << (x case first)  <<newline
print << (x case second) <<newline
print << (y case first ) <<newline
print << (y case second) <<newline
print << x.first       <<newline
print << y.second      <<newline
print << y.first       <<newline
