#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "include/generator.hpp"
#include "include/typecheck.hpp"

int main(int argc, char** argv) {
	
	if (argc < 2) {std::cerr << "No source file detected"; return 1;}
	
	std::string source;
	{	
		std::ifstream sourcefile(argv[1]);
		std::stringstream buffer;
		buffer << sourcefile.rdbuf();
		source = buffer.str();
	}

	Tokenizer tokenizer(std::move(source));	
	std::vector<Token> tokens = tokenizer.tokenize();

	Parser parser(std::move(tokens));
	std::optional<NodeProg*> prog = parser.parse_prog();
	if (!prog.has_value())
	{
		std::cerr << "Failed to parse\n";
		exit(EXIT_FAILURE);
	}
		
	TypeChecker checker;
	checker.check(prog.value());	

	Generator generator(std::move(prog.value()), checker.get_sym_table());

	{
		std::ofstream file ("bin/out.asm");
		file << generator.gen_prog();
	}

	system("nasm -f elf64 bin/out.asm");
	system("ld bin/out.o -o bin/out");
	system("rm bin/out.o");


	return 0;

}

