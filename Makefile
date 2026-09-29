all: build


makedir:
	mkdir -p build

build: main.c makedir
	gcc main.c -I./thirdparty/rax/ ./thirdparty/rax/rax.c -lm -o build/main
