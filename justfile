alias b := build

makedir:
  mkdir -p build

build: makedir
  gcc main.c -o build/main
