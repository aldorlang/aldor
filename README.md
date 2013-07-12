aldor
=====

The Aldor Programming Language.

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

See the wiki for more information.
