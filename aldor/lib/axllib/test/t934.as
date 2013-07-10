-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testgen c -O
--> testcomp -O

#include "axllib"

-- This Package imports a few primitives for controling processes
-- and hopefully re-exports them in a nicer way..

import from Machine;
PID ==> BSInt;
FD  ==> BSInt;

import {
	fork:   () -> PID;
	-- pipe:	(PrimitiveArray BSInt) -> BSInt;
	pipe:	(PrimitiveArray SingleInteger) -> BSInt;
	write: (FD, BArr, BSInt) -> BSInt;
	read:  (FD, BArr, BSInt) -> BSInt;

	open:  (String, BSInt) -> FD;
	close: (FD) -> BSInt;
} from Foreign C "<unistd.h>";

import {
	wait: 		BPtr -> PID;
	waitpid:	(BSInt, BPtr, BSInt) -> PID;

	 WIFSTOPPED:	BPtr -> BBool;
	 WSTOPSIG:	BPtr -> BHInt;
	 WIFSIGNALED:	BPtr -> BBool;
	 WTERMSIG:	BPtr -> BHInt;
	 WIFEXITED:	BPtr -> BBool;
	 WEXITSTATUS:	BPtr -> BHInt;
} from Foreign C "<sys/wait.h>";

import {
	getpid: () -> PID;
	getppid: () -> PID;
} from Foreign C "<sys/types.h>";

import {
	EINTR: BSInt;
} from Foreign C "<errno.h>";

import {
	O__RDONLY: BSInt;
	O__WRONLY: BSInt;
	O__RDWR:   BSInt;
} from Foreign C "<fcntl.h>";

-- local takiness for wait...
local WaitStatus: with {
	empty:         () -> %;
	coerce:	 	% -> BPtr;
	dispose!:	% -> ();

	isStopped?:	% -> Boolean;
	isSignalled?:	% -> Boolean;
	isStdExit?:	% -> Boolean;

	stopSig:	% -> HalfInteger;
	termSig:	% -> HalfInteger;
	exitStatus:	% -> HalfInteger;

} == add {
	Rep ==> BPtr;
	import from Boolean;
	import from HalfInteger;
	default x: %;
	
	empty(): %     == array(BSInt)(0, 1+1) pretend %;
	coerce x: BPtr == rep x;
	dispose! x: () == dispose!(x pretend Arr);

	isStopped?   x: Boolean == (WIFSTOPPED rep x)::Boolean;
	isSignalled? x: Boolean == (WIFSIGNALED rep x)::Boolean;
	isStdExit?   x: Boolean == (WIFEXITED rep x)::Boolean;
	
	stopSig	     x: HalfInteger == (WSTOPSIG rep x)::HalfInteger;
	termSig	     x: HalfInteger == (WTERMSIG rep x)::HalfInteger;
	exitStatus   x: HalfInteger == (WEXITSTATUS rep x)::HalfInteger;
}

Process: BasicType with {
	pid: 	     %  -> SingleInteger;
	parentProc:  () -> %;
	thisProcess: () -> %;
	fork: 	     () -> (%, Boolean);
		++ (new proc, child?)
	wait:	     () -> (%, Boolean, HalfInteger);
		++ (exiting proc, stdexit?, signalId/exit code)
} == add {
	Rep ==> PID;
	import from SingleInteger;

	pid(x: %): SingleInteger == (rep(x))::SingleInteger;
	parentProc(): 	       % == per(getppid());
	thisProcess():         % == per(getpid());

	hash(x: %): SingleInteger == pid(x);

	fork(): (%, Boolean) == {
		result: PID := fork();
		(result = -1)::Boolean => error "fork failure";
		(result = 0)::Boolean  => (thisProcess(), true);
		(per(result), false)
	}

	wait(): (%, Boolean, HalfInteger) == {
		import from WaitStatus;
		status := empty();
		pid := per wait(status::BPtr);
		isStdExit?   status => return (pid, true, exitStatus status);
		isSignalled? status => return (pid, true, stopSig status);
		-- try again
		error "got weird child";
	}
	sample: % == thisProcess();
	(a: %) = (b: %): Boolean == (rep(a) = rep(b))::Boolean;
	(out: TextWriter) << (p: %): TextWriter ==
					out << "#Process(" << pid(p) << ")";
}

FileDescriptor: BasicType with {
	writer: % -> TextWriter;
	reader: % -> TextReader;
	open:   (FileName, OpenOps, lst: List OpenFlgs == []) -> %;
	pipe: 	() -> (%, %);
		++ reader, then writer
	close:	     %  -> ();
} == add {
	Rep ==> FD;
	import from Rep;
	import from OpenOps;

	open(name: FileName, opt: OpenOps, lst: List OpenFlgs == []): % == {
		import from FileName, SingleInteger;
		local opt: BSInt;
		nm := unparse(name);
		not empty? lst => error "open opts not supported!";
		if opt = rdonly then opt := O__RDONLY;
		if opt = rdwr   then opt := O__RDWR;
		if opt = wronly then opt := O__WRONLY;
		fd := open(nm, opt);
		if fd::SingleInteger < 0 then error "open failed!";
		per fd
	}

	close(x: %): () == {
		ret := close(rep x);
		if (ret < 0)::Boolean then error "close";
	}
	pipe(): (%, %) == {
		-- import from PrimitiveArray BSInt;
		import from PrimitiveArray SingleInteger;
		import from SingleInteger;
		arr := new(2);
		res := pipe arr;
		if (res < 0)::Boolean then error "pipe failure";
		r1 := (arr.1)::BSInt; -- arr.1;
		r2 := (arr.2)::BSInt; -- arr.2;
		dispose! arr;
		return (per r1, per r2);
	}

	writer(x: %): TextWriter == {
		-- The first function writes a character.
		-- The second function takes parameters `(s,i1,iL)'
		-- and writes the substring `s(i1..min(iL-1, #s))'.
		-- `iL=0' writes everthing up to the end of `s'.
		wrchar(c: Character): () == {
			import from SingleInteger;
			s: String == new(1,c);
			res: BSInt := write(rep x, data s, 1);
			if (res ~= 1)::Boolean then error "write --- whups";
		}

		wrstr(s: String, i1: SingleInteger, iL: SingleInteger): SingleInteger
		   == {	
			last := i1;
			while not end?(s, last) repeat last := last + 1;
			last := last;
			if zero? iL then iL := last - i1;
			end := min(i1+iL, last);
			--- xxx! should avoid substring!
			nwrit := write(rep x, data substring(s,i1, iL),
				       (end - i1)::BSInt);
			-- check for errors!
			(nwrit < 0)::Boolean => error "write";
			nwrit::SingleInteger
		}
		writer(wrchar, wrstr);
	}

	reader(x: %): TextReader == {
		import from Character, SingleInteger, String;
		rdchar(eof: Character == char 0): Character == {
			space := new(1);
			res := read(rep x, data space, 1);
			(res < 0)::Boolean => error "read failed!";
			(res = 0)::Boolean => eof;
			return space.1;
		}
		rdline(s: String, n: SingleInteger, m: SingleInteger):
							SingleInteger == {
			-- bug: rdchar() should work
			res: Character := rdchar(char 0);
			if res = char 0 then return 0;
			s.n := res;
			return 1;
		}
			
		reader(rdchar, rdline);
	}

	sample: % == per 0; -- stdin.
	(a: %) = (b: %): Boolean == (rep(a) = rep(b))::Boolean;
	(out: TextWriter) << (a: %): TextWriter == {
		import from SingleInteger;
		out << "#fd[" << (rep a)::SingleInteger << "]";
	}
} where {
	OpenOps  ==> 'rdonly,rdwr,wronly';
	OpenFlgs ==> 'noctty,nonblock,append,sync,creat,truc,excl';
}

	


