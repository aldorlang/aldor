--* Received: from igw2.watson.ibm.com by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA05582; Thu, 13 Jun 96 19:16:44 BST
--* Received: from asharp.watson.ibm.com (asharp.watson.ibm.com [9.2.224.83]) by igw2.watson.ibm.com (8.7.4/8.7.1) with SMTP id OAA10356 for <ax-bugs@nag.co.uk>; Thu, 13 Jun 1996 14:11:33 -0400
--* Received: by asharp.watson.ibm.com (AIX 3.2/UCB 5.64/5/18/96)
--*           id AA20111; Thu, 13 Jun 1996 14:11:11 -0400
--* Date: Thu, 13 Jun 1996 14:11:11 -0400
--* From: "M.Hassner" <mhass@watson.ibm.com>
--* Message-Id: <9606131811.AA20111@asharp.watson.ibm.com>
--* To: ax-bugs
--* Subject: [3] Illegal instruction (caused by extend DoubleFloat)

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -Fx -Fc -laxldem UDbug.as
-- Version: 1.1.6
-- Original bug file name: UDbug.as



--mr1331, pulse  eq10.data


#include "axllib.as"
#include "axldem.as"
#pile

extend DoubleFloat:IntegralDomain == add
   unit?(x:%):Boolean == x ~= 0
   (exquo)(a:%,b:%):Partial % ==
       b = 0 => failed
       [a/b]

-- renamings
Integer ==> SingleInteger
Vector ==> Array
Float ==> DoubleFloat
NonNegativeInteger ==> SingleInteger

-- type abbreviations
VF==>Vector Float
vf==>Vector Float
VI==>Vector Integer
vi==>Vector Integer
lmf==>List Matrix Float
mf==> Matrix Float
msi==> Matrix Single Integer
lf==> List Float
lvi==> List Vector Integer
vvf==> Vector Vector Float
LDF==>List DoubleFloat
VDF==>Vector DoubleFloat
DFLOAT ==> DoubleFloat
NNI ==> SingleInteger
SI ==> SingleInteger

import from SingleInteger

intScheme ==> Record(numInt:SI, numIntp:SI, length:SI, offset:SI)

print << "starting up now" << newline

CLK1:VF:=[0.3011278680,0.4080646356,0.5317280335,0.6718961229,_
0.8279249493,0.9987585866,1.1829405,1.3786181900,1.583536716,_
1.7950219160]


CLK2:VF:=[2.00995973,2.224782855,2.435479111,2.637636622,2.826538848,_
2.997317335,3.145161913,3.26557765,3.354666702,3.409403747]


CLK3:VF:=[3.427868868,3.409403747,3.354666702,3.26557765,3.145161913,_
2.997317335,2.826538848,2.637636622,2.435479111, 2.224782855]


CLK4:VF:=[2.009959730,1.795021916,1.583536716,1.378618190,1.1829405,_
0.998758586,0.827924949,0.6718961229,0.5317280335,0.4080646356,_
0.301127868]

VectorConcatPackage(S:BasicType): with 
   concat: (Vector S, Vector S) -> Vector S
   concat: (S, Vector S) -> Vector S
   concat: (Vector S, S) -> Vector S
   fill!: (Vector S, SingleInteger, Vector S) -> Vector S
   rest: Vector S -> Vector S
 == add
   fill!(v1:Vector S, pos:SingleInteger, v2:Vector S):Vector S ==
      pos := pos-1
      for i in 1..#v2 repeat v1(pos+i) := v2(i)
      v1

   concat(v1:Vector S, v2:Vector S):Vector S ==
      n1 := # v1
      zero? n1 => v2
      v3:Vector S := new(n1+#v2, v1.1)
      fill!(v3, 1, v1)
      fill!(v3, n1+1, v2)

   concat(s:S, v2:Vector S):Vector S ==
      v3:Vector S := new(#v2+1, s)
      fill!(v3, 2, v2)

   concat(v1:Vector S, s:S):Vector S ==
      v3:Vector S := new(#v1+1, s)
      fill!(v3, 1, v1)

   rest(v:Vector S):Vector S ==
      n1 := #v
      zero? n1 => error "rest of empty vector"
      zero?(n1:=n1-1) => empty()
      v3:Vector S := new(n1, v.2)
      for i in 1..n1 repeat v3(i) := v(i+1)
      v3

import from VectorConcatPackage DFLOAT


C12:=concat(CLK1,CLK2)
C34:=concat(CLK3,CLK4)
ONE:=concat(C12,C34)



vfo:VI:=[1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,_
         0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,_
         0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,_
         0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,1,0,1,0,0,0,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,_
         0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,_
         0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,_
         0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,_
         0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,_
         0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,1,0,0,1,0,1,0,0,0,_
         0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,1,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,_
         0,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,_
         0,0,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,0,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,_
         0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,_
         0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1]

print << "begin vfo"  << newline
print << vfo << newline

(i:SingleInteger) * (f:DFLOAT):DFLOAT == i::DFLOAT * f

clockEncoder(s:Vector Integer):VF==
  n:=#s
  sig:VF:=new(10*(n+4)+1,0)
  polarity:Integer:=-1
  for i in 1..n repeat
     if s.i=1 then
       polarity:=-polarity
       posONE:Integer:=10*(i-1)
       for j in 1..41 repeat
         sig(j+posONE):=sig(j+posONE)+polarity*ONE(j)
  sig

reduce(f:(DFLOAT,DFLOAT)->DFLOAT, g:Generator DFLOAT, i:DFLOAT):DFLOAT ==
   for x in g repeat i:=f(i,x)
   i

trapezInteg1(l:VDF, pos:SI, count:SI):DFLOAT ==
--    (reduce(+,[l(j) for j in pos..pos+count],0)
   sum:DFLOAT := 0
   for j in pos..pos+count repeat sum := sum + l(j)
   (sum - (0.5* (l(pos)+l(pos+count)))) / count::DFLOAT

    -- count represents samples per clock cycle
trapezIntegrateClock(l:VDF, count:SI):VF==
  n:=(#l) quo count
  tI:VF := new(n, 0)
  for i in 1..n repeat
    pos:=count*(i-1)+1
    tI(i) := trapezInteg1(l, pos, count)
  tI

-- following not correct yet
trapezIntegrateSymbol(l:VDF, count:SI, integ:intScheme):VF==
  n:=(#l) quo count
  tI:VF := new(n, 0)
  for i in 1..n repeat
    pos:=count*(i-1)+1
    tI(i) := trapezInteg1(l, pos, count)
  tI



ss1:Float:=(1/10.0)*reduce(+,generator CLK1,0)+(1/10.0)*CLK2.1-_
(1/10.0)*(1/2.0)*(CLK1.1+CLK2.1)
ss2:Float:=(1/10.0)*reduce(+,generator CLK2,0)+(1/10.0)*CLK3.1-_
(1/10.0)*(1/2.0)*(CLK2.1+CLK3.1)
ss3:Float:=(1/10.0)*reduce(+,generator CLK3,0)+(1/10.0)*CLK4.1-_
(1/10.0)*(1/2.0)*(CLK3.1+CLK4.1)
ss4:Float:=(1/10.0)*reduce(+,generator CLK4,0)-_
(1/10.0)*(1/2.0)*(CLK4.1+CLK4.11)


one:VF:=[ss1,ss2,ss3,ss4]

print << "one"  << newline

signalEncoder(b:Vector Integer):VF==
  n:=#b
  sig:VF:=new(n+3,0);  -- print << sig  << newline
  polarity:Integer:=-1
  for i in 1..n repeat
     if b.i=1 then
       polarity:=-polarity
       for j in 0..3 repeat
         sig(j+i):=sig(j+i)+polarity*one(j+1)
  sig




import from Vector Vector DFLOAT
import from Matrix DFLOAT

--FILTER


A:mf:=matrix(_
[[- 16.9122172223,46.3766747090,- 62.1300409541,55.1680,-31.2184]])


M:mf:= matrix([[0,0,0,0,1,1,1],_        --INTEGRATOR MATRIX
               [0,0,0,1,1,1,0],_
               [0,0,1,1,1,0,0],_
               [0,1,1,1,0,0,0],_
               [1,1,1,0,0,0,0]])


V:mf:=A*M


--NOISELESS READBACK SIGNAL OF 6500 BITS

import {
	fread: (Pointer, SingleInteger, SingleInteger, InFile) -> SingleInteger
} from Foreign C


readSingleFloatVec(fname:String, count:SingleInteger): PrimitiveArray SingleFloat ==
    import from InFile
    import from FileName
    vec:PrimitiveArray SingleFloat := new(count)
    file := open(filename fname)
    rcount := fread(vec pretend Pointer, 4, count, file)
    rcount < count => error "not enough points!"
    vec

prim2DFArray(pa:PrimitiveArray SingleFloat, count:SingleInteger):VF ==
    import from SingleFloat
    import from DFLOAT
    import from Machine
    df:VF := new(count,0)
    for i in 1..count repeat df(i) := convert(pa(i)::BSFlo)::DFLOAT
    df

NPOINTS := 65001
--SIGNAL:= readSingleFloatVec("b36.fb", NPOINTS)
--NOISE:= readSingleFloatVec("b36nse.fb", NPOINTS)

--import from NumberScanPackage DFLOAT


--SIG:=clockEncoder(vfo)
--print << "begin ReadData"  << newline
--SIG:= prim2DFArray(SIGNAL,NPOINTS);         --NOISELESS READBACK SIGNAL
--NSE := prim2DFArray(NOISE,NPOINTS);         --NOISY READBACK SIGNAL



scale16:DoubleFloat:=0.1584893192                       --10**(-16/20)
scale18:DoubleFloat:=0.12548925411                      --10**(-18/20)
scale20:DoubleFloat:=0.1                                --10**(-20/20)
scale22:DoubleFloat:=0.0794328234                       --10**(-22/20)
scale24:DoubleFloat:=0.0630957344                       --10**(-24/20)
scale26:DoubleFloat:=0.0501187233                       --10**(-26/20)
scale28:DoubleFloat:=0.0398107170                       --10**(-28/20)


--print << "begin NoiseScale"  << newline
--NoiseScale:VDF:=[scale24*NSE(i) for i in 1..#NSE];  --NOISE



--print << "begin NoiseAdd"  << newline
--SPN:VDF:=[SIG(i)+NoiseScale(i) for i in 1..#SIG];



--print << "begin TrapezIntegrate"  << newline
--SN:=trapezIntegrateClock(SPN,10);
--S:=trapezIntegrateClock(SIG,10);



DataParser(s:VF,d:NNI):vvf==
  d < 3 => error "illegalProcessor"
  StringInit:VF := new(d, 0)
  StringInit(d-1) := s.1
  StringInit(d) := s.2
  
  n:=#s
  DataStrings:vvf := new(n-1,StringInit)
  for i in 3..n repeat
     si:=s.i
     dStringInit:=rest(StringInit)
     Si:=concat(dStringInit,si)
     DataStrings(i-1):= Si
     StringInit:=Si
  DataStrings



--print << "begin DataParser"  << newline
--SP:=DataParser(S,7);
--print << "begin DataParserSN"  << newline
--SNP:=DataParser(SN,7);


DecisionFunctionGenerator(data:vvf,integrator:mf,filter:mf):VF==
  n:=#data
  v:VF:=new(n,0)
  for i in 1..n repeat
   datai:=data.i
   mi:mf:=matrix([datai])
   Processor:=filter*integrator
   di:= Processor*transpose mi
   v.i := di(1,1)
  v




--print << "begin DecisionFunctionGenerator" << newline
--DSNP:VF:=DecisionFunctionGenerator(SNP,M,A);
--DS:VF:=DecisionFunctionGenerator(SP,M,A);



--STATE VECTORS

--ST1: Vector Integer:=[0,0,0,0,0,0];
ST1:VI:=           [0,0,0,0,0,0,0,0];
ST2:VI:=           [0,1,0,0,0,0,0,0];
ST3:VI:=           [1,0,0,0,0,0,0,0];

--ST2: Vector Integer:=[0,0,0,0,1,0];
ST4:VI:=           [0,0,0,0,0,0,1,0];
ST5:VI:=           [0,1,0,0,0,0,1,0];
ST6:VI:=           [1,0,0,0,0,0,1,0];

--ST3: Vector Integer:=[0,0,0,1,0,0];
ST7:VI:=           [0,0,0,0,0,1,0,0];
ST8:VI:=           [0,1,0,0,0,1,0,0];
ST9:VI:=           [1,0,0,0,0,1,0,0];

--ST4: Vector Integer:=[0,0,1,0,0,0];
ST10:VI:=          [0,0,0,0,1,0,0,0];
ST11:VI:=          [0,1,0,0,1,0,0,0];
ST12:VI:=          [1,0,0,0,1,0,0,0];

--ST5: Vector Integer:=[0,0,1,0,1,0];
ST13:VI:=          [0,0,0,0,1,0,1,0];
ST14:VI:=          [0,1,0,0,1,0,1,0];
ST15:VI:=          [1,0,0,0,1,0,1,0];

--ST6: Vector Integer:=[0,1,0,0,0,0];
ST16:VI:=          [0,0,0,1,0,0,0,0];
ST17:VI:=          [0,1,0,1,0,0,0,0];
ST18:VI:=          [1,0,0,1,0,0,0,0];

--ST7: Vector Integer:=[0,1,0,0,1,0];
ST19:VI:=          [0,0,0,1,0,0,1,0];
ST20:VI:=          [0,1,0,1,0,0,1,0];
ST21:VI:=          [1,0,0,1,0,0,1,0];

--ST8: Vector Integer:=[0,1,0,1,0,0];
ST22:VI:=          [0,0,0,1,0,1,0,0];
ST23:VI:=          [0,1,0,1,0,1,0,0];
ST24:VI:=          [1,0,0,1,0,1,0,0];

--ST9: Vector Integer:=[1,0,0,0,0,0];
ST25:VI:=          [0,0,1,0,0,0,0,0];
ST26:VI:=          [0,1,1,0,0,0,0,0];

--ST10:Vector Integer:=[1,0,0,0,1,0];
ST27:VI:=          [0,0,1,0,0,0,1,0];
ST28:VI:=          [1,0,1,0,0,0,1,0];

--ST11:Vector Integer:=[1,0,0,1,0,0];
ST29:VI:=          [0,0,1,0,0,1,0,0];
ST30:VI:=          [1,0,1,0,0,1,0,0];

--ST12:Vector Integer:=[1,0,1,0,0,0];
ST31:VI:=          [0,0,1,0,1,0,0,0];
ST32:VI:=          [1,0,1,0,1,0,0,0];

--ST13:Vector Integer:=[1,0,1,0,1,0];
ST33:VI:=          [0,0,1,0,1,0,1,0];
ST34:VI:=          [1,0,1,0,1,0,1,0];



STATES:lvi:=[ST1,ST2,ST3,ST4,ST5,ST6,ST7,ST8,ST9,ST10,ST11,ST12,ST13,_
ST14,ST15,ST16,ST17,ST18,ST19,ST20,ST21,ST22,ST23,ST24,ST25,ST26,ST27,_
ST28,ST29,ST30,ST31,ST32,ST33,ST34]
nS:Integer:=#STATES



F1:vi:=[0,0,1]
F2:vi:=[-1,0,1]
F3:vi:=[0,0,0]
F4:vi:=[1,0,0]
F5:vi:=[0,1,0]


F:lvi:=[F1,F2,F3,F4,F5]




RECM ==> Record(Umat:Matrix Float, Vmat:Matrix Float)

SignalMatrix(puls:VF,integ:intScheme):RECM==
 length := integ.length
 offst := integ.offset
 numint := integ.numInt
 numintp := integ.numIntp
 p:=numintp+length-1+2       --length of past bits
 f:SI:=numint-numintp+1     --length of future and current
 n:=numint
 m:=p
 local U
 U:mf:=zero(n,m)$mf
 local V
 V:mf:=zero(n,f)$mf
 template:VF:=makeTemplate(puls,integ)
 for i in 1..n repeat
   pos := m-(n-i)- #template
   for j in 1..#template repeat
      pos + j > 0 => U(i,pos+j):=template(j)
 for i in 1..n repeat
   for j in 1..#template repeat
       pos := i+j-1
       pos <= f => V(i,i+j-1) := template(j)
 [U,V]


makeTemplate(puls:VF,integ:intScheme):VF ==
 a:=puls(1)
 b:=puls(2)
 b:=puls(3)
 a:=puls(4)
 template:VF:=[a,a+b,a+2*b,a+2*b,a+b,a]


print << "begin SigMat" << newline
sigmat:RECM:=SignalMatrix(one,[6,4,3,1]$intScheme)
print << sigmat.Umat << newline
print << sigmat.Vmat << newline




--rb36nse:vf:=[1.0,0.43,-0.62,-0.652,-0.196,0.0129,0.00717,-0.00708,0.0]


r24:vf:=_
[0.15299843178717221, 0.062359627045052067, -0.061387150803799459,_
-0.06515592107927938, -0.017336446017199896, 0.0020690496738138692,_
 0.0011722773696427756, -0.00063551229134023435,0.0,0.0]


r:vf:=[r24.i/r24.1 for i in 1..#r24]



covMatrix(r:vf):mf==
  n:=#r
  local C
  C:mf:=new(n,n,0)$mf
  for i in 1..n repeat
   for j in 1..n repeat
     ij:=abs(i-j) +1
     C(i,j):=r(ij)
  C


C:=covMatrix(r)




covInt(corr:VF,integ:intScheme):mf  ==
  numint := integ.numInt
  length := integ.length
  offset := integ.offset
  c:mf:=covMatrix(corr)
  NN:=new(numint,numint,0)$mf
  vec:vf:=new(#corr,0)
  for j in 1..length repeat
    vec(j):=1
  vc:=vec*c
  tvec:vf:=new(numint,0)
  for i in 1..#tvec repeat
      sum:DFLOAT := 0
      for j in 1..length repeat
         sum := sum + vc(i+j-1)
      tvec(i):=sum
  tv1 := tvec(1)
  for i in 1..#tvec repeat       ------PROBLEM
      tvec(i) := tvec(i)/tv1
  for i in 1..numint repeat
      for j in i..numint repeat
          NN(i,j):=tvec(j-i+1)
  for i in 2..numint repeat
      for j in 1..(i-1) repeat
         NN(i,j) := NN(j,i)
  NN

print << "begin NNMat" << newline
nn:mf:=covInt(r,[6,4,3,1]$intScheme)
print << nn << newline


import from Partial mf
import from MatrixOpDom DoubleFloat
syncByte ==> Record(data:VI, polarity:SI)

--MAIN CONSTRUCTOR (PORTABLE)

flist(k:SI):lvi ==
   v1:VI := new(k,0)
   v1(1) := 1
   v2:VI := new(k,0)
   lv := concat!(flistSplit(v1,1), flistSplit(v2,1))
   map(fixPolarity, lv)

fixPolarity(v:VI):VI ==
   pol:SI := 1
   for i in 1..#v repeat
       if v.i = 1 then
           v.i := v.i * pol
           pol := - pol
   v

flistSplit(v:vi, j:SI) : lvi ==
   j=#v => [v]
   v.j = 1 => flistSplit(v, j+1)
   v2:vi := new(#v, 0)
   for i in 1..j repeat v2(i):=v(i)
   v2(j+1):= 1
   concat!(flistSplit(v,j+1),flistSplit(v2,j+1))

convert2F(v:VI):VF ==
   v2:VF := new(#v, 0)
   for i in 1..#v repeat v2(i):=v(i)::DFLOAT
   v2

(v1:VF) - (v2:VF):VF ==
   #v1 ~= #v2 => error "different lengths"
   v3:VF := new(#v1, 0)
   for i in 1..#v1 repeat v3.i := v1.i-v2.i
   v3

univDetector(target:VF,data:VF,integ:intScheme,corr:VF,sync:syncByte):VI ==
  numint:= integ.numInt
  numpint := integ.numIntp
  length := integ.length
  offset := integ.offset
  -- assuming offset = 1
  integVec : VF := new(#data - length + 1, 0)
  for i in 1..#integVec repeat
      sum:DFLOAT := 0
      for j in 1..length repeat
         sum := sum + data(i+j-1)
      integVec(i) := sum

  uv:=SignalMatrix(target,integ)
  u:mf := uv.Umat
  v:mf := uv.Vmat
  local nn
  nn:mf:=covInt(corr,integ)
  n:=retract invertIfCan nn
  pol:SI := sync.polarity
  pinit:VI := sync.data

  flp : lvi := flist(numint-numpint)
  flm : lvi := [map(-,vv) for vv in flp]
  w : VF := new(numint, 0)
  resultVec : VI := new(#integVec - numint + 1,0)
  for k in 1..#resultVec repeat
     for j in 1..numint repeat
          w(j) := integVec(k+j-1)
     lmin:DFLOAT:=1.0e50
     detectedbit:SI := 0
     fl := if pol=1 then flp else flm
     for f in fl repeat
        noisei:vf:=w-u*convert2F pinit-v*convert2F f
        quadratici:=transpose noisei*n*noisei
        if quadratici.1 < lmin then
            lmin:=quadratici.1
            detectedbit:= f.1
     for i in 2..#pinit repeat
       pinit(i-1) := pinit(i)
     pinit(#pinit) := detectedbit
     if abs(detectedbit) =1 then pol := -pol
     resultVec(k) := detectedbit
  resultVec





T:lf:=_
[- 0.896, 9.504, - 6.496, - 33.553, 42.162, - 62.206, 70.815,_
54.813, - 66.249, 74.858, 58.856,31.799, - 23.190]



numOfStateBits :=6



stateVec2Int(v:Vector Integer):Integer ==
   s :Integer:= 0
   for i in 1..(#v)$VI repeat
       s:=2*s+v.i
   s



numOfStates := 2^numOfStateBits
hiBit := 2^(numOfStateBits-1)




StateInt(S:lvi):VI== [stateVec2Int s for s in S]
states:=StateInt(STATES)


ThreshHolds:Vector DFLOAT := new(numOfStates,1000.0)
for i in 1..nS repeat
   ThreshHolds(states.i+1):=T.i






--NOISE SCALES

import from DoubleFloatElementaryFunctions
--nscale16:DoubleFloat:=10.0^(-16.0/20.0)
--nscale18:DoubleFloat:=10.0^(-18.0/20.0)
--nscale20:DoubleFloat:=10.0^(-20.0/20.0)
--nscale22:DoubleFloat:=10.0^(-22.0/20.0)
nscale24:DoubleFloat:=10.0^(-24.0/20.0)
--nscale26:DoubleFloat:=10.0^(-26.0/20.0)
--nscale28:DoubleFloat:=10.0^(-28.0/20.0)
--nscale:LDF:=_
--[nscale16,nscale18,nscale20,nscale22,nscale24,nscale26,nscale28]


--v123:mf:=[[1,1,1,0,0,0,0,0,0]]
--v234:mf:=[[0,1,1,1,0,0,0,0,0]]
--v345:mf:=[[0,0,1,1,1,0,0,0,0]]
--v456:mf:=[[0,0,0,1,1,1,0,0,0]]
--v567:mf:=[[0,0,0,0,1,1,1,0,0]]
--v678:mf:=[[0,0,0,0,0,1,1,1,0]]
--v789:mf:=[[0,0,0,0,0,0,1,1,1]]



--t0:=v123*C*transpose v123
--t1:=v123*C*transpose v234
--t2:=v123*C*transpose v345
--t3:=v123*C*transpose v456
--t4:=v123*C*transpose v567
--t5:=v123*C*transpose v678
--t6:=v123*C*transpose v789



--t:List Float:=[t0,t1,t2,t3,t4,t5,t6]
--s0:=(t.1/t.1)
--s1:=(t.2/t.1)
--s2:=(t.3/t.1)
--s3:=(t.4/t.1)
--s4:=(t.5/t.1)
--s5:=(t.6/t.1)
--s6:=(t.7/t.1)
--s:List Float:=[s0,s1,s2,s3,s4,s5,s6]

--N:Matrix Float:=[_
--                [s0,s1,s2,s3,s4,s5],_
--                [s1,s0,s1,s2,s3,s4],_
--                [s2,s1,s0,s1,s2,s3],_
--                [s3,s2,s1,s0,s1,s2],_
--                [s4,s3,s2,s1,s0,s1],_
--                [s5,s4,s3,s2,s1,s0]]






REC==>Record(State:Integer,Metric:Float,Polarity:Integer,bit:Integer)
VREC ==> Vector REC

StateMachineDecoder(rec:REC,D:Float):REC==
  b:=rec.bit
  SS:=rec.State
  P:SI:=rec.Polarity
  DF:=P*D+ThreshHolds.(SS+1)   --CAN OUTPUT DF HERE
  local rbit : SingleInteger
  if b=1 or DF<=0 then rbit:=0
  else
    rbit:=1
    P := -P
  if SS>=hiBit then SS:=SS-hiBit
  SS:=2*SS+rbit
  [SS,DF,P,rbit]

detector(d:VF):VREC==
  n:=(#d)$VF
  b:VREC := new(n, [0,0,0,0])
  stateInit:=states.1
  metricInit:Float:=0
  polarityInit:Integer:=1
  bitInit:Integer:=0
  drec:REC:=[stateInit,metricInit,polarityInit,bitInit]
  for i in 1..n repeat
     drec:=StateMachineDecoder(drec,d.i)  --CAN OUTPUT rec HERE
     b(i) := drec
  b




(x:Integer) / (y:Integer):DFLOAT == (x::DFLOAT) / (y::DFLOAT)

--print << "begin detector" << newline
--print << "begin B" << newline
--BR:VREC:=detector(DS)                      --DECODED BIT PATTERN
--print << "begin B" << newline
--B:VI := [x.bit for x in BR]
--print << B << newline

--print << "begin P" << newline
--P:VI := [x.Polarity for x in BR]      --CHANGE, 6/1/96
--print << P << newline

--NBR:=detector(DSNP)                   --DECODED NOISY BIT PATTERN
--NB:VI := [x.bit for x in NBR]         --CHANGE, 6/1/96
--print << "begin ERR" << newline
--ERR:=[NB(i)-B(i) for i in 1..#B]
--print << ERR << newline

--print << "begin NP" << newline
--NP:VI := [x.Polarity for x in NBR]
--print << NP << newline


--print << "begin ERRP" << newline
--ERRP:=[NP(i)-P(i) for i in 1..#P]     --CHANGE, 6/1/96
--print << ERRP << newline









average(N:VF):DFLOAT ==
  sum:DFLOAT := 0
  n:=#N
  for i in 1..n repeat sum:=sum+N(i)
  sum/(n::DFLOAT)

correlator(N:VF):VF ==
  n:=#N
--  m :=(1/n)*reduce(+,[N(i) for i in 1..n],0)
  m := average N
  corr:VF:=new(9, 0)
  corr(1):= m
  for j in 0..7 repeat
    sum:DFLOAT := 0
    for i in 1..n-j repeat
        sum:=sum + N(i)*N(i+j)
    sum := sum / (n-j)::DFLOAT
    corr(j+2) := sum
  corr





--corrNoise:=correlator(NOISE)
--print << corrNoise << newline





--CHECKS FOR PROGRAM CORRECTNESS



--print << "begin signalEncoder" << newline
--EB:=signalEncoder(B);           --"NOISELESS" SIGNAL
--print << "begin EncDataParser" << newline
--PB:=DataParser(EB,7);


--print << "begin DecisionFunctionGenerator" << newline
--DB:=DecisionFunctionGenerator(PB,M,A);
--print << DB << newline

--print << "begin CHECKDB" << newline
--CHECKDB:VF:=[DS(i)-DB(i) for i in 1..#DS]
--print << CHECKDB << newline

--print << "begin detector" << newline
--DEB:VREC:=detector(DB)              --DECODED SIGNAL
--BE:VI := [x.bit for x in DEB]
--print << BE << newline


--print << "begin CHECKB" << newline
--CHECKB:=[B(i)-BE(i) for i in 1..#B]
--print << CHECKB << newline




--index(b:VI):VI== [j for j in 1..(#b-1) | b(j) ~= 0]













































