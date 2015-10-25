#include "comp"
#include "aldorio"

#pile

TFormAttrs: with
    import from Id
    create: (s: Id == id "", sx: SExpression == nil) -> %
    id: % -> Id
    sx: % -> SExpression
== add
    Rep == Record(s: Id, sx: SExpression)
    import from Rep
    create(s: Id == id "", sx: SExpression): % == per [s, sx]
    id(a: %): Id == rep(a).s
    sx(a: %): SExpression == rep(a).sx

Subst: OutputType with
    create: (Id, TForm) -> %
    create: List Cross(Id, TForm) -> %
    lookup: (%, Id, TForm) -> TForm
    empty?: % -> Boolean

    export from List Cross(Id, TForm)
== add
    Rep == HashTable(Id, TForm)
    import from Rep
    empty?(sigma: %): Boolean == empty? rep sigma
    create(l: List Cross(Id, TForm)): % == per [pair for pair in l]
    create(id: Id, tf: TForm): % == per [(id, tf)@Cross(Id, TForm)]
    lookup(sigma: %, id: Id, alt: TForm): TForm ==
        import from Partial TForm
        x := find(id, rep(sigma))
	failed? x => alt
	retract x

    (o: TextWriter) << (sigma: %): TextWriter ==
        import from Id, TForm
        sep := ""
	o << "{"
        for (k, v) in rep sigma repeat
	    o << sep << k << " --> " << v
	    sep := ","
        o << "}"

TForm: Join(OutputType, PrimitiveType) with
    args: % -> List %
    argCount: % -> Integer
    bindings: % -> BindingSet
    attrs: % -> TFormAttrs

    new: (TFormTagCat, List TForm) -> %
    new: (TFormTagCat, List TForm, BindingSet) -> %
    new: (TFormTagCat, List TForm, bs: BindingSet == empty(), attrs: TFormAttrs) -> %

    tag: % -> TFormTagCat
    kind: % -> String
    freeVars: % -> List Id
    subst: (%, Subst) -> %

    same?: (%, %) -> Boolean
    lookup: (%, List MachineInteger) -> %

    absyn: % -> AbSyn
== add
    Rep == Record(bindings: BindingSet, tag: TFormTagCat,
                  args: List TForm, attrs: TFormAttrs,
		  absyn: Partial AbSyn == failed);
    import from Rep
    import from TFormAttrs
    default tf, tf1, tf2: %

    args tf: List % == rep(tf).args
    argCount tf: Integer ==
        import from MachineInteger
	coerce #args tf
    tag tf: TFormTagCat == rep(tf).tag
    bindings tf: BindingSet == rep(tf).bindings
    attrs tf: TFormAttrs == rep(tf).attrs
    kind tf: String == name$(tag tf)
    freeVars tf: List Id == tfFreeVars(tf)$(tag tf)
    subst(tf, sigma: Subst): % == tfSubst(tf, sigma)$(tag tf)

    new(D: TFormTagCat, args: List TForm): % ==
    	   new(D, args, empty()$BindingSet)

    new(D: TFormTagCat, args: List TForm, bindings: BindingSet): % ==
    	   new(D, args, bindings, create())

    new(D: TFormTagCat, args: List TForm, bindings: BindingSet, a: TFormAttrs): % ==
    	   per [bindings, D, args, a]
   
    same?(tf1, tf2): Boolean ==
        import from TypedPointer TForm
        pointer tf1 = pointer tf2

    (tf1) = (tf2): Boolean ==
        import from Id
	same?(tf1, tf2) => true
        name$(tag tf1) = name$(tag tf2) => tfEquals(tf1, tf2)$(tag tf1)
  	false

    absyn(tf: %): AbSyn ==
        import from Partial AbSyn
        not failed? rep(tf).absyn => retract rep(tf).absyn
        tfAsAbSyn := tfAbSyn(tf)$(tag tf)
	rep(tf).absyn := [tfAsAbSyn]
	tfAsAbSyn

    lookup(tf, path: List MachineInteger): % ==
        empty? path => tf
	n: MachineInteger := first path
	lookup((args tf).n, rest path)

    (o: TextWriter) << tf: TextWriter ==
        import from Id
        o << "{" << name$(tag tf) << " ";
	info(o, tf)$(tag tf)
	o << args tf << " " << "}"

TFormTagCat: Category == with
    name: String
    type?: () -> Boolean
        ++ 'type?()' is true if the tform must represent a type.  ie. X satisfies Type
	++ some tags represent types in some cases, but not all.  eg. Id.
    category?: () -> Boolean
        ++ 'category?()' is true if this tform must be a category
    tfEquals: (TForm, TForm) -> Boolean
    tfSubst: (TForm, Subst) -> TForm
    tfFreeVars: TForm -> List(Id)
    info: (TextWriter, TForm) -> ();
    tfAbSyn: TForm -> AbSyn;
    default 
        info(tx: TextWriter, tf: TForm): () == {}
        category?(): Boolean == false
