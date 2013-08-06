--* From DOOLEY%WATSON.vnet.ibm.com@yktvmv.watson.ibm.com  Wed Aug 31 09:17:10 1994
--* Received: from yktvmv-ob.watson.ibm.com by watson.ibm.com (AIX 3.2/UCB 5.64/930311)
--*           id AA15669; Wed, 31 Aug 1994 09:17:10 -0400
--* Received: from watson.vnet.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with BSMTP id 5287; Wed, 31 Aug 94 09:17:14 EDT
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id <A.DOOLEY.NOTE.VAGENT2.7877.Aug.31.09:17:13.-0400>
--*           for asbugs@watson; Wed, 31 Aug 94 09:17:13 -0400
--* Received: from YKTVMV by watson.vnet.ibm.com with "VAGENT.V1.0"
--*           id 7873; Wed, 31 Aug 1994 09:17:12 EDT
--* Received: from watson.ibm.com by yktvmv.watson.ibm.com (IBM VM SMTP V2R3)
--*    with TCP; Wed, 31 Aug 94 09:17:12 EDT
--* Received: by watson.ibm.com (AIX 3.2/UCB 5.64/920123)
--*           id AA18309; Wed, 31 Aug 1994 09:11:28 -0400
--* Date: Wed, 31 Aug 1994 09:11:28 -0400
--* From: dooley@watson.ibm.com (Sam Dooley)
--* Message-Id: <9408311311.AA18309@watson.ibm.com>
--* To: asbugs@watson.ibm.com
--* Subject: [4] Default types on parameters confuse identification of exported symbols.

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


-- Command line: asharp bug1.as
-- Version: 0.36.4
-- Original bug file name: bug1.as

#include "axllib"
#pile

+++ Comments

DynamicSetCategory: Category == BasicType with

  roughEqual?: (%,%) -> Boolean
    ++ If roughEqual?(reduce(x),reduce(y)) then
    ++ x=y in the current case, without splitting.
    ++ Otherwise nothing can be said.
  reduce: % -> %
    ++ reduce(x) gives the reduced value of x
    ++ Note: reduce(reduce(x))=reduce(x), BUT for example if
    ++ y:=reduce(x); ...; z:=reduce(y); then z may be different from y


+++ Coments.

DynamicRingCategory: Category == Join(DynamicSetCategory,Ring) with

  roughExquo: (%,%) -> %
    ++ roughExquo(x1,x2) returns y such that x1=y*x2
    ++ Error if such a y does not exist,
    ++ and even often when it does exist
  roughZero?: % -> Boolean
  power: (%,Integer) -> %
    ++ power(x,nn) returns x ** nn
    ++ power(0,0) returns 1
    ++ Note: used to avoid confusion with ** : (%,I) -> %
    ++ which is a splitting operation when K has DynamicFieldCategory
    ++ NOTE if nn < 0 error from A#
  * : (Integer,%) -> %
  one? : % -> Boolean
  recip : % -> Union(%,'failed')



  default

    default x,x1,x2: %
    default n: Integer
    default ns: SingleInteger

    -- define

    -- no splitting

    roughEqual?(x1,x2):Boolean == roughZero?(x1-x2)

    power(x,n):% ==
      zero? n => 1
      (n = 1) => x
      n < 0 => error "Negative argument in power"
      x1:= power(x*x,n quo 2)
      odd?(n) => x * x1
      x1

    (coerce n):% == n * 1

    (coerce ns):% == (ns::Integer) * 1

    (x ^ n):% == power(x,n)

    (x1 = x2):Boolean == zero?(x1-x2)

    (one? x):Boolean == zero?(x-1)

