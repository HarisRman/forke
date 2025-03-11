all:
	g++ -std=c++20 ../src/main.cpp -o forke

dbg: 
	g++ -std=c++20 ../src/main.cpp -o forke -g
run-dbg:
	gdb --args ./forke ./examples/test.forke
run:
	bin/forke ../examples/test.forke
clean:
	rm out out.asm forke
