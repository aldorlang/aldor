echo job: $JOB_NAME directory: $(pwd)
set -x
case $JOB_NAME in
    aldor-gcc)
	configure_options='CFLAGS='
	;;
    aldor-gcc-m32)
	configure_options='CFLAGS=-m32 LDFLAGS=-m32 UNICLFLAGS=-Wsys=linux-x32'
	build_options=UNICLFLAGS=-Wsys=linux-x32
	;;
    aldor-clang)
	configure_options='--with-boehm-gc CC=clang UNICLFLAGS=-Wsys=linuxclang'
	build_options=UNICLFLAGS=-Wsys=linuxclang
	;;
    *)
	echo "Unknown job name"
	;;
esac


thisdir=$(pwd)
cd aldor
./autogen.sh
cd $thisdir
rm -rf build
mkdir build
cd $thisdir/build
$thisdir/aldor/configure --prefix=$thisdir/opt $configure_options
make -j $build_options && make check $build_options -j && make install $build_options -j && make -j installcheck


