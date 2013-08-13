#include "aldor"

MI ==> MachineInteger ;

#include "ord-set-type"

PartialType (T:Type) : Category == with {
    bracket : T -> % ;
    failed : % ;
    failed? : % -> Boolean ;
    retract : % -> T ;
}

extend Partial(T:Type) : PartialType T == add Partial T ;

SomeErrorType : Category == with {} ;
SomeError : SomeErrorType == add {} ;

ImpossibleType : Category == with {} ;
Impossible : ImpossibleType == add {} ;

NotFoundType : Category == with {} ;
NotFound : NotFoundType == add {} ;

SemiKeyType(MyKey:OrderedKeyType) : Category == with {
    semiKey  : MyKey          -> %       ;
    isDeleted? :  %           -> Boolean ;
    delete   : %              -> ()      ;
    coerce   : %              -> MyKey   ;
    compare  : (%     ,%)     -> order   ;
    compare  : (MyKey ,%)     -> order   ;
    compare  : (%     ,MyKey) -> order   ;
    set!     : (%     ,MyKey) -> %       ;

}

SemiKey(MyKey:OrderedKeyType) : SemiKeyType MyKey == add {
    Rep == Record(key:MyKey,deleted?:Boolean) ;
    import from Rep ;
    semiKey(me:MyKey):% == per[key==me,deleted?==false] ;
    isDeleted?(me:%) : Boolean == rep(me).deleted? ;
    delete(me:%) : () == { rep(me).deleted? := true ; }
    coerce(me:%) : MyKey == rep(me).key ;
    compare(me1:%,me2:%) : order == compare(me1::MyKey,me2::MyKey) ;
    compare(me1:MyKey,me2:%) : order == compare(me1,me2::MyKey) ;
    compare(me1:%,me2:MyKey) : order == compare(me1::MyKey,me2) ;
    set!(me1:%,k:MyKey):% == { rep(me1).key:=k ; rep(me1).deleted? := false ;
                               me1 ;
    }
}

AvlSet(MyKey:OrderedKeyType) : with {
                               -- OrderedSetType
}
                               == add {
    Key : OrderedKeyType == MyKey ;
    SemiKey : SemiKeyType Key == SemiKey(Key) ;
    import from SemiKey ;

    NonEmpty == Record(bal:order,left:Tree,right:Tree,key:SemiKey) ;
    import from NonEmpty ;
    Tree : PartialType NonEmpty == Partial NonEmpty ;
    import from Tree ;
    Rep == Record(count:MI, deletedCount:MI,tree:Tree) ;
    import from Rep ;

    empty : Tree == failed ;
    empty : % == per [count==0,deletedCount==0,tree==empty] ;

    -- local constructors
    local build(key1:SemiKey,left1:Tree,right1:Tree,bal1:order) : NonEmpty == [
      key==key1,
      left==left1,
      right==right1,
      bal==bal1] ;

    build(key1:Key,left1:Tree,right1:Tree,bal1:order) : NonEmpty == {
        build(semiKey key1,left1,right1,bal1);
    }

    build(key1:Key,left1:Tree,right1:Tree,bal1:order) : Tree == {
        [build(semiKey key1,left1,right1,bal1)];
    }

    build(key1:SemiKey,left1:Tree,right1:Tree) : NonEmpty == {
        hl == height left1 ;
        hr == height right1 ;
        bal1 : order == {
            hl=hr+1 => GREATER ;
            hl=hr   => EQUAL ;
            hl=hr-1 => LESS;
            assert(hl-hr>-2 and hl-hr<2) ;
            never;
        }
        [bal==bal1,left==left1,right==right1,key==key1]
    } -- end build

    build(key1:SemiKey,left1:Tree,right1:Tree) : Tree == {
        [build(key1,left1,right1)] ;
    }

    build(key1:Key,left1:Tree,right1:Tree) : Tree == {
        [build(semiKey(key1),left1,right1)] ;
    }

    build (key1:Key,left1:%,right1:%) : % == {
        per[count==rep(left1).count+rep(right1).count,
            deletedCount==rep(left1).deletedCount+rep(right1).deletedCount,
            tree==build(key1,rep(left1).tree,rep(right1).tree)
        ] ;
    }

    
    -- local accessors
    local height(t:Tree) : MI == {
        import from Tree ;
        if failed? t then 0 else height retract t ;
    }
          height(t:NonEmpty) : MI == select t.bal in {
        LESS   => 2 + height t.left ;
        EQUAL  => 1 + height t.left ;
        GREATER=> 2 + height t.right;
        never;
    }
          height(me:%) : MI == height(rep(me).tree) ;

    key(me:%) : Partial Key == key(rep(me).tree) ;
    key(me:%) : Partial SemiKey == key(rep(me).tree) ;
    key(me:Tree) : Partial Key == {
        empty? me => failed ;
        [key retract me] ;
    }
    key(me:Tree) : Partial SemiKey == {
        empty? me => failed ;
        [key retract me] ;
    }
    key(me:NonEmpty):SemiKey == me.key ;
    key(me:NonEmpty):Key == key(me)::Key ;
    left(me:NonEmpty) : Tree == me.left ;
    left(me:Tree):Tree == (failed? me => empty ; retract(me).left) ;
    right(me:NonEmpty) : Tree == me.right ;
    right(me:Tree): Tree == (failed? me => empty;  retract(me).right) ;
    balance(me:Tree):Partial order == (failed? me => failed;
                                       [balance retract me]);
    balance(me:NonEmpty):order == (me.bal) ;

    empty?(me:%):Boolean == rep(me).count=0 ;
    empty?(t:Tree):Boolean == failed? t ;

    listItems (me:%) : List Key == [ k for k:Key in me ] ;


    foldl(T:Type) (f:(Key,T)->T) (b:T) (me:%) : T == {
        answer : T := b ;
        for x:Key in me repeat answer := f(x,answer) ;
        answer ;
    }

    foldr(T:Type) (f:(Key,T)->T) (b:T) (me:%) : T == {
        answer : T := b ;
        for x:Key in reverse me repeat answer := f(x,answer) ;
        answer ;
    }

    filter(f:Key->Boolean) (me:%) : % == {
        (answer:%,__:%) := partition(f)(me) ;
        answer ;
    }

    exists (f:Key->Boolean) (me:%) : Boolean == {
        answer := false ;
        for x:Key in me while not answer repeat answer := f x ;
        answer ;
    }

    -- public constructors
    singleton(key:Key) : % == build(key,empty,empty) ;
    singleton(key:Key) : NonEmpty == retract build(key,empty,empty) ;

    add1(me:%,k:Key) : % == {
        empty? me => singleton k ;
        (new,reuse,htChange,tree1) == add1(rep(me).tree,k) ;
        per[count==rep(me).count+(new => 1;0),
            deletedCount==rep(me).deletedCount + (reuse => -1;0),
            tree==[tree1]];
    } where {
        llRotate(me:NonEmpty) : NonEmpty == build(
          --            A               B
          --           / \             / \
          --          /   \           /   \
          --         B    xxC  ==>  xxL    A
          --        / \                   / \
          --       /   \                 /   \
          --     xxL   xxxR           xxxR   xxC
          --
          bal1==GREATER,
          key1==topCenter,
          left1==topLeft,
          right1==[build(
            key1==topRight,
            bal1==GREATER,
            left1==rightLeft,
            right1==rightRight)]) where {
                topCenter : SemiKey== key retract left me ;
                topLeft   : Tree   == left left me ;
                topRight  : SemiKey== key me ;
                rightRight: Tree   == right me ;
                rightLeft : Tree   == right left me ;
            } ;

        rrRotate(me:NonEmpty) : NonEmpty == build(
          bal1==LESS,
          key1==topCenter,
          left1==[build(
            key1==topLeft,
            bal1==LESS,
            right1==leftRight,
            left1==leftLeft)],
          right1==topRight) where {
                topCenter : SemiKey== key retract right me ;
                topRight  : Tree   == right right me ;
                topLeft   : SemiKey== key me ;
                leftLeft  : Tree   == left me ;
                leftRight : Tree   == left right me ;
            } ;

        lrRotate(me:NonEmpty) : NonEmpty == build(
          --            A               C
          --           / \             / \+
          --          /   \           /   \
          --         B    xxC  ==>   B     A
          --        / \             / \   / \+
          --       /   \        xxxL  cL /   \
          --     xxxL   C               cR    xxC
          --           / \
          --          cL  cR
          --
          --
          --
          --
          --
          --
          bal1==GREATER,
          key1==C,
          left1==[build(
                    bal1==(cBal=LESS=>LESS;EQUAL),
                    left1==xxxL,
                    right1==cL,
                    key1==B)],
          right1==[build(
            key1==A,
            bal1==GREATER,
            left1==cR,
            right1==xxC)]) where {
                import from Partial SemiKey ;
                A : SemiKey == key me;
                ll: Tree == left me ;
                rr: Tree == right me ;
                B : SemiKey == retract key ll ;
                lr: Tree == right ll ;
                C : SemiKey == retract key lr ;
                cBal : order == balance retract lr ;
                xxxL : Tree == left ll ;
                cL : Tree == left lr ;
                cR : Tree == right lr ;
                xxC : Tree == right me ;
            } ;

        rlRotate(me:NonEmpty) : NonEmpty == build(
          -- The reflection of 
          --            A               C
          --           / \             / \+
          --          /   \           /   \
          --         B    xxC  ==>   B     A
          --        / \             / \   / \+
          --       /   \        xxxL  cL /   \
          --     xxxL   C               cR    xxC
          --           / \
          --          cL  cR
          --
          bal1==LESS,
          key1==C,
          right1==[build(
                    bal1==(cBal=GREATER=>GREATER;EQUAL),
                    right1==xxxL,
                    left1==cL,
                    key1==B)],
          left1==[build(
            key1==A,
            bal1==LESS,
            right1==cR,
            left1==xxC)]) where {
                -- these are all labelled wrong, in that l and r are reversed
                import from Partial SemiKey ;
                A : SemiKey == key me;
                ll: Tree == right me ;
                rr: Tree == left me ;
                B : SemiKey == retract key ll ;
                lr: Tree == left ll ;
                C : SemiKey == retract key lr ;
                cBal : order == balance retract lr ;
                xxxL : Tree == right ll ;
                cL : Tree == right lr ;
                cR : Tree == left lr ;
                xxC : Tree == left me ;
            } ;

        add1(me:Tree,k:Key):(Boolean,Boolean,Boolean,NonEmpty) == {
            (empty? me)=> (true, false, true, singleton k) ;
            add1(retract me,k) ;
        }
        add1(me:NonEmpty,k:Key):(Boolean,Boolean,Boolean,NonEmpty) == {
            select (compare(key me@Key,k)) in {
                EQUAL=> (false,isDeleted? key me,false,me) ;
                LESS => {
                    (new,reused,htChange,ltree) := add1(me.left,k) ;
                    me.left := [ltree] ;
                    not htChange => (new,reused,htChange,me) ;
                    me.bal=GREATER => { me.bal := EQUAL ;
                                        (new,reused,false,me)
                    }
                    me.bal=EQUAL => { me.bal := LESS ;
                                      (new,reused,true,me)
                    }
                    me.bal=LESS => select balance retract left me in {
                        LESS => (new,reused,false,llRotate(me)) ;
                        EQUAL=> {
                            -- Impossible, the left branch was heavy
                            -- so it contained a node, so inserting into left
                            -- can only change the height if left becomes
                            -- unbalanced.
                            throw Impossible ;
                        }
                        GREATER=>(new,reused,false,lrRotate(me));
                        never;
                    }
                    never ;
                }
                GREATER=> {
                    (new,reused,htChange,rtree) := add1(me.right,k) ;
                    me.right := [rtree] ;
                    not htChange => (new,reused,htChange,me) ;
                    me.bal=LESS => { me.bal := EQUAL ;
                                        (new,reused,false,me)
                    }
                    me.bal=EQUAL => { me.bal := GREATER ;
                                      (new,reused,true,me)
                    }
                    me.bal=GREATER => select balance retract right me in {
                        LESS => (new,reused,false,rlRotate(me)) ;
                        EQUAL=> {
                            -- Impossible, the right branch was heavy
                            -- so it contained a node, so inserting into right
                            -- can only change the height if right becomes
                            -- unbalanced.
                            throw Impossible ;
                        }
                        GREATER=>(new,reused,false,rrRotate(me));
                        never;
                    }
                    never ;
                }
                never ;
            } -- select (compare(key me1,k))
        } -- end add1
    } -- end where

#if 0    

    add1P(k:Key,me:%) : % == add1(me,k) ;

    map (g:Key->Key) (me:%) : % == {
        answer : % := empty ;
        for x:Key in me repeat answer := add1(answer,g x) ;
        answer ;
    }

    app (g:Key->())(me:%) : () == do g x for x:Key in me ;

    addList(me:%,kl:List Key):% == {
        for k in kl repeat me := add1(me,k) ;
        me ;
    }

    delete(me:%,k:Key) : % == {
        rep(me).tree := delete(rep(me).tree,k) ;
        rep(me).deletedCount := rep(me).deletedCount + 1 ;
        rep(me).count := rep(me).count -1 ;
        if rep(me).count < rep(me).deletedCount then me := copy me else me ;
    }

    delete(me:Tree,k:Key) : Tree == {
        failed? me => throw NotFound ;
        [delete (retract me,k)] ;
    }

    delete(me:NonEmpty,k:Key) : NonEmpty == {
        select compare(key me@Key,k) in {
            LESS    => {delete(left me,k) ; me}
            GREATER => {delete(right me,k) ; me }
            EQUAL => {
                import from SemiKey ;
                if isDeleted? key me then throw NotFound else {
                delete key me ; me
                }
            }
            never
        }
    }

    member(me:%,k:Key) : Boolean == member(rep(me).tree,k) ;
    member(me:Tree,k:Key) : Boolean == {
        failed? me => false ;
        select compare (k,key retract me::Key) in {
            EQUAL => isDeleted? ((key retract me)@SemiKey) ;
            LESS  => member(left me,k) ;
            GREATER=> member(right me,k) ;
            never ;
        }
    }

    find (f:Key->Boolean) (me:%) : Partial Key == {
        for k:Key in me repeat {
            if f k then return [k] ;
        }
        failed;
    }

    isEmpty?(me:%) : Boolean == numItems me = 0 ;
    numItems(me:%) : MI == rep(me).count ;
#endif

    generator(me:%) : Generator Key == k::Key for k:SemiKey in me | not isDeleted? k ;
    generator(me:%) : Generator SemiKey == generator rep(me).tree ;
    generator(me:Tree) : Generator SemiKey == {
        generate {
            failed? me => {} ;
            for x in left me repeat yield x ;
            yield key retract me ;
            for x in right me repeat yield x ;
        }
    }

    reverse(me:%)   : Generator Key == k::Key for k:SemiKey in reverse me
                                       | not isDeleted? k ;
    reverse(me:%) : Generator SemiKey == reverse rep(me).tree ;
    reverse(me:Tree) : Generator SemiKey == generate {
        failed? me => {} ;
        for x in reverse right me repeat yield x ;
        yield key retract me ;
        for x in reverse left me repeat yield x ;
    }

    walkTwo(left1:Tree,right1:Tree) : Generator Cross(Key,order) == generate {
        ll : List NonEmpty := initial left1 ;
        rr : List NonEmpty := initial right1 ;
        while not (empty? ll or empty? rr) repeat {
            while not empty? ll and isDeleted? key first ll repeat {
                ll := next ll ;
            }
            if empty? ll then iterate ;
            while not empty? rr and isDeleted? key first rr repeat {
                rr := next rr ;
            }
            if empty? rr then iterate ;
            local which : order := compare(key first ll@SemiKey,
                                           key first rr@SemiKey) ;
            yield select which in {
                LESS   => ((key first ll)::Key,which);
                EQUAL  => ((key first ll)::Key,which);
                GREATER=> ((key first rr)::Key,which);
                never;
            }

            if which ~= GREATER then ll := next ll ;
            if which ~= LESS    then rr := next rr ;
        } -- while both not empty

        while not (empty? ll) repeat {
            assert(empty? rr) ;
            while not empty? ll and isDeleted? key first ll repeat {
                ll := next ll ;
            }
            if empty? ll then iterate ;
            yield ((key first ll)::Key,LESS) ;
            ll := next ll ;
        }

        while not (empty? rr) repeat {
            assert(empty? ll) ;
            while not empty? rr and isDeleted? key first rr repeat {
                rr := next rr ;
            }
            if empty? rr then iterate ;
            yield ((key first rr)::Key,GREATER) ;
            rr := next rr ;
        }
    } -- generate
    where {
        +++ leftY prepends all of the trees on the left spine of
        +++ t to ell. 
        local leftY(t:Tree,ell:List NonEmpty) : List NonEmpty == {
            failed? t => ell ;
            leftY(left t,cons(retract t,ell)) ;
        }
        initial(t:Tree) : List NonEmpty == leftY(t,empty) ;
        next(ln:List NonEmpty):List NonEmpty == {
            empty? ln => empty ;
            leftY(right first ln,rest ln);
        }
    }

    (set1:%) = (set2:%) : Boolean == (EQUAL=compare(set1,set2)) ;

    compare(set1:%, set2:%) : order == {
        for kq in walkTwo(rep(set1).tree,rep(set2).tree) repeat {
            (k:Key,q:order) := kq ;
            if q=EQUAL then iterate else return q ;
        }
        EQUAL ;
    }

    isSubset(set1:%,set2:%) : Boolean == {
        for kq in walkTwo(rep(set1).tree,rep(set2).tree) repeat {
            (k:Key,q:order) := kq ;
            if q=LESS then return false ;
        }
        true ;
    }

    make(g:Generator Cross(Key,order),pred:Cross(Key,order)->Partial Key) : % =={
        make( -- different make that builds a set from a Generator Key
              -- that is guaranteed to generate elements in order
          generate {
              for kq in g repeat {
                  pk := pred kq ;
                  import from Partial Key ;
                  if not failed? pk then yield retract pk ;
              }
          }) ;
    }

    make(g:Generator Key) : % == {
        local (c:MI,t:Tree) := make g ;
        per [count==c,deletedCount==0,tree==t] ;
    }

    union (set1:%,set2:%) : % == make(walkTwo(rep(set1).tree,rep(set2).tree),
                                      (kq:Cross(Key,order)) : Partial Key +-> {
                                          (k,q) := kq ; [k]}) ;
    intersection (set1:%,set2:%) : % == make(
      walkTwo(rep(set1).tree,rep(set2).tree),
      (kq:Cross(Key,order)) : Partial Key +-> {
          (k,q) := kq ; if q=EQUAL then [k] else failed;
      }) ;

    difference (set1:%,set2:%) : % == make(
      walkTwo(rep(set1).tree,rep(set2).tree),
      (kq:Cross(Key,order)) : Partial Key +-> {
          (k,q) := kq ; if q=LESS then [k] else failed;
      }) ;

    copy(me:%) : % == make(x for x:Key in me) ;

    { -- beginning of where local block
      -- add1, all are defined below
        make(g:Generator Key) : (MI,Tree) == {
            digit : Digits := zero ;
            i : MI := 0 ;
            for x in g for free i in 1.. repeat digit := add1(x,digit) ;
            (i,all digit) ;
        }
        partition (pred:Key->Boolean) (me:%) : (%,%) == {
            l1 : Digits := zero;
            r1 : Digits := zero ;
            cl1 : MI := 0 ;
            cr1 : MI := 0 ;
            for x:Key in me repeat {
                if pred x then {
                    l1 := add1(x,l1) ;
                    cl1 := 1 + cl1 ;
                } else {
                    r1 := add1(x,r1) ;
                    cr1 := 1 + cr1 ;
                }
            }
            (per [tree==all l1,deletedCount==0,count==cl1],
             per [tree==all r1,deletedCount==0,count==cr1]
            ) ;
        }
    }
    where {
        oneRecord == Record(left:Tree,k:Key) ;
        twoRecord == Record(left1:Tree,k1:Key,left2:Tree,k2:Key) ;
        Digit == Union(one:oneRecord, two:twoRecord) ;
        Digits == List Digit ;
        import from oneRecord, twoRecord, Digit, Digits ;
        zero : Digits == empty ;
        add1(x:Key,d:Digits):Digits == add1(x,empty,d) ;
        add1(x:Key,t:Tree,d:Digits) : Digits == {
            empty? d => [[one==[left==t,k==x]]] ;
            dr := rest d ;
            d1 := first d ;
            select d1 in {
                one  => cons([two==[left1==d1.one.left,
                                    k1==d1.one.k, left2==t, k2==x]],
                             dr);
                two  => cons([one==[left==t,k==x]],
                             add1(d1.two.k2,
                                  build(bal1==EQUAL,
                                        left1==d1.two.left1,
                                        key1==d1.two.k1,
                                        right1==d1.two.left2),
                                  dr)) ;
                never;
            }
        }
        all(d:Digits):Tree == all(failed$Tree,d) ;
        -- I'm not sure that this is a proper strategy for AVL trees.
        -- Presumably we can build them in O(n) offline time, but I suspect
        -- that the current strategy does not work.

        -- We need to propagate the balance upwards.  As soon as we hit
        -- a two digit, we need to build a tree that is one higher than
        -- anticipated in the remaining digits.  We should be able to
        -- compensate
        all(t:Tree,ds:Digits) : Tree == {
            local shifting := false ;
            for d1 in ds repeat {
                if not shifting then {
                    t :=select d1 in {
                         one  => build(bal1==EQUAL,
                                       left1==d1.one.left,
                                       key1==d1.one.k,
                                       right1==t) ;
                         two  => (shifting := true ;
                                  build(
                           bal1==GREATER,
                           left1==d1.two.left1,
                           key1==d1.two.k1,
                           right1==build(
                             bal1==EQUAL,
                             left1==d1.two.left2,
                             key1==d1.two.k2,
                             right1==t))) ;
                         never;
                     } -- all . select
                } -- if shifting
                else {
                    t :=select d1 in {
                        one  => build(bal1==GREATER,
                                      left1==d1.one.left,
                                      key1==d1.one.k,
                                      right1==t) ;
                        two  => build(
                          right1==t,
                          bal1==EQUAL,
                          left1==build(
                            bal1==EQUAL,
                            left1==d1.two.left1,
                            key1==d1.two.k1,
                            right1==d1.two.left2),
                          key1==d1.two.k2) ;
                        never;
                    }
                }
            } -- all . for
            t
        } -- all
    } -- where

#if 0    
    --- Conditional exports
    if Key has OutputType then {
        (t:TextWriter) << (me:%) : TextWriter == {
            import from String ;
            local comma : String := "{" ;
            for x:Key in me repeat {
                t << comma << x ;
                comma := "," ;
            }
            t << "}" ;
        }
    }

    if Key has HashType then {
        local f : MI -> MI == (x:MI):MI +-> 3421 * x - 10341 ; -- expt
        macro h2(x) == f(hash(x)$Key) ;
        hash(me:%) : MI == (foldl(MI) ((x:Key,ans:MI):MI+->h2(x)+ans) 32576) me ;
    }

#endif
}  -- end add AvlSet

