@echo off
rem
rem The Aldor compiler configured to use GMP
rem
rem Please set GmpDir variable to the location of the libgmp.lib file. If the
rem library uses a DLL file, put the dll file in path.
rem 

rem set GmpDir=/projects/orcca/aldor_win/base/win32msvc/lib

if not defined GmpDir (
	echo "no GmpDir defined -- the linker will fail unless the GMP library is in a well-known place"
) else (
	set d="-Y %GmpDir%"
	echo aldor %d% -Cruntime=cyggmp-3,foam-gmp,foam %*
	aldor %d% -Cruntime=foam-gmp,gmp,foam %*
)

echo on
