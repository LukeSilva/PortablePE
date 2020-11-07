PATH="$PWD/../toolchain/cross/bin:$PATH"

i686-elf-as helloworld.s -o helloworld.o
i686-elf-ld -s -T linker.T -Map mapfile helloworld.o
WIN_ENTRY=`grep _start_win mapfile | sed -e 's/^.*\(0x[0-9a-f]*\).*/\1/g'`
echo "#define WIN_ENTRY $WIN_ENTRY" > win_entry.h
gcc create_exe.c -o create_exe
./create_exe ./a.out out.exe