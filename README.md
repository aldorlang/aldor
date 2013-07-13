The Aldor Programming Language
==============================

Building the compiler
---------------------

Before you start the build, take a look at `config.mk`. This file contains
some toolchains and build-specific variables. You will probably not need to
touch most of those, but in the first section, you can select your platform.
This is set to `unix` by default, and can be modified for other platforms.

To build, run `make` in the project root directory. This will create a
directory called `build` with everything in it. If you use a bourne shell
(such as dash, bash or zsh), you can `source env.sh` to set up the correct
environment variables needed to run the aldor compiler.

You can run `make loop` to start a REPL (read-eval-print-loop).

Note that running `make` twice is likely to fail on aldor libraries. If you
want to rebuild a single library, it and all its dependencies must be cleaned,
first. The dependencies are as follows: libfoam -> libfoamlib, libalgebra ->
libaldor, libaxldem -> libaxllib.

In order to clean a single target (library or program), run `make
clean-$TARGET`, where `$TARGET` is the target you want to clean. E.g. `make
clean-aldor` removes the compiler program from the build directory and cleans
up all the object files for the program. Running `make clean-libaldor` will do
the same for libaldor.

This version of Aldor was tested on Linux64, Cygwin32, and Mingw32. Users of
Mac OS X, FreeBSD and other platforms are welcome to report their issues.

See the [wiki](https://github.com/pippijn/aldor/wiki) for more information.


Advanced method
---------------

The directory `aldor` contains a more advanced build infrastructure based on
the GNU build system: libtool, autoconf and automake. In order to use it, you
will need these tools, and run the command `./autogen.sh`. After that, you can
set up the build with `./configure` and start it with `make`.

Unlike the simple build above, the advanced build supports parallel builds
with `make -jN` where N is the number of parallel tasks (generally: number of
CPU cores + 2).

Also unlike the above method, the autotools based build supports installing
and uninstalling. You can run `./configure --prefix=/install/path` to select
the path you want to install aldor to (in this case `/install/path`, but you
will likely want a different path). You will still need to set ALDORROOT to
the install path in order to use the installed aldor.


Reporting issues
----------------

Questions about compiler and library behaviour are best asked on the public
[mailing list](https://groups.google.com/forum/#!forum/aldor-devel). For
issues such as inexplicable program crashes, compiler crashes, incorrect code
generation, etc., the [issue tracker](https://github.com/pippijn/aldor/wiki)
is the place to report them.

When reporting an issue, it is best to provide a minimal test case that we can
use to reproduce your issue. If the issue occurs with interpreted code, you
can test it on the latest version of the compiler and interpreter provided at
[Try Aldor](http://xinutec.org/~pippijn/eval/aldor.html).

#### Report format

In order to facilitate machine-aided processing of tickets, we recommend that
any code illustrating the issue you are reporting be formatted in github
flavoured [Markdown](http://github.github.com/github-flavored-markdown/). The
first line of a file should contain the file name in a comment. *A precise
definition of the expected format will be added to the wiki, later.*
