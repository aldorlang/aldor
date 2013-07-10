#include "aldor"
#include "aldorio"

Tree(S: OutputType): OutputType with {
    export from S;

    empty: %;
    tree:  S  -> %;
    tree:  (S, %, %) -> %;

    empty?: % -> Boolean;

    left:   % -> %;
    right:  % -> %;
    node:   % -> S;

    preorder:  % -> Generator S;
    inorder:   % -> Generator S;
    postorder: % -> Generator S;
}
== add {
    Rep == Record(node: S, left: %, right: %);
    import from Rep;

    empty: % == nil$Pointer pretend %;
    empty?(t: %): Boolean == nil?(t pretend Pointer)$Pointer;

    tree(s: S): % == per [s, empty, empty];
    tree(s: S, l: %, r: %): % == per [s, l, r];

    local nonempty(t: %): Rep == {
        import from String;
        empty? t => error "Taking a part of a non-empty tree";
        rep t
    }

    left (t: %): % == nonempty(t).left;
    right(t: %): % == nonempty(t).right;
    node (t: %): S == nonempty(t).node;
    
    preorder(t: %): Generator S == generate {
        if not empty? t then {
            yield node t;
            for n in preorder left  t repeat yield n;
            for n in preorder right t repeat yield n;
        }
    }
    inorder(t: %): Generator S == generate {
        if not empty? t then {
            for n in inorder left  t repeat yield n;
            yield node t;
            for n in inorder right t repeat yield n;
        }
    }
    postorder(t: %): Generator S == generate {
        if not empty? t then {
            for n in postorder left  t repeat yield n;
            for n in postorder right t repeat yield n;
            yield node t;
        }
    }
    (tw: TextWriter) << (t: %): TextWriter == {
        import from String;
        import from S;

        empty? t => tw << "empty";
        empty? left t and empty? right t => tw << "tree " << node t;

        tw << "tree(" << node t << ", "
           << left t  << ", " << right t << ")"
    }
}


main():() == {
    import from Tree String;
    import from List String;

    t := tree("*", tree("1", tree "a", tree "b"),
           tree("2", tree "c", tree "d"));

    stdout << "The tree is " << t << newline;
    stdout << "Preorder:   " << [preorder  t] << newline;
    stdout << "Inorder:    " << [inorder   t] << newline;
    stdout << "Postorder:  " << [postorder t] << newline;
}

main();
