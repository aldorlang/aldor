[![Aldor CI](https://github.com/gvanuxem/aldor/actions/workflows/ci.yaml/badge.svg)](https://github.com/gvanuxem/aldor/actions/workflows/ci.yaml)

The Aldor Programming Language
==============================

Building the compiler
---------------------

The directory `aldor` contains a build infrastructure based on the GNU build
system: libtool, autoconf and automake. In order to use it, you should have
these tools, and run the command `./autogen.sh`. After that, you can set up
the build with `./configure` and start it with `make`.

Note that having the GNU build tools installed is optional.  Instead,
you use the supplied `configure` script with
`--disable-maintainer-mode`.

The build system supports parallel builds with `make -jN` where N is the number
of parallel tasks (generally: number of logical CPU cores + 2).

You can run `./configure --prefix=/install/path` to select the path you want to
install aldor to (in this case `/install/path`, but you will likely want a
different path).

Compilation of the 'C' files in aldor will fail if a warning is
generated.  If you want to the build to ignore warnings and simply
continue, add `--disable-error-on-warning` as an argument to
`./configure`.  It's likely that any new warning is harmless, and due
to a change in C compiler.

Reporting issues
----------------

Questions about compiler and library behaviour are best asked on the public
[mailing list](https://groups.google.com/forum/#!forum/aldor-devel). For
issues such as inexplicable program crashes, compiler crashes, incorrect code
generation, etc., the [issue tracker](https://github.com/pippijn/aldor/issues)
is the place to report them.

When reporting an issue, it is best to provide a minimal test case that we can
use to reproduce your issue. If the issue occurs with interpreted code, you
can test it on the latest version of the compiler and interpreter provided at
[Try Aldor](http://pippijn.github.io/aldor/eval).

#### Report format

In order to facilitate machine-aided processing of tickets, we recommend that
any code illustrating the issue you are reporting be formatted in github
flavoured [Markdown](http://github.github.com/github-flavored-markdown/). The
first line of a file should contain the file name in a comment. *A precise
definition of the expected format will be added to the wiki, later.*
