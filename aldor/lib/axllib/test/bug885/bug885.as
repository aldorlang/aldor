--* From SMWATT%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Tue Oct 18 19:54:22 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA19315; Tue, 18 Oct 1994 19:54:22 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 4077; Tue, 18 Oct 94 19:54:28 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.SMWATT.NOTE.VAGENT2.3925.Oct.18.19:54:27.-0400>
--*           for asbugs@watson; Tue, 18 Oct 94 19:54:28 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 3921; Tue, 18 Oct 1994 19:54:27 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com
--*    (IBM VM SMTP V2R3) with TCP; Tue, 18 Oct 94 19:54:27 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA19323; Tue, 18 Oct 1994 19:44:08 -0400
--* Date: Tue, 18 Oct 1994 19:44:08 -0400
--* From: smwatt@watson.ibm.com (Stephen Watt)
--* X-External-Networks: yes
--* Message-Id: <9410182344.AA19323@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [2] Dependent lookup broken

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: asharp -Fx ns.as
-- Version: 0.37.0
-- Original bug file name: ns.as

--+ ./ns
--+ 3
--+ (3/1)
--+ Looking in Ratio(Integer) for * with code 367510736
--+ Export not found
#include "axllib"

NumberSorts == add {
    Nat == Integer;
    Rat == Ratio Nat;

    rat(a: Nat, b : Nat): Rat == a / b;
    num(r: Rat): Nat == numer r;
    den(r: Rat): Nat == denom r;
}

import from NumberSorts;

n: Nat := 1 + 1 + 1;
print << n << newline;

--r: Rat := rat(n - 1, n + 1);
r: Rat := 1 + 1 + 1;
print << r << newline;

rr: Rat := n * r;
print << rr << newline;
