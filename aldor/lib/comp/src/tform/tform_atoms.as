#include "comp.as"
#pile

import from Id

TFormTagType: TFormTagCat with
    type: () -> TForm
    type?: TForm -> Boolean
== add
    import from List TForm, TForm
    import from MachineInteger

    name: String == "type"
    type?(): Boolean == true
    tfEquals(ttf1: TForm, ttf2: TForm): Boolean == true

    tfFreeVars(cx: TForm): List Id == []

    type(): TForm == new(TFormTagType, [])
    type?(tf: TForm): Boolean == kind tf = name

    tfAbSyn(tf: TForm): AbSyn == id id "Type"

TFormTagCategory: TFormTagCat with
    category: () -> TForm
    theCategory?: TForm -> Boolean
== add
    import from List TForm, TForm
    name: String == "Category"
    type?(): Boolean == true

    category(): TForm == new(TFormTagCategory, [])
    theCategory?(tf: TForm): Boolean == kind tf = name
    tfEquals(t1: TForm, t2: TForm): Boolean == true

    tfFreeVars(tf: TForm): List Id == []
    tfAbSyn(tf: TForm): AbSyn == id id "Category"

TFormTagExit: TFormTagCat with
    exit: () -> TForm
    exit?: TForm -> Boolean
== add
    import from List TForm, TForm
    import from MachineInteger

    name: String == "exit"
    type?(): Boolean == true
    exit?(tf: TForm): Boolean == kind tf = name

    exit(): TForm == new(TFormTagExit, [])

    tfEquals(etf1: TForm, etf2: TForm): Boolean == true

    tfFreeVars(cx: TForm): List Id == []

    tfAbSyn(tf: TForm): AbSyn == id id "Exit"



