
#include "axllib"
#include "debuglib"

-- Change IndexEntry into SI recording file position.
-- Bug: attemping to insert entries may cause the tree to subdivide even
--      though we may eventually realise that we cannot insert the element.

SI ==> SingleInteger;

IndexEntry:with
{
	new: (SI, SI) -> %;
		++ `new(lin, pos)' represents the index entry for line
		++ `lin' whose absolute position in a file is `pos'.

	copy: % -> %;
		++ Return a new index entry which does not share any
		++ storage with the original.

	nil: %;
		++ The special invalid index entry. Only the nil?
		++ function may be applied to this value.

	nil?: % -> Boolean;
		++ Returns true iff applied to the special value nil.

	line: % -> SI;
		++ `line(idx)' returns the line number associated with
		++ the index entry `idx'.

	position: % -> SI;
		++ `position(idx)' returns the absolute file position
		++ associated with the index entry `idx'.

	dispose!: % -> ();
		++ Dispose of the specified index entry.

	<< : (TextWriter, %) -> TextWriter;
		++ Debugging print function.
}
== add
{
	Rep == Record(fline:SI, fpos:SI);
	import from Rep;

	nil?(x:%):Boolean == zero?(x pretend SI);

	nil:% == (0@SI) pretend %;

	new(fl:SI, fp:SI):% == per [fl, fp];

	copy(x:%):% ==
	{
		(nil? x) => x;
		per record explode rep x;
	}

	line(x:%):SI == (rep x).fline;

	position(x:%):SI == (rep x).fpos;

	dispose!(x:%):() ==
	{
		if (not nil? x) then
			dispose! rep x;
	}

	(tw:TextWriter) << (x:%):TextWriter ==
	{
		nil? x => (tw << "*");
		tw << "<" << (line x) << ", " << (position x) << ">";
	}
}


TextFileIndex:with
{
	new: FileName -> %;
		++ Create a new index for the specified file.

	apply: (%, SI) -> IndexEntry;
		++ `idx.lin' searches for the line `lin' in the
		++ index `idx'. It returns the index entry of the
		++ closest line preceding `lin' or nil if `lin'
		++ is less than one.

	set!: (%, SI, IndexEntry) -> %;
		++ `idx.lin := elt' sets the index entry for line
		++ `lin' to `elt' provided the index isn't too
		++ densely populated at this point. The entry will
		++ be copied if used and can be freed afterwards.

	dispose!: % -> ();
		++ Completely dispose of the index.

	<< : (TextWriter, %) -> TextWriter;
		++ Debugging print function.

	readline: (FileName, %, SI) -> String;
		++ readline(fname, fidx, lin) returns the line appearing
		++ in fname at line lin. The index fidx is updated. Only
		++ bad reads (EOF etc) return the empty string. All others
		++ have a newline terminator.
}
== add
{
	-- Internal representation: indices are represented as a
	-- multi-level binary decomposition. At the top-level for
	-- a file with N lines we have two segments covering the
	-- range 1..N/2 and N/2+1..N. The first segment always
	-- contains an index entry corresponding to the first line
	-- of the file.
	--
	-- To find the nearest index entry that precedes a given
	-- line we recursively search the upper segment of the
	-- index if the line number is greater than N/2. If an
	-- entry is found then it will be returned. Otherwise
	-- we recursively search the lower segment: this will
	-- always return an index entry because the recursion
	-- will eventually reach the entry for line 1.
	--
	-- Initially the upper index segment is empty (searches
	-- immediately fail) while the lower segment contains
	-- a leaf node for line 1.
	--
	-- Adding a new entry to an empty segment turns it into a
	-- leaf node with that entry. Adding an entry to a leaf
	-- node changes it into a binary segment with a leaf node
	-- in the relevent part and an empty node in the other.
	-- Adding a new entry to a segment node causes a recursion
	-- into the lower or upper segment as appropriate.
	--
	-- Subdivision stops if the new segment node would cover
	-- a range smaller than K lines (a compile-time constant).
	-- This number ought not to be too small otherwise we will
	-- spend more time searching for index entries than it
	-- would take to fseek and read multiple lines. On the
	-- other hand it must not be too large otherwise there
	-- will be unacceptable delays for locating lines.
	--
	-- We may wish to use tree depth as an alternative limit.
	NodeRep == Record(nlo:SI, nmid:SI, nhi:SI, plo:%, phi:%);
	Rep == Union(inode:NodeRep, ileaf:IndexEntry, inull:SI);
	import from SI, Rep;


	-- The smallest range we allow is K lines. Note that a
	-- binary segment of K lines may contain two indices for
	-- the lines 1..K/2 and K/2+1..K.
	K ==> (4@SingleInteger);


	-- Abbreviations to keep signatures short.
	IE ==> IndexEntry;


	-- Local mid-point function to ensure that all functions
	-- in this domain use the same one!
	local midpoint(lo:SI, hi:SI):SI == (lo + hi) quo 2;


	-- Local tests.
	empty?(x:%):Boolean == (rep x) case inull;
	leaf?(x:%):Boolean  == (rep x) case ileaf;
	node?(x:%):Boolean  == (rep x) case inode;


	-- Local accessors.
	getLeaf(x:%):IndexEntry == (rep x).ileaf;
	getNode(x:%):NodeRep    == (rep x).inode;


	-- Local constructors.
	local empty():% == per [inull == 0];

	local leaf(x:IndexEntry):% == per [ileaf == copy x];

	local node(nl:SI, nh:SI, pl:%, ph:%):% ==
	{
		local nm:SI := midpoint(nl, nh);
		per [inode == [nl, nm, nh, pl, ph]];
	}


	-- Local function for counting the number of lines
	-- in a text file. We simply open the file and see
	-- how many lines we can read before the EOF.
	local countLines(fn:FileName):SI ==
	{
		import from Character;

		local line:String   := copy("");
		local result:SI     := 0;
		local handle:InFile := open(fn);


		-- Check to see if we were successful.
		(not open? handle) => -1;


		-- Loop reading lines.
		repeat
		{
			-- Dispose the previous line.
			dispose! line;


			-- Read the next one from the file.
			line := readline! handle;


			-- An empty line means that we hit EOF.
			(zero? #line) => break;


			-- Read another line.
			result := result + 1;


			-- If the line doesn't have a newline on
			-- the end then it must be the final one.
			if (line.(#line) = newline) then iterate;
			break;
		}


		-- Dispose of the final line.
		dispose! line;


		-- Close the file.
		close handle;


		-- Return the line count.
		result;
	}


	-- Index creation.
	new(fn:FileName):% ==
	{
		local totlines:SI;
		local first:%;


		-- Count the number of lines in the file.
		totlines := countLines(fn);


		-- Create the first real index entry for line 1.
		first := leaf(new(1, 0));


		-- Create the top-level index entry.
		node(1, totlines, first, empty());
	}


	-- Accessor function.
	apply(x:%, lin:SI):IndexEntry ==
	{
		-- The documentation for this export states that it
		-- always returns a valid index entry unless the line
		-- number is less than one. This only true for indices
		-- which have been created by the new() export: the
		-- recursive calls in this function may return nil.
		local node:NodeRep;
		local result:IndexEntry;


		-- The empty node has no entries: this can never
		-- happen for indices created with new() but may
		-- occur when we recurse into an index segment.
		(empty? x) => nil;


		-- Index leaves have just one entry.
		(leaf? x) =>
		{
			-- Get this entry.
			result := getLeaf x;


			-- Return it if it precedes ours.
			(line result <= lin) => result;


			-- No: return nothing.
			nil;
		}


		-- Extract the node.
		node := getNode x;


		-- Check to see if it lies within this segment. This
		-- test is redundant when we recurse ...
		(lin < node.nlo) => nil;


		-- If it lies in the upper segment and we have an
		-- index entry for it, return that.
		if (lin > node.nmid) then
		{
			-- Search this segment.
			result := (node.phi).lin;


			-- Did we find anything?
			if (not nil? result) then return result;
		}


		-- The lower segment contains the closest preceding
		-- index entry for this line.
		(node.plo).lin;
	}


	-- Update function.
	set!(x:%, lin:SI, ent:IndexEntry):% ==
	{
		-- This function does not guarantee to insert `ent'
		-- into the index `x'. However, if it does insert it
		-- then a copy is made. The caller is encouraged to
		-- dispose of the index entry after this update.
		local nd:NodeRep;
		local nl, nm, nh:SI;


		-- All indices must be segment nodes.
		assert(node? x);


		-- Update a leaf node by subdivision.
		local updateLeaf!(idx:%, lin:SI, elt:IE, lo:SI, hi:SI):% ==
		{
			-- Temporary work area.
			local tmp:%;


			-- Extract the leaf.
			local orig:IE := getLeaf idx;


			-- Do nothing if already indexed.
			(lin = (line orig)) => idx;


			-- Compute the midpoint.
			local mid:SI := midpoint(lo, hi);


			-- Which segment does it lie in?
			((line orig) > mid) =>
			{
				-- Upper segment: what about the new node?
				(lin > mid) =>
				{
					-- Both in upper segment.
					tmp := node(lo, hi, empty(), idx);


					-- Insert the new entry and return.
					tmp.lin := elt;
				}


				-- One in each segment: nice.
				node(lo, hi, leaf elt, idx);
			}


			-- Original lies in lower segment: new one?
			(lin > mid) => node(lo, hi, idx, leaf elt);


			-- Both in lower segment: drat.
			tmp := node(lo, hi, idx, empty());


			-- Insert the new entry and return.
			tmp.lin := elt;
		}


		-- We use a local recursive function to perform
		-- the index update because we need to give it
		-- some extra information.
		local update!(idx:%, lin:SI, elt:IE, lo:SI, hi:SI):% ==
		{
			-- Safety checks.
			assert(lin >= lo);
			assert(lin <= hi);
			assert(lo  <= hi);


			-- Empty nodes are always replaced with leaves.
			(empty? idx) => leaf ent;


			-- Can we subdivide this node any further?
			((hi - lo) <= K) => idx;


			-- Leaf nodes are replaced by segment nodes if
			-- the line is not already indexed.
			(leaf? idx) => updateLeaf!(idx, lin, elt, lo, hi);


			-- Get the node representation.
			local nd:NodeRep := getNode idx;


			-- Get the segment ranges.
			local nl:SI := nd.nlo;
			local nm:SI := nd.nmid;
			local nh:SI := nd.nhi;


			-- Which segment do we update?
			if (lin > nm) then
				nd.phi := update!(nd.phi, lin, ent, nm+1, nh);
			else
				nd.plo := update!(nd.plo, lin, elt, nl, nm);


			-- Return the update node.
			idx;
		}


		-- Convert into a segment node.
		nd := getNode x;


		-- Get the segment ranges.
		nl := nd.nlo;
		nm := nd.nmid;
		nh := nd.nhi;


		-- Check for sane indexes (eg for empty files etc).
		(nh <= nl) => x;


		-- Check that it lies in the range of the index.
		(lin < nl) => x;
		(lin > nh) => x;


		-- Recursively update the index.
		update!(x, lin, ent, nl, nh);
	}


	-- Waste disposal.
	dispose!(x:%):() ==
	{
		-- Deal with the contents of the index.
		if (node? x) then
		{
			-- Extract the node representation.
			local node:NodeRep := getNode x;


			-- Recursively dispose of the contents.
			dispose! node.plo;
			dispose! node.phi;
		}
		else if (leaf? x) then
			dispose! getLeaf x;


		-- Now dispose the top-level representation.
		dispose! rep x;
	}


	-- Debugging print function.
	(tw:TextWriter) << (x:%):TextWriter ==
	{
		(empty? x) => (tw << "#");
		(leaf? x) => (tw << (getLeaf x));

		local n:NodeRep := getNode x;

		tw << "[";
		tw << (n.nlo) << "-" << (n.nmid) << ": ";
		tw << (n.plo);
		tw << ", ";
		tw << (n.nmid + 1) << "-" << (n.nhi) << ": ";
		tw << (n.phi);
		tw << "]";
	}


	-- Read a line from the specified file. The index is used
	-- to find the closest position in the file and is updated
	-- with details of the newly found position. The empty
	-- string is ONLY returned if an error occurred such as
	-- trying to read past the EOF or failure to open the file.
	readline(fname:FileName, fidx:%, lin:SI):String ==
	{
		import
		{
			fseek: (InFile, SI, SI) -> SI;
			ftell: InFile -> SI;
		} from Foreign C;

		import from SI, Character;


		local lno:SI;
		local handle:InFile;
		local ent:IndexEntry;
		local result:String;
		local eof?:Boolean;


		-- We require that the index is a segmented node.
		assert(node? fidx);


		-- Check the maximum line number.
		(lin > (getNode fidx).nhi) => "";


		-- Try to open the file.
		handle := open(fname);


		-- Did we succeed?
		(not open? handle) => "";


		-- Search for the closest preceding index line.
		ent := fidx.lin;


		-- Seek to that position in the file.
		fseek(handle, position ent, 0);


		-- Which line are we at?
		lno := line ent;


		-- Safety check.
		assert(lno >= 1);


		-- Read lines until we each this one.
		eof? := false;
		while ((not eof?) and (lno < lin)) repeat
		{
			-- Read line.
			result := readline! handle;


			-- An empty line means that we hit EOF.
			if (zero? #result) then eof? := true;


			-- Increment the line counter.
			lno := lno + 1;
		}


		-- Have we read past EOF?
		eof? => "";


		-- Note the file position as an index entry.
		ent := new(lin, ftell handle);


		-- Try to store the position in the index.
		fidx.lin := ent;


		-- Don't need this entry now.
		dispose! ent;


		-- Read the requested line.
		result := readline! handle;


		-- Add a newline character if necessary.
		if (not (result.(#result) = newline)) then
			result := concat(result, new(1, newline));


		-- Close the file.
		close handle;


		-- Return the requested line.
		result;
	}
}


trace(steps? == true)$NewDebugPackage;
showLines();


showLines():() ==
{
	import from List SI, Character;

	local fname:FileName;
	local fidx:TextFileIndex;
	local tmp:String;


	-- Construct a filename.
	fname := filename "./freader.as";


	-- Create a simple index for this file.
	fidx := new(fname);


	-- Loop printing specific lines from the file.
	for i in [5, 2, 87, 6, 24, 1000, 87, 403, 130, 38, 635] repeat
	{
		-- Find this line.
		tmp := readline(fname, fidx, i);


		-- Empty line means error/EOF.
		if (zero? #tmp) then
			print << i << ": (EOF or error)" << newline;
		else
			print << i << ": " << tmp;
	}
}
