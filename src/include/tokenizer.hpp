#pragma once


enum class TokenType 
{
	exit,
	open_paren,
	close_paren,
	v_bar,
	open_curly,
	close_curly,
	int_lit,
	semi,
	ident,
	make,
	_if,
	eq,
	plus,
	minus,
	star,
	fslash
};

struct Token 
{
	TokenType type; 
	std::optional<std::string> value;
};

std::optional<int> bin_op_prec(const Token& tok) {
	switch(tok.type)
	{
		case TokenType::plus :
		case TokenType::minus:
			return 0;
		case TokenType::star :
		case TokenType::fslash  :
			return 1;
		default:
			return std::nullopt;
	}
}


class Tokenizer {
public:
	inline Tokenizer (std::string p_src) : m_src(std::move(p_src)), m_index(0) {};

	inline std::vector<Token> tokenize() {
		std::vector<Token> output;

		std::string buf;
		while(peak().has_value())
		{
			char current = peak().value();
			if (isspace(current))
				consume();
			
			else if (isalpha(current))
			{
				buf.push_back(consume());
				while (peak().has_value() && isalnum(peak().value()))
					buf.push_back(consume());

				if (buf == "exit")
					output.push_back({.type = TokenType::exit});
				else if (buf == "make")
					output.push_back({.type = TokenType::make});
				else if (buf == "if")
					output.push_back({.type = TokenType::_if});				
				else {
					output.push_back({.type = TokenType::ident, .value = buf});
				}

				buf.clear();
			}

			else if (isdigit(current)) 
			{
				while (peak().has_value() && isdigit(peak().value()))
					buf.push_back(consume());

				output.push_back({TokenType::int_lit, buf});

				buf.clear();
			}

			else 
			{
				switch(current)
				{
					case ';':
						consume();
						output.push_back({.type = TokenType::semi});
						break;
					case '|': 
						consume();
						output.push_back({.type = TokenType::v_bar});
						break;
					case '(':
						consume();
						output.push_back({.type = TokenType::open_paren});
						break;
					case ')':
						consume();
						output.push_back({.type = TokenType::close_paren});
						break;
					case '{':
						consume();
						output.push_back({.type = TokenType::open_curly});
						break;
					case '}':
						consume();
						output.push_back({.type = TokenType::close_curly});
						break;
					case '=':
						consume();
						output.push_back({.type = TokenType::eq});
						break;
					case '+':
						consume();
						output.push_back({.type = TokenType::plus});
						break;
					case '*':
						consume();
						output.push_back({.type = TokenType::star});
						break;
					case '-':
						consume();
						output.push_back({.type = TokenType::minus});
						break;
					case '/':
						consume();
						output.push_back({.type = TokenType::fslash});
						break;
					default:
						std::cerr<<"try something valid next time bitchass mf\n";
						exit(EXIT_FAILURE);
				}
			}	
		}

		m_index = 0;
		return output;
	
	}
	
private:
	const std::string m_src;
	size_t m_index;

	inline std::optional<char> peak(size_t jump = 0) const {
		if (m_index + jump >= m_src.length())
			return std::nullopt; 

		return m_src.at(m_index + jump);
	}

	inline char consume() {
		return m_src.at(m_index++);
	}

};




