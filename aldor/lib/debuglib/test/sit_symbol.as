
#include "aldor"

Symbol: with {
	new: () -> %;

} == add {
	Rep == String;

	new():% == {
		import from String;
		return "new";
		never;
	}


}
