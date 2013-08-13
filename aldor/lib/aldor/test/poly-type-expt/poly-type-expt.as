#include "aldor"

MI ==> MachineInteger ;

EmptyErrorType : Category == with ;
EmptyError : EmptyErrorType == add ;


PairType(T:Type) : Category == with {
    first : % -> T ;
    second: % -> T ;
    pair  :(T,T) -> % ;
}

Pair(T:Type) : PairType T == add {
    Rep == Record(first:T,second:T) ;
    import from Rep ;
    first(me:%):T == rep(me).first ;
    second(me:%):T == rep(me).second ;
    pair(a:T,b:T):% == {
        import from Rep ;
        per [first==a,second==b] ;
    }
}

Stack(T:Type) : with {
    empty  : %                ;
    push   : (T,%) -> %       ;
    empty? : %     -> Boolean ;
    pop    : %     -> T       ;
    top    : %     -> T       ;
} == add {
    Rep == Record(head:Partial T,tail:Partial Stack Pair T) ;
    import from Rep ;

    empty : % == per [head==failed,tail==failed] ;
    
    empty? (me:%) : Boolean == failed? rep(me).head and failed? rep(me).tail ;

    top(me:%) : T == {
        import from Stack Pair T ;
        not failed? rep(me).head => retract rep(me).head ;
        not failed? rep(me).tail => first top retract rep(me).tail ;
        empty? me                => throw EmptyError ;
        never;
    }


    push(t:T,s:%):% == (
      import from Pair T,Partial T,Partial Stack Pair T; 
      if failed? rep(s).head
      then rep(s).head := [t] ;
      else push(pair(t,retract rep(s).head),
                (failed? rep(s).tail => empty ;retract(rep(s).tail))) ;
      s) ;

    
    pop(me:%) : T == {
        import from Stack Pair T ;
        not failed? rep(me).head => {
            local x := retract rep(me).head ;
            rep(me).head := failed ;
            x
        }
        not failed? rep(me).tail => {
            local y:Pair T := pop(retract rep(me).tail) ;
            push(second y,me) ;
            first y ;
        }
        empty? me => throw EmptyError ;
        never ;
    }
}
