#include "comp.as"
#pile

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

    tfSubst(tf: TForm, sigma: Subst): TForm == tf


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

    tfSubst(tf: TForm, sigma: Subst): TForm == tf



