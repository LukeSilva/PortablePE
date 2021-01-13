all: directories install

directories: 
	mkdir -p build

build/create_exe: create_exe/create_exe.c
	gcc create_exe/create_exe.c -o build/create_exe

build/pegenimp: pegenimp/pegenimp.c
	gcc pegenimp/pegenimp.c -o build/pegenimp

clean:
	rm -rf build

install: build/create_exe build/pegenimp
	mkdir -p bin
	cp build/create_exe bin/create_exe
	cp build/pegenimp bin/pegenimp
