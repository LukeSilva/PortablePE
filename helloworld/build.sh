PATH="$PWD/../toolchain/cross/bin:$PWD/../pe_tools/bin:$PATH"
LINK_SCRIPT="$PWD/../pe_tools/linker.T"

mkdir -p build

i686-elf-as helloworld.s -o build/helloworld.o
i686-elf-as print_fib.s -o build/print_fib.o

cd build
cp ../win_imports .
pegenimp

i686-elf-as win_imports.s -o win_imports.o
i686-elf-ld -g win_imports.o helloworld.o -o debug_hello # for debugging
i686-elf-ld -s -T $LINK_SCRIPT win_imports.o helloworld.o -o hello
i686-elf-ld -g win_imports.o print_fib.o -o debug_fib # for debugging
i686-elf-ld -s -T $LINK_SCRIPT win_imports.o print_fib.o -o fib 

create_exe ./hello ../hello.exe
create_exe ./fib ../fib.exe