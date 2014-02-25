echo job: $JOB_NAME directory: $(pwd)
set -x
case $JOB_NAME in
    aldor-gcc)
	configure_options='CFLAGS='
	make=make
	;;
    aldor-gcc-m32)
	configure_options='CFLAGS=-m32 LDFLAGS=-m32 UNICLFLAGS=-Wsys=linux-x32'
	build_options=UNICLFLAGS=-Wsys=linux-x32
	make=make
	;;
    rh-32bit-aldor-gcc)
	configure_options='CFLAGS='
	make=make
	;;
    aldor-clang)
	configure_options='--with-boehm-gc CC=clang UNICLFLAGS=-Wsys=linuxclang'
	build_options=UNICLFLAGS=-Wsys=linuxclang
	make=make
	;;
    freebsd-clang)
	configure_options='--with-boehm-gc CC=clang UNICLFLAGS=-Wsys=linuxclang'
	build_options=UNICLFLAGS=-Wsys=linuxclang
	make=gmake
	;;
    freebsd-gcc)
	configure_options='--with-boehm-gc CC=gcc UNICLFLAGS=-Wsys=freebsd-gcc'
	build_options=UNICLFLAGS=-Wsys=freebsd-gcc
	make=gmake
	;;
    *)
	echo "Unknown job name"
	;;
esac

if [ "$make" = "" ];
then
    exit 1
fi

thisdir=$(pwd)
cd aldor
./autogen.sh
cd $thisdir
rm -rf build
mkdir build
cd $thisdir/build
$thisdir/aldor/configure --prefix=$thisdir/opt $configure_options
$make -j $build_options && $make check $build_options -j && $make install $build_options -j && $make -j installcheck


