# PortablePE
Because the Portable Executable isn't Portable Enough

This is a project where we're aiming to create a .exe file that runs on multiple platforms (almost) natively.

The idea is that every Microsoft windows .exe file starts with an MS-DOS header.
This header MUST begin with the characters 'MZ', but then, if you don't want the .exe to run properly on DOS you have 60 bytes of unused space.
The idea is to fill this space with a bash script, that eventually constructs a native executable (elf on linux, Mach-O on Mac) from the .exe, reusing as much application code as possible, and runs it
Something like this could be used as a first stage bootstrap
```MZ=1
dd if=$1 bs=512 skip=4 count=8 | sh
```

Potentially with more magic and ASCII code hacks we could make this bash script be a valid header for some other systems.



## This project might be dead
However, due to time constraints, don't expect much development on this. I'm just uploading it to GitHub as a backup.
