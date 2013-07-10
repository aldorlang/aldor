
#include "dbg_defs"


DebugUtilities:with
{
	words: String -> List String;
		++ words(line) can be used to split the text line
		++ into individual words. Words are separated by
		++ whitespace (space, tab or newline characters).
	split: String -> (String, String);
		++ return the first word which is command
		++ and the string other than command which is the content
}
== add
{
	-- Function to normalise replace different sorts
	-- of whitespace with a space character.
	local norm(c:Character):Character ==
	{
		(c = space) => space;
		(c = tab) => space;
		(c = newline) => space;
		c;
	}

	words(cmd:String):List(String) ==
	{
		import from Character, String, List String;
		--import from TextWriter, WriterManipulator;
		
		local chr:SingleInteger;
		local word:String;
		local result:List String := empty;

		-- Normalise the command.
		cmd := map!(norm)(cmd);

		-- Trim leading and trailing whitespace.
		cmd := leftTrim(rightTrim(cmd, space), space);

		-- Nothing in the word yet.
		word := new(#cmd);
		chr  := 0;

		-- Split into words.
		--while (not(end?(cmd, chr))) repeat
		while (chr < #cmd) repeat
		{
			-- Have we reached a space yet?
			if ((cmd.chr = space) or (cmd.chr = newline))then
			{
				-- Yes: terminate the word. We can't use
				-- end! because Basicmath lacks it.
				word1 := substring(word, 0, chr);

				-- Add it to the command list.
				result := cons(word1, result);


				-- Chop this word from the line.
				cmd := substring(cmd, chr+1, #cmd);


				-- Remove leading whitespace.
				cmd := leftTrim(cmd, space);


				-- Ready to start the next word.
				chr := 0;
			}
			else
			{
				-- Add this character to the word.
				word.chr := cmd.chr;


				-- Move to the next character.
				chr := chr + 1;
			}
		}


		-- Deal with any partial words.
		if (chr > 0) then
		{
			-- Terminate the word.
			word := substring(word, 0, chr);


			-- Add it to the command list.
			result := cons(copy word, result);
		}

		-- Reverse the list of command words.
		reverse result;
	}


	split(cmd: String): (String, String) == {
		import from TextWriter, Character, String;

		local chr:SingleInteger;
		local word:String;	

		-- Normalise the command.
		cmd := map!(norm)(cmd);

		-- Trim leading and trailing whitespace.

		cmd := leftTrim(cmd, space);

		cmd := rightTrim(cmd, space);

		local fst, rst: String;

		-- Nothing in the word yet.
		word := new(#cmd);
		chr  := 0;

		--while (not(end?(cmd, chr))) repeat
		while (chr < #cmd) repeat
		{
			-- Have we reached a space yet?
			if (cmd.chr = space) then
			{
				-- Yes: terminate the word. We can't use
				-- end! because Basicmath lacks it.
				fst := substring(word, 0, chr);

				-- Add it to the command list.
				--result := cons(copy word, result);

				-- Chop this word from the line.
				rst := substring(cmd, chr+1, #cmd-chr-1);

				-- Remove leading whitespace.
				rst := leftTrim(rst, space);

				return (fst, rst);
			}
			else
			{
				-- Add this character to the word.
				word.chr := cmd.chr;

				-- Move to the next character.
				chr := chr + 1;
			}
		}

		(word, new(0));
	}

}
