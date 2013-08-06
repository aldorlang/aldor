--* From BMT%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Thu Jun 22 13:59:01 1995
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA19140; Thu, 22 Jun 1995 13:59:01 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 8063; Thu, 22 Jun 95 13:59:01 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.BMT.NOTE.VAGENT2.4175.Jun.22.13:58:58.-0400>
--*           for asbugs@watson; Thu, 22 Jun 95 13:59:00 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.01 on VAGENT2"
--*           id 0597; Thu, 22 Jun 1995 13:58:58 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com
--*    (IBM VM SMTP V2R3) with TCP; Thu, 22 Jun 95 13:58:57 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/900524)
--*           id AA17025; Thu, 22 Jun 1995 14:00:15 -0400
--* Date: Thu, 22 Jun 1995 14:00:15 -0400
--* From: bmt@watson.ibm.com
--* X-External-Networks: yes
--* Message-Id: <9506221800.AA17025@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [2] scanBint returns wrong buffer position

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: asharp -grun bugscan.as
-- Version: 1.0.9
-- Original bug file name: bugscan.as

#include "axllib"

local int:Integer;
pos:SingleInteger:=1;
buf:String := "3xyz";
(int, pos) := scan(buf, pos)$Format;
print << "scanned integer is: " << int << newline;
print << "new buffer pos is: " << pos << newline;
