
#include "dbg_defs"

------------------------------------------------------------------------
-- Justification of arbitrary values.

JustifyPackage(T:BasicType):with
{
	justify: (T, SingleInteger, 'left') -> String;
		++ justify(t, w, left) truncates, or pads t with
		++ spaces on the right to into w characters.

	justify: (T, SingleInteger, 'right') -> String;
		++ justify(t, w, right) truncates, or pads t with
		++ spaces on the left to fit into w characters.

	justify: (T, SingleInteger, 'center') -> String;
		++ justify(t, w, center) truncates, or pads t with
		++ spaces both sides to fit into w characters.

	justify: (T, SingleInteger, 'centre') -> String;
		++ justify(t, w, centre) truncates, or pads t with
		++ spaces both sides to fit into w characters.

	export from 'left', 'centre', 'center', 'right';
}
== add
{
	-- Justification type.
	import from 'left', 'centre', 'center', 'right';
	import from String;

	-- This is just an alias.
	justify(t:T, w:SingleInteger, dummy:'center'):String ==
		justify(t, w, centre);


	-- Left justification of text strings.
	justify(t:T, w:SingleInteger, dummy:'left'):String ==
	{
		import from Character;
		--import from FormattedOutput;
		import from TextWriter, String;


		local extra:SingleInteger;
		--local result:String := string("~a")(<< t);
		local result:String;
		result::TextWriter << t;
		

		-- How much padding is required?
		extra := (w - #result);

		-- Truncate if too wide.
		(extra < 1) => substring(result, 0, w);

		-- Padding on the right.
		--concat(result, new(extra, space));
		result := result + new(extra, space);
	}


	-- Right justification of text strings.
	justify(t:T, w:SingleInteger, dummy:'right'):String ==
	{
		import from Character;
		--import from FormattedOutput;

		local extra:SingleInteger;
		--local result:String :=  string("~a")(<< t);
		local result:String;
		result::TextWriter << t;

		-- How much padding is required?
		extra := (w - #result);


		-- Truncate if too wide.
		(extra < 1) => substring(result, 1, w);


		-- Padding on the left.
		--concat(new(extra, space), result);
		tmp := new(extra, space) + result;
		result := tmp;
	}


	-- Centre justification of text strings.
	justify(t:T, w:SingleInteger, dummy:'centre'):String ==
	{
		import from Character;
		--import from FormattedOutput;

		local left, right, extra:SingleInteger;
		--local result:String := string("~a")(<< t);
		local result:String;
		result::TextWriter << t;

		-- How much padding is required?
		extra := (w - #result);


		-- Truncate if too wide.
		(extra < 1) => substring(result, 1, w);


		-- Split the padding.
		left  := extra quo 2;
		right := extra - left;


		-- Padding on both sides.
		--concat(new(left, space), result, new(right, space));
		tmp := new(left, space) + result + new(right, space);
		result := tmp;
	}
}

