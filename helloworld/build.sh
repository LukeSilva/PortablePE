PATH="$PWD/../toolchain/cross/bin:$PWD/../pe_tools/bin:$PATH"
LINK_SCRIPT="$PWD/../pe_tools/linker.T"

mkdir -p build

i686-elf-as helloworld.s -o build/helloworld.o

cd build
cp ../win_imports .
pegenimp

i686-elf-as win_imports.s -o win_imports.o

i686-elf-ld -s -T $LINK_SCRIPT win_imports.o helloworld.o 

create_exe ./a.out ../out.exe