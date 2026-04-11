#library L0 "l0.ao"
#pile
import from L0

TT2: with
    value: %
    +: (%, %) -> %
== add
    create(): % == never
    consume(a: %): () == never
    value: % == never
    (a: %) + (b: %): % == never

