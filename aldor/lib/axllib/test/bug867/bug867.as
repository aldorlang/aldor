--* From DOOLEY%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Fri Sep 16 16:35:15 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA17110; Fri, 16 Sep 1994 16:35:15 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 0023; Fri, 16 Sep 94 16:35:16 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.DOOLEY.NOTE.VAGENT2.9553.Sep.16.16:35:15.-0400>
--*           for asbugs@watson; Fri, 16 Sep 94 16:35:16 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 9547; Fri, 16 Sep 1994 16:35:14 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Fri, 16 Sep 94 16:35:14 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA05937; Fri, 16 Sep 1994 16:29:36 -0400
--* Date: Fri, 16 Sep 1994 16:29:36 -0400
--* From: dooley@watson.ibm.com (Sam Dooley)
--* Message-Id: <9409162029.AA05937@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [5] Error message should say 'Expected type Cat0(Dom3)'.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: none
-- Version: 0.37.0
-- Original bug file name: catself0.as

#include "axllib"

Cat0 (Dom0: BasicType) : Category == BasicType;

Cat1: Category == Cat0 % with;

Cat2 (Dom2: Cat1, Dom: Cat0 Dom2) : Category == BasicType;

Cat3 (Dom3: Cat1) : Category == Cat2(Dom3, Dom3);
