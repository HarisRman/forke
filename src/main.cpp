#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <optional>
#include <vector>

#include "include/generator.hpp"

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
	
	Generator generator(std::move(prog.value()));

	{
		std::ofstream file ("out.asm");
		file << generator.generate_prog();
	}

	system("nasm -f elf64 out.asm");
	system("ld out.o -o out");
	system("rm out.o");


	return 0;

}

