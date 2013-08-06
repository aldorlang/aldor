--* Received: from nldi16.nag.co.uk by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA20299; Thu, 4 Jan 96 14:00:12 GMT
--* Date: Thu, 4 Jan 96 13:57:08 GMT
--* Message-Id: <16025.9601041357@nldi16.nag.co.uk>
--* Received: by nldi16.nag.co.uk (920330.SGI/UK-2.1)
--* 	id AA16025; Thu, 4 Jan 96 13:57:08 GMT
--* From: Peter Broadbery <peterb@uk.co.nag>
--* To: ax-bugs@uk.co.nag
--* Subject: [2] Substituion problems

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>


#include "axllib.as"
define FooCat(S:Type, F:(D:Type)->LinearAggregate(D)): Category == LinearAggregate(S) with {
	map: (B:Type) -> (S->B,%) -> F(B);
	default map(B:Type) (f:S->B,x:%) :F(B) == {
		[f i for i in x]$F(B); 
		}

};

--- This fails
Foo(D:Type):Join(LinearAggregate(D),FooCat(D,Foo)) with {}== add{} pretend 
	Join(LinearAggregate(D),FooCat(D,Foo)) ;

--- This works
Bar(S:Type):Join(LinearAggregate(S),FooCat(S,Bar)) with {}== add{} pretend 
	Join(LinearAggregate(S),FooCat(S,Bar)) ;

--extend List(D:Type): FooCat(D,List) with {} == add {};

--Foo(S:Type):LinearAggregate(S) with {}== add{} pretend LinearAggregate(S) ;
--extend Foo(S:Type): FooCat(S,Foo) with {} == add{};
--import from List(Integer);
--import from List(Float);
--l:=[1,2,3];
--print << l << newline;
--print << map (Float) (makeFloat$Float,l) << newline;

