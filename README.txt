1. Clone AXL (support lib) into ./axl

	$ git submodule update --init

2. Run the standard CMake build

	$ mkdir build
	$ cd build
	$ cmake ..
	$ make -j`nproc`

3. Build a tarball (optional)

	$ cpack -G TXZ

To build `tdevmon.ko` without a tarball:

	$ cd build/devmon/src/dm_lnx_lkm/kbuild
	$ make
