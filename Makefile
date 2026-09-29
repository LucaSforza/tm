all: build


makedir:
	mkdir -p build

build: main.c makedir
	gcc main.c -I./thirdparty/rax/ -o build/main
