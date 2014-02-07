--* From postmaster%watson.vnet.ibm.com@yktvmh.watson.ibm.com  Sat May  7 20:48:31 1994
--* Received: from yktvmh.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA22855; Sat, 7 May 1994 20:48:31 -0400
--* Received: from watson.vnet.ibm.com by yktvmh.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 8518; Sat, 07 May 94 20:48:40 EDT
--* Received: from YKTVMH by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.RMC.NOTE.VAGENT2.7903.May.07.20:48:40.-0400>
--*           for asbugs@watson; Sat, 07 May 94 20:48:40 -0400
--* Received: from YKTVMH by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 7901; Sat, 7 May 1994 20:48:39 EDT
--* Received: from watson.ibm.com by yktvmh.watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Sat, 07 May 94 20:48:39 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA33506; Sat, 7 May 1994 20:46:11 -0400
--* Date: Sat, 7 May 1994 20:46:11 -0400
--* From: rmc@watson.ibm.com
--* Message-Id: <9405080046.AA33506@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [2] Zero division is OK????? [huh.as][35.0]

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Check that division by integer zero throws an exception
-- note there's no check on the nature of the exception.
#pile
#include "axllib"

import from Integer
flg := true
try
  print << "For large values of " << (1 quo 0) << " I get confused." << newline
  flg := false

catch E in {

}

if not flg then never
