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
	elif,
	_else,
	loop,
	eq,
	plus,
	minus,
	star,
	fslash,
	g_than,
	l_than,
	eq_to,
	not_eq_to
};

struct Token 
{
	TokenType type; 
	size_t line;
	std::optional<std::string> value;	
};

std::optional<int> bin_op_prec(const Token& tok) {
	switch(tok.type)
	{
		case TokenType::g_than :
		case TokenType::l_than :
		case TokenType::eq_to :
		case TokenType::not_eq_to :
			return 0;
		case TokenType::plus :
		case TokenType::minus:
			return 1;
		case TokenType::star :
		case TokenType::fslash  :
			return 2;
		default:
			return std::nullopt;
	}
}

std::string type_to_str(TokenType type) {
	switch(type) 
	{
		case TokenType::exit :
			return "an exit"; 
		case TokenType::open_paren :
			return "'('";
		case TokenType::close_paren :
			return "')'";
		case TokenType::v_bar :
			return "'|'";
		case TokenType::open_curly :
			return "'{'";
		case TokenType::close_curly :
			return "'}'";
		case TokenType::int_lit :
			return "an integer";
		case TokenType::semi :
			return "';'";
		case TokenType::ident :
			return "an identifier";
		case TokenType::make :
			return "a make statement";
		case TokenType::_if :
			return "an if statement";
		case TokenType::elif :
			return "an elif statement";
		case TokenType::_else :
			return "an else statement";
		case TokenType::loop :
			return "a loop";
		case TokenType::eq :
			return "'='";
		case TokenType::plus :
			return "'+'";
		case TokenType::minus :
			return "'-'";
		case TokenType::star :
			return "'*'";
		case TokenType::fslash :
			return "'/'";
		//TODO add more
		default:
			return "i dont even know bruh\n";
			exit(EXIT_FAILURE);	
	}
}


class Tokenizer {
public:
	inline Tokenizer (std::string p_src) : m_src(std::move(p_src)), m_index(0), m_line(1) {};

	inline std::vector<Token> tokenize() {
		std::vector<Token> output;

		std::string buf;
		while(peak().has_value())
		{
			char current = peak().value();
			if (isspace(current))
			{
				if (current == '\n')
					m_line++;
				consume();
			}
			else if (isalpha(current))
			{
				buf.push_back(consume());
				while (peak().has_value() && isalnum(peak().value()))
					buf.push_back(consume());

				if (buf == "exit")
					output.push_back({TokenType::exit, m_line});
				else if (buf == "make")
					output.push_back({TokenType::make, m_line});
				else if (buf == "if")
					output.push_back({TokenType::_if, m_line});
				else if (buf == "elif")
					output.push_back({TokenType::elif, m_line});
				else if (buf == "else")
					output.push_back({TokenType::_else, m_line});
				else if (buf == "loop")
					output.push_back({TokenType::loop, m_line});				
				else {
					output.push_back({TokenType::ident, m_line, buf});
				}

				buf.clear();
			}

			else if (isdigit(current)) 
			{
				while (peak().has_value() && isdigit(peak().value()))
					buf.push_back(consume());

				output.push_back({TokenType::int_lit, m_line, buf});

			    	buf.clear();
			}

			else if (current == '/' && peak(1).has_value() && peak(1).value() == '/')	//Single line comments	
			{
				consume(); consume();
				while (peak().has_value() && peak().value() != '\n')
					consume();
			}

			else if (current == '/' && peak(1).has_value() && peak(1).value() == '*')        /*multi line comments*/
			{
				consume(); consume();
				auto p1 = peak();
				auto p2 = peak(1);
				while (p1.has_value())
				{
					if (p1.value() == '*' && p2.has_value() && p2.value() == '/')
					{
						consume(); consume();
						break;
					}
					
					consume();

					p1 = p2;
					p2 = peak(1);
				}
			}

			else if (current == '=' && peak(1).has_value() && peak(1).value() == '=')
			{
				consume();
				consume();
				output.push_back({TokenType::eq_to, m_line});
			}

			else if (current == '!' && peak(1).has_value() && peak(1).value() == '=')
			{	
				consume();
				consume();
				output.push_back({TokenType::not_eq_to, m_line});
			}

			else 
			{
				switch(current)
				{
					case ';':
						consume();
						output.push_back({TokenType::semi, m_line});
						break;
					case '|': 
						consume();
						output.push_back({TokenType::v_bar, m_line});
						break;
					case '(':
						consume();
						output.push_back({TokenType::open_paren, m_line});
						break;
					case ')':
						consume();
						output.push_back({TokenType::close_paren, m_line});
						break;
					case '{':
						consume();
						output.push_back({TokenType::open_curly, m_line});
						break;
					case '}':
						consume();
						output.push_back({TokenType::close_curly, m_line});
						break;
					case '=':
						consume();
						output.push_back({TokenType::eq, m_line});
						break;
					case '+':
						consume();
						output.push_back({TokenType::plus, m_line});
						break;
					case '*':
						consume();
						output.push_back({TokenType::star, m_line});
						break;
					case '-':
						consume();
						output.push_back({TokenType::minus, m_line});
						break;
					case '/':
						consume();
						output.push_back({TokenType::fslash, m_line});
						break;
					case '>':
						consume();
						output.push_back({TokenType::g_than, m_line});
						break;
					case '<':
						consume();
						output.push_back({TokenType::l_than, m_line});
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
	size_t m_line;

	inline std::optional<char> peak(int jump = 0) const {
		if (m_index + jump >= m_src.length())
			return std::nullopt; 

		return m_src.at(m_index + jump);
	}

	inline char consume() {
		return m_src.at(m_index++);
	}

};




