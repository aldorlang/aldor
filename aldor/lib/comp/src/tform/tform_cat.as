#include "comp.as"
#pile
-- A category is a type containing a bunch of exports.
-- a value whose type is a category is called a domain
-- categories can be constructed in the following ways:
--    from atoms: with('x', apply('List', 'Integer'))
--    via join: Join(C1, C2, C3)
--    via a name and another category : cat('Group', with("*", ...)) [nb: semantics/defaults here?]

-- Category: Type, and Category is a subtype of Type
-- Join: Tuple Category -> Category

TFormTagWith: TFormTagCat with
   _with: (Id, TForm) -> TForm

   withType: TForm -> TForm
   withId: TForm -> Id
   with?: TForm -> Boolean
== add
    import from TFormAttrs, BindingSet
    import from List TForm
    import from TForm, Id

    name: String == "with"
    with?(tf: TForm): Boolean == name = kind tf
    _with(id: Id, tf: TForm): TForm ==
        new(TFormTagWith, [tf], empty(), create(s == id))

    type?(): Boolean == true
    category?(): Boolean == true

    tfEquals(tf1: TForm, tf2: TForm): Boolean ==
        withId tf1 = withId tf2 and withType tf1 = withType tf2

    withType(tf: TForm): TForm == first args tf
    withId(tf: TForm): Id == id attrs tf

    tfFreeVars(tf: TForm): List Id == [id for id in freeVars tf  | id ~= withId tf]
    tfAbSyn(tf: TForm): AbSyn == never

TFormTagJoin: TFormTagCat with
    join: Tuple TForm -> TForm
    join: Generator TForm -> TForm
    joinArgs: TForm -> List TForm
    join?: TForm -> Boolean
== add
    import from List TForm
    import from TForm
    name: String == "join"

    type?(): Boolean == true
    category?(): Boolean == true
    join?(tf: TForm): Boolean == name = kind tf

    joinArgs(tf: TForm): List TForm == args tf
    join(types: Tuple TForm): TForm == new(TFormTagJoin, [types])
    join(types: Generator TForm): TForm == new(TFormTagJoin, [types])

    tfEquals(tf1: TForm, tf2: TForm): Boolean == joinArgs tf1 = joinArgs tf2

    tfFreeVars(tf: TForm): List Id ==
        import from Fold2(List Id, List Id), List Id
        reverse!((append!, [])/(freeVars arg for arg in args tf))

    tfAbSyn(tf: TForm): AbSyn == never

TFormTagNamedCat: TFormTagCat with
   namedCategory: (Id, TForm) -> TForm
   namedCategoryId: TForm -> Id
   namedCategoryTForm: TForm -> TForm
   namedCategory?: TForm -> Boolean
== add
    import from TForm, List TForm
    import from TFormAttrs
    import from Id

    name: String == "namedcategory"
    namedCategory?(tf: TForm): Boolean == name = kind tf

    type?(): Boolean == true
    category?(): Boolean == true

    namedCategoryId(tf: TForm): Id == id attrs tf
    namedCategoryTForm(tf: TForm): TForm == first args tf

    namedCategory(id: Id, tf: TForm): TForm == new(TFormTagNamedCat, [tf], attrs == create(s == id))

    tfEquals(tf1: TForm, tf2: TForm): Boolean ==
        namedCategoryId tf1 = namedCategoryId tf2
           and namedCategoryTForm tf1 = namedCategoryTForm tf2

    tfFreeVars(tf: TForm): List Id == tfFreeVars namedCategoryTForm tf

    tfAbSyn(tf: TForm): AbSyn == never
