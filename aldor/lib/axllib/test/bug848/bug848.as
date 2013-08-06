--* From SMWATT%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Aug 30 21:58:09 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA17295; Tue, 30 Aug 1994 21:58:09 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 7907; Tue, 30 Aug 94 21:58:14 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.SMWATT.NOTE.VAGENT2.9057.Aug.30.21:58:13.-0400>
--*           for asbugs@watson; Tue, 30 Aug 94 21:58:13 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 9051; Tue, 30 Aug 1994 21:58:13 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com
--*    (IBM VM SMTP V2R3) with TCP; Tue, 30 Aug 94 21:58:13 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/900524)
--*           id AA27143; Tue, 30 Aug 1994 21:58:30 -0400
--* Date: Tue, 30 Aug 1994 21:58:30 -0400
--* From: smwatt@watson.ibm.com (Stephen Watt)
--* X-External-Networks: yes
--* Message-Id: <9408310158.AA27143@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [4][tinfer] Unable to package call from % in a "with"

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: asharp foo2.as
-- Version: current
-- Original bug file name: foo2.as

--+ "foo2.as", line 15:         h: X(1$%);
--+                     ..............^
--+ [L15 C15] #1 (Error) Have determined 0 possible types for the expression.
--+
#include "axllib"

import from Integer;

C == with {

        T: Type;

        1: T;

        Y: Integer -> Type;
        X: T -> Type;

        g: Y(1$Integer);
        h: X(1$%);
}
