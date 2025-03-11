all:
	g++ -std=c++20 ../src/main.cpp -o bin/forke

run-debug: 
	g++ -std=c++20 ../src/main.cpp -o bin/forke -g
debug:
	gdb --args bin/forke ./examples/test.forke
exe:
	bin/forke ./examples/test.forke

run:
	bin/out
asm: 
	vim bin/out.asm
tst:
	vim examples/test.forke
clean:
	rm bin/out bin/out.asm bin/forke
