.PHONY: build run clean

build:
	cmake -S . -B build
	cmake --build build

run: build
	./build/invoice-example

clean:
	rm -rf build