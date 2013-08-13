#include "aldor" ;

macro order == 'LESS,EQUAL,GREATER' ;
import from order ;

(x:order) case (y:order) : Boolean == x=y;

define OrderedKeyType : Category == with {
    compare: (%,%) -> order ;
}

#if top
MI ==> MachineInteger ;
#endif
import from MI ;

define OrderedSetType : Category == with {

    Key : OrderedKeyType ;

    empty : % ;
	-- (* The empty set *)

    singleton : Key -> % ;
	-- (* Create a singleton set *)

    add1  : (%, Key)-> % ;
    add1P : (Key, %) -> % ;
	-- (* Insert an item. *)

    addList : (%, List Key )-> % ;
	-- (* Insert items from list. *)

    delete : (%, Key )-> % ;
	-- (* Remove an item. Raise NotFound if not found. *)

    member : (%, Key )-> Boolean ;
	-- (* Return true if and only if item is an element in the set *)

    isEmpty?: % -> Boolean ;
	-- (* Return true if and only if the set is empty *)

    (=) : (%, %) -> Boolean ;
	-- (* Return true if and only if the two sets are equal *)

    compare : (%, %) -> order ;
	-- (* does a lexical comparison of two sets *)

    isSubset : (%, %) -> Boolean ;
	-- (* Return true if and only if the first set is a subset of the second *)

    numItems : % ->  MI ;
	-- (* Return the number of items in the table *)

    listItems : % -> List Key ;
	-- (* Return an ordered list of the items in the set *)

    union : (%, %) -> % ;
        -- (* Union *)

    intersection : (%, %) -> % ;
        -- (* Intersection *)

    difference : (%, %) -> % ;
        -- (* Difference *)

    map : (Key -> Key) -> % -> % ;
	-- (* Create a new set by applying a map function to the elements
	-- * of the set.
        -- *)
     
    app : (Key -> ()) -> % -> () ;
	--(* Apply a function to the entries of the set 
        -- * in increasing order
        -- *)

    foldl : (T:Type) -> ((Key, T )-> T) -> T -> % -> T ;
	--(* Apply a folding function to the entries of the set 
        -- * in increasing order
        -- *)

    foldr : (T:Type) -> ((Key, T )-> T) -> T -> % -> T ;
	--(* Apply a folding function to the entries of the set 
        -- * in decreasing order
        -- *)

    partition : (Key -> Boolean) -> % -> (%, %) ;

    filter : (Key -> Boolean) -> % -> % ;

    exists : (Key -> Boolean) -> % -> Boolean ;

    find : (Key -> Boolean) -> % -> Partial Key ;

#if 0  -- the following cause compiler bug 1376.
    if Key has OutputType then OutputType ;
    if Key has HashType   then HashType ;
    -- if Key has CopyableType then DeepCopyableType 
#endif

} --  (* OrderedSetType *)

