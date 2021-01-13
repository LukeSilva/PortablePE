all: directories create_exe pegenimp

directories: 
	mkdir -p build

create_exe: create_exe/create_exe.c
	gcc create_exe/create_exe.c -o build/create_exe

pegenimp: pegenimp/pegenimp.c
	gcc pegenimp/pegenimp.c -o build/pegenimp

clean:
	rm -rf build

install:
	mkdir -p bin
	cp build/create_exe bin/create_exe
	cp build/pegenimp bin/pegenimp
