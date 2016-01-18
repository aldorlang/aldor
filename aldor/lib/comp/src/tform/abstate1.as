#include "comp"
#pile

extend AbState: OutputType with
    unique?: % -> Boolean
    unique: % -> TForm
== add
    import from String, TPoss, TForm
    default state: %

    unique? state: Boolean == final? state or (tposs? state and unique? tposs state)
    unique state: TForm ==
        import from Partial TForm
        final? state => final state
	tposs? state => retract unique tposs state
	never

    (o: TextWriter) << state: TextWriter ==
        o << "{ "
	if final? state then o << "Final " << final state
	else if error? state then o << "Error " << tposs state
	else o << "None"
	o << "}"
