# PortablePE
Because the Portable Executable isn't Portable Enough

![Screenshot](https://github.com/LukeSilva/PortablePE/blob/master/screenshot.png?raw=true)

This is a project where we're aiming to create a .exe file that runs on multiple platforms (almost) natively.

The idea is that every Microsoft windows .exe file starts with an MS-DOS header.
This header MUST begin with the characters 'MZ', but then, if you don't want the .exe to run properly on DOS you have 60 bytes of unused space.

The idea is to fill this space with a bash script, that eventually constructs a native executable (elf on linux, Mach-O on Mac) from the .exe, reusing as much application code as possible, and runs it.

Justine Tunney has created a complete toolchain to generate these types of executables ([Introduction](https://justine.storage.googleapis.com/ape.html)) ([GitHub](https://github.com/jart/cosmopolitan)). I've had the idea for a while however, and ended up creating a demo before realising his work.


# My Implementation

For our usage, we place the following bash script in the first 60 bytes of the exe.
```
MZ=1
S=`tail -c+64 $0|head|tr -d '\0'`
sh -c "i=$0;$S"
exit
```

This opens the current file, skips the first 64 bytes, reads the next 10 lines after this point, removes any null characters, and stores the result in the variable S.
Then a shell runs, setting i to the name of the file, and executing the script found at offset 64.

Offset 64 contains the following shell script:

```
dd if=$i bs=1K skip=8 of=$i.elf 2>/dev/null
chmod a+x $i.elf
./$i.elf
exit
```

This reads an elf file starting at 8KB into the file, and runs it.


The layout of the exectuable is as follows:
```
------------------------------------------
0x0000  'MZ' MAGIC string     (2 bytes)
------------------------------------------
0x0002  Stage 1 shell script  (58 bytes)
------------------------------------------
0x003c  Pointer to PE Header  (4 bytes)
------------------------------------------
0x0040  Stage 2 shell script  (192 bytes)
------------------------------------------
0x0100  PE Headers            (??? bytes)
------------------------------------------
0x2000  ELF Executable        (??? bytes)
------------------------------------------
```

In order to share code between both Windows and Linux, all code is stored within the elf executable. The PE Headers are generated to reference the text / data sections within the elf executable. The build process thus first builds the elf executable (making sure sections are aligned as required by Windows), and then the additional headers are added and appended to the elf file.