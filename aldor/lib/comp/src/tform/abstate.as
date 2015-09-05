#include "comp"
#pile

AbState: with
    abstate: () -> %
    unique?: % -> Boolean
    final?: % -> Boolean
    error?: % -> Boolean
    tposs?: % -> Boolean
    tposs: % -> TPoss
    unique: % -> TForm

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

    unique? state: Boolean == rep(state).flg = FINAL or (rep(state).flg = POSS and unique? rep(state).type.tposs)
    final? state: Boolean == rep(state).flg = FINAL
    error? state: Boolean == rep(state).flg = ERROR
    tposs? state: Boolean == rep(state).flg = POSS
    tposs state: TPoss == rep(state).type.tposs

    unique state: TForm ==
        import from Partial TForm
        rep(state).flg = FINAL => rep(state).type.tf
	rep(state).flg = POSS => retract unique rep(state).type.tposs
	never

    setFinal(state, tform: TForm): () ==
        rep(state).flg ~= POSS and rep(state).flg ~= NONE => error("Can't set type")
        rep(state).flg := FINAL
	rep(state).type.tf := tform

    setTPoss(state, tp: TPoss): () ==
        rep(state).flg ~= NONE => error "too late for setTPoss"
        rep(state).flg := POSS
	rep(state).type.tposs := tp

    setError(state): () == rep(state).flg := ERROR

