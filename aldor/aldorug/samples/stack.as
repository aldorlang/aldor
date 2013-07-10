#include "aldor"
#include "aldorio"

-- implementation of stacks via lists
-- the lines starting with ++ are saved in the output of
-- the compiler, and may be browsed with an appropriate tool

Stack(S: OutputType): OutputType with {
        empty?:    % -> Boolean; ++ test for an empty stack
        empty:    () -> %;       ++ create an empty stack
        push!:(S, %) -> %;       ++ put a new element onto the stack
        pop!:      % -> S;       ++ remove the top element and return it
        top:       % -> S;       ++ return the top of the stack

        export from S;
                -- expose all operations from S 
                -- when Stack S is imported
} == add {
        -- Stacks are represented using a list. 
        -- To go between the representation and % we use the
        -- rep and per functions.
        Rep == Record(contents: List S);
        import from Rep;

        -- utility functions
        local contents(stack: %): List S == rep(stack).contents;

        -- simple functions
        empty(): % == per [empty];
        empty?(s: %): Boolean == empty? contents s;
        top(s: %): S == first contents s;

        push!(elt: S, s: %): % == {
                rep(s).contents := cons(elt, contents s);
                s
        }

        pop!(s: %): S == {
                next := first contents s;
                rep(s).contents := rest contents s;
                next;
        }

        -- needed to satisfy OutputType
        import from String;
        (tw: TextWriter) << (s: %): TextWriter == tw << "<stack>";
}


test(): () == {
        -- Importing the domains involed in the next two 
        -- lines is made by the affectations.
        l: List MachineInteger := [1,2,3,4,5,6];
        stack: Stack MachineInteger := empty();
        for x in l repeat 
                push!(x, stack);
        -- Importing the domains involed in the next
        -- line is needed.
        stdout << "stack is:" << stack << newline;
        while not empty? stack repeat {
                stdout << "Next is: " << top stack << newline;
                pop! stack;
        }
}

test()

