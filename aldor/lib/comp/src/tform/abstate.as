#include "comp"
#pile

AbState: with
    abstate: () -> %
    final?: % -> Boolean
    error?: % -> Boolean
    tposs?: % -> Boolean
    tposs: % -> TPoss
    final: % -> TForm

    setFinal: (%, TForm) -> ()
    setTPoss: (%, TPoss) -> ()
    setError: % -> ()
== add
    FLAG == 'NONE,POSS,FINAL,ERROR'
    TYPE == Union(tf: TForm, tposs: TPoss, none: Boolean)
    Rep == Record(flg: FLAG, type: TYPE)
    import from Rep, FLAG, TYPE
    import from String, TPoss
    default state: %

    abstate(): % == per [NONE, [false]]

    final? state: Boolean == rep(state).flg = FINAL
    error? state: Boolean == rep(state).flg = ERROR
    tposs? state: Boolean == rep(state).flg = POSS
    tposs state: TPoss == rep(state).type.tposs
    final state: TForm == rep(state).type.tf

    setFinal(state, tform: TForm): () ==
        rep(state).flg ~= POSS and rep(state).flg ~= NONE => error("Can't set type")
        rep(state).flg := FINAL
	rep(state).type.tf := tform

    setTPoss(state, tp: TPoss): () ==
        rep(state).flg ~= NONE => error "too late for setTPoss"
        rep(state).flg := POSS
	rep(state).type.tposs := tp

    setError(state): () == rep(state).flg := ERROR
