#pragma once

#include <cstdint>

#define NO_OF_TYPES 3

#define BITMASK(x) ((x) >= 4 ? (UINT32_MAX) : ((UINT32_C(1) << (x*8)) - 1))

enum DataType 
{
	INT,
	CHAR,
	PTR
};

class TypeTable {
public:
	TypeTable(const TypeTable&) = delete;
    	TypeTable& operator=(const TypeTable&) = delete;
    	TypeTable(TypeTable&&) = delete;
    	TypeTable& operator=(TypeTable&&) = delete;
	
	TypeTable() 
	{
		table[INT]      = Type_Properties{.type_size = 4, .size_asm = "dword", .getReg = reg_32_bit};
		table[CHAR]     = Type_Properties{.type_size = 1, .size_asm = "byte" , .getReg = reg_8_bit };
		table[PTR]      = Type_Properties{.type_size = 8, .size_asm = "qword", .getReg = reg_64_bit};
	}

	struct Type_Properties 
	{	
		size_t type_size;
		const char* size_asm;
		const std::string(*getReg)(char);
	};

	const Type_Properties& operator[](const DataType index) const {
		return table[index];
	}
		
private:
	
	Type_Properties table[NO_OF_TYPES];
	
	static const std::string reg_64_bit (char name) {
		std::string reg = {'r', 'x'};
		reg.insert(1,1, name);
		return reg;
	}

	static const std::string reg_32_bit (char name) {
		std::string reg = {'e', 'x'};
		reg.insert(1, 1, name);
		return reg;
	}

	static const std::string reg_8_bit (char name) {
		std::string reg = {'l'};
		reg.insert(0, 1, name);
		return reg;
	}
};
	


