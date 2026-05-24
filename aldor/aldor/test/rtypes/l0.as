#pile

Type: with == add
Category: with == add
((A: Tuple Type) -> (R: Tuple Type)): with == add
Tuple(T: Type): with == add
Record(T: Tuple Type): with == add
Enumeration(T: Tuple Type): with == add;

Boolean: with
    true: %
    false: %
== add
    true: % == never
    false: % == never

TT: with
    create: () -> %
    consume: % -> ()
    value: %
    *: (%, %) -> %
== add
    create(): % == never
    consume(a: %): () == never
    value: % == never
    (a: %) * (b: %): % == never

