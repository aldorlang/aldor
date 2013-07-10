------------------------------- sal_timer.as ----------------------------------
--
-- This file provides a stopwatch-style timers for code profiling
--
-- Copyright (c) Manuel Bronstein 1998
-- Copyright (c) INRIA 1998, Version 29-10-98
-- Logiciel Salli (c) INRIA 1998, dans sa version du 29/10/1998
-----------------------------------------------------------------------------

#include "aldor"

#if ALDOC
\thistype{Timer}
\History{Manuel Bronstein}{1/10/98}{moved from sumit to salli and adapted}
\Usage{import from \this}
\Descr{\this~is a type whose elements are stopwatch timers, which can be used
to time precisely various sections of code, including garbage collection.
The precision can be up to 1 millisecond but depends on the operating system.
The times returned are CPU times (user + gc) used by the process that
created the timer.}
\begin{exports}
\alexp{gc}:     & \%  $\to$ \altype{MachineInteger} & read a timer\\
\alexp{read}:   & \%  $\to$ \altype{MachineInteger} & read a timer\\
\alexp{reset!}: & \%  $\to$ \% & reset a timer to 0\\
\alexp{start!}: & \%  $\to$ \altype{MachineInteger} & start or restart a timer\\
\alexp{stop!}:  & \%  $\to$ \altype{MachineInteger} & stop a timer\\
\alexp{timer}:  & () $\to$ \% & create a new timer\\
\end{exports}
#endif

macro Z == MachineInteger;

Timer: with {
	gc: % -> Z;
#if ALDOC
\alpage{gc}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em t} & \% & The timer to read\\ }
\Descr{Reads the timer t without stopping it.}
\Retval{Returns the total accumulated garbage collection time in
milliseconds by all
the start/stop cycles since t was created or last reset.
If t is running, the garbage collection time since the last start is added in,
and t is not stopped or affected.}
\alseealso{\alexp{read}}
#endif
	read:   %  -> Z;
#if ALDOC
\alpage{read}
\Usage{\name~t}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em t} & \% & The timer to read\\ }
\Descr{Reads the timer t without stopping it.}
\Retval{Returns the total accumulated time in milliseconds by all
the start/stop cycles since t was created or last reset.
This times includes any eventual garbage collection time
(see \alexp{gc} to extract this information).
If t is running, the time since the last start is added in,
and t is not stopped or affected.}
\begin{asex}
The following function takes a positive \altype{MachineInteger} $n$ as input,
computes and prints a machine approximation of $\sum_{i=1}^n 1/i$,
and returns the CPU time needed to compute it,
but not the time needed to print it.
\begin{ttyout}
timeHarmonic(n:MachineInteger):MachineInteger == {
        import from MachineInteger, SingleFloat, Timer, Character, TextWriter;
        t := timer();
        m:SingleFloat := 1;
        start! t;
        for i in 2..n repeat m := m + 1 / (i::SingleFloat);
        stop! t;
        stdout << "H" << n << " = " << m << newline;
        read t;
}
\end{ttyout}
\end{asex}
\alseealso{\alexp{gc}, \alexp{start!}, \alexp{stop!}}
#endif
	reset!: %  -> %;
#if ALDOC
\alpage{reset!}
\Usage{\name~t}
\Signature{\%}{\%}
\Params{ {\em t} & \% & The timer to reset\\ }
\Descr{Resets the timer t to 0 and stops it if it is running.}
\Retval{Returns the timer t after it is reset.}
#endif
	start!: %  -> Z;
#if ALDOC
\alpage{start!}
\Usage{\name~t}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em t} & \% & The timer to start\\ }
\Descr{Starts or restarts t, without resetting it to 0,
It has no effect on t if it is already running.}
\Retval{Returns 0 if t was already running, the absolute time at which
the start/restart was done otherwise.}
\alseealso{\alexp{read}, \alexp{stop!}}
#endif
	stop!:  %  -> Z;
#if ALDOC
\alpage{stop!}
\Usage{\name~t}
\Signature{\%}{\altype{MachineInteger}}
\Params{ {\em t} & \% & The timer to stop\\ }
\Descr{Stops t without resetting it to 0.
It has no effect on t if it is not running.}
\Retval{Returns the elapsed time in milliseconds since the last time t
was restarted, 0 if t was not running.}
\alseealso{\alexp{read}, \alexp{start!}}
#endif
	timer:  () -> %;
#if ALDOC
\alpage{timer}
\Usage{\name()}
\Signature{()}{\%}
\Descr{Creates a timer, set to 0 and stopped.}
\Retval{Returns the timer that has been created.}
\alseealso{\alexp{reset!}}
#endif
} == add {
        -- time     = total accumulated time since created or reset
        -- start    = absolute time of last start
        -- gctime   = total accumulated GC-time since created or reset
        -- gcstart  = absolute GC-time of last start
        -- running? = true if currently running, false if currently stopped
	Rep == Record(time:Z,start:Z,gctime:Z,gcstart:Z,running?:Boolean);

	local start(t:%):Z		== { import from Rep; rep(t).start; }
	local gcstart(t:%):Z		== { import from Rep; rep(t).gcstart; }
	local time(t:%):Z		== { import from Rep; rep(t).time; }
	local gctime(t:%):Z		== { import from Rep; rep(t).gctime; }
	local running?(t:%):Boolean	== { import from Rep; rep(t).running?; }

	timer():% == {
		import from Rep, Z, Boolean;
		per [0, 0, 0, 0, false];
	}

	local gcTime():Z == {
		import { gcTimer: () -> Pointer } from Foreign C;
		import from Record(gc:Z);
		(gcTimer() pretend Record(gc:Z)).gc;
	}

	local cpuTime():Z == {
		import { osCpuTime: () -> Z } from Foreign C;
		osCpuTime();
	}

	gc(t:%):Z == {
		running? t => gctime t + gcTime() - gcstart t;
		gctime t;
	}

	read(t:%):Z == {
		running? t => time t + cpuTime() - start t;
		time t;
	}

	stop!(t:%):Z == {
		running? t =>
			stop!(rep t, cpuTime() - start t, gcTime() - gcstart t);
		0;
	}

	local stop!(r:Rep, elapsed:Z, gcelapsed:Z):Z == {
		import from Boolean;
		r.running? := false;
		r.time := r.time + elapsed;
		r.gctime := r.gctime + gcelapsed;
		elapsed;
	}

	start!(t:%):Z == {
		running? t => 0;
		start!(rep t, cpuTime(), gcTime());
	}

	local start!(r:Rep, time:Z, gctime:Z):Z == {
		r.start := time;
		r.gcstart := gctime;
		r.running? := true;
		time;
	}

	reset!(t:%):% == {
		import from Rep, Boolean;
		rec := rep t;
		rec.time := rec.start := rec.gcstart := 0;
		rec.running? := false;
		t
	}
}
