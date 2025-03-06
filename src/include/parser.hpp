#pragma once

#include <variant>

#include "./tokenizer.hpp"
#include "./types.hpp"
#include "./arena.hpp"



enum class EXPRTYPE {
	RVALUE,
	LVALUE
};


//EXPRESSIONS		//TODO fix redundent structs
struct NodeExpr;

struct NodeTermInt {
	Token int_lit;
};

struct NodeTermChar {
	Token char_lit;
};


struct NodeTermIdent {
	Token ident;
};

struct NodeTermParen {
	NodeExpr* expr;
};


struct NodeTerm {
	std::variant<NodeTermInt*,NodeTermChar*, NodeTermIdent*, NodeTermParen*> var;
	DataType type;
};

struct NodeBinExprAdd {
	NodeExpr* lhs;
	NodeExpr* rhs;	
};

struct NodeBinExprSub {
	NodeExpr* lhs;
	NodeExpr* rhs;
};

struct NodeBinExprMulti {
	NodeExpr* lhs;
	NodeExpr* rhs;
};

struct NodeBinExprDiv {
	NodeExpr* lhs;
	NodeExpr* rhs;
};

struct NodeBinExprCmp {
	TokenType cmp_op;
	NodeExpr* lhs;
	NodeExpr* rhs;
};

struct NodeBinExpr {
	std::variant<NodeBinExprAdd*, NodeBinExprSub*, NodeBinExprMulti*, NodeBinExprDiv*, NodeBinExprCmp*> var;	
};

struct NodeUnExprDref {
	Token ident;
	std::optional<NodeExpr*> expr;
};

struct NodeUnExpr {
	std::variant<NodeUnExprDref*> var;
};

struct NodeExpr {
	std::variant<NodeTerm*, NodeBinExpr*, NodeUnExpr*> var;
	//DataType type;
	EXPRTYPE expr_type;
};

//STATEMENTS
struct NodeStmt;

struct NodeScope {
	std::vector<NodeStmt*> stmts;
};

struct NodeStmtExit {
	NodeExpr* expr;
};

struct NodeStmtDeclare {
	Token ident;
	DataType type;
	size_t count;
	std::optional<DataType> pointed_type;
};

struct NodeStmtAssign {
	NodeExpr* lvalue_expr;
	NodeExpr* rvalue_expr;
};

struct NodeStmtWrite {
	std::variant<NodeExpr*, std::string> var;
	bool nl = false;
};

struct NodeIfChain;

struct NodeChainElif {
	NodeExpr* expr;
	NodeStmt* stmt;
	std::optional<NodeIfChain*> chain;
};

struct NodeChainElse {
	NodeStmt* stmt;
};

struct NodeIfChain {
	std::variant<NodeChainElif*, NodeChainElse*> var;
};

struct NodeStmtIf {
	NodeExpr* expr;
	NodeStmt* stmt;
	std::optional<NodeIfChain*> chain;
};

struct NodeLoop {
	NodeExpr* expr;
	NodeStmt* scope;
};

struct NodeStmt {
	std::variant<NodeStmtExit*, NodeStmtDeclare*, NodeStmtAssign*, NodeScope*, NodeStmtIf*, NodeLoop*, NodeStmtWrite*> var;
};


struct NodeProg {
	std::vector<NodeStmt*> stmts;
};

class Parser {
public:
	inline Parser(std::vector<Token> tokens) 
		: m_tokens(std::move(tokens)), m_index(0), m_allocater(1024 * 1024)
	{
	}

	inline std::optional<NodeProg *> parse_prog() {
		NodeProg* output = m_allocater.alloc<NodeProg>();
		while (peak().has_value())
		{
			if (auto stmt = parse_stmt())
			{
				output->stmts.push_back(stmt.value());			 
			} else {
				std::cerr << "Invalid statement. fix that shit\n";
				exit(EXIT_FAILURE);
			  }
		}

		return output;
	}

private:
	std::vector<Token> m_tokens;
	size_t m_index;
	
	ArenaAllocater m_allocater;
	
	//UTILITY METHODS
	inline std::optional<Token> peak(int jump = 0) const {
		if (m_index + jump >= m_tokens.size() )
			return std::nullopt;
		return m_tokens.at(m_index + jump);
	}
	inline Token consume() {
		return m_tokens.at(m_index++);
	}

	inline Token try_consume_exit(TokenType type) {
		if (peak().has_value() && peak().value().type == type)
			return consume();
		
		EXIT_WARNING(type_to_str(type));
		return {};
	}

	inline std::optional<Token> try_consume(TokenType type) {
		if (peak().has_value() && peak().value().type == type)
			return consume();
		return std::nullopt;
	}

	inline void EXIT_WARNING(const std::string& expected) {
		std::cerr << "[Parser] |LINE <" << peak(-1).value().line << ">| Expected " << expected << '\n';
		exit(EXIT_FAILURE);
	}	

	//EXPRESSION PARSE

	inline std::optional<NodeTerm*> parse_term() {
		if (auto tok_int = try_consume(TokenType::int_lit))
		{
			auto term_int = m_allocater.alloc<NodeTermInt>();
			term_int->int_lit = tok_int.value();
			auto term = m_allocater.alloc<NodeTerm>();
			term->var = term_int;

			return term;
		}

		if (auto tok_char = try_consume(TokenType::char_lit))
		{
			auto term_char = m_allocater.alloc<NodeTermChar>();
			term_char->char_lit = tok_char.value();
			auto term = m_allocater.alloc<NodeTerm>();
			term->var = term_char;

			return term;
		}

		else if (auto tok_ident = try_consume(TokenType::ident))
		{
			auto term_ident = m_allocater.alloc<NodeTermIdent>();
			term_ident->ident = tok_ident.value();
			auto term = m_allocater.alloc<NodeTerm>();
			term->var = term_ident;

			return term;
		}

		else if (auto tok_paren = try_consume(TokenType::open_paren))
		{
			auto expr = parse_expr();
			if (!expr.has_value()) {
				EXIT_WARNING("expression");
			}

			try_consume_exit(TokenType::close_paren);
			
			auto term_paren = m_allocater.alloc<NodeTermParen>();
			term_paren->expr = expr.value();
			auto term = m_allocater.alloc<NodeTerm>();
			term->var = term_paren;

			return term;
		}

		return std::nullopt;
	}

	inline NodeExpr* make_expr_bin(NodeExpr* lhs, NodeExpr* rhs, const Token& op) {
		NodeExpr* out = m_allocater.alloc<NodeExpr>();
		NodeBinExpr* bin_expr = m_allocater.alloc<NodeBinExpr>();

		if (op.type == TokenType::plus)
		{
			auto bin_expr_add = m_allocater.alloc<NodeBinExprAdd>();	
			bin_expr_add->lhs = lhs;
			bin_expr_add->rhs = rhs;
		
			bin_expr->var = bin_expr_add;
			out->var = bin_expr;

			return out;
		}				
		
		else if (op.type == TokenType::minus)
		{
			auto bin_expr_sub = m_allocater.alloc<NodeBinExprSub>();
			bin_expr_sub->lhs = lhs;
			bin_expr_sub->rhs = rhs;
				
			bin_expr->var = bin_expr_sub;
			out->var = bin_expr;
				
			return out;
		}
		
		else if (op.type == TokenType::star)
		{
			auto bin_expr_multi = m_allocater.alloc<NodeBinExprMulti>();
			bin_expr_multi->lhs = lhs;
			bin_expr_multi->rhs = rhs;
			
			bin_expr->var = bin_expr_multi;
			out->var = bin_expr;
				
			return out;
		}

		else if (op.type == TokenType::fslash)		
		{
			auto bin_expr_fslash = m_allocater.alloc<NodeBinExprDiv>();
			bin_expr_fslash->lhs = lhs;
			bin_expr_fslash->rhs = rhs;
				
			bin_expr->var = bin_expr_fslash;	
			out->var = bin_expr;
			
			return out;
		}

		else if (op.type == TokenType::g_than ||
			 op.type == TokenType::l_than ||
			 op.type == TokenType::eq_to  ||
			 op.type == TokenType::not_eq_to)
		{
			auto bin_expr_cmp = m_allocater.alloc<NodeBinExprCmp>();
			bin_expr_cmp->cmp_op = op.type;
			bin_expr_cmp->lhs = lhs;
			bin_expr_cmp->rhs = rhs;

			bin_expr->var = bin_expr_cmp;
			out->var = bin_expr;

			return out;
		}

		else {
			std::cerr << "Unreachable edge case, how tf you get here? anyways expected binary expression :(\n";
			exit(EXIT_FAILURE);
		}
	}

	inline std::optional<NodeExpr*> parse_expr(int min_prec = 0, EXPRTYPE type = EXPRTYPE::RVALUE) {        //TODO Unary Expression support			
		auto lhs_expr = m_allocater.alloc<NodeExpr>();

		if (auto lhs_term = parse_term())
		{
			lhs_expr->var = lhs_term.value();

			while(true)
			{
				auto cur = peak();
				if (cur.has_value())
				{
					auto prec = bin_op_prec(cur.value());
					if (!prec.has_value() || prec < min_prec)
						break;
				} else {
					break;
				  }
				
				consume();
				auto rhs_expr = parse_expr(min_prec + 1);

				NodeExpr* expr_bin = make_expr_bin(lhs_expr, rhs_expr.value(), cur.value());

				lhs_expr = expr_bin;
			}	
		}
	       
		else if	(try_consume(TokenType::dref))
		{
			auto dref = m_allocater.alloc<NodeUnExprDref>();
			dref->ident = try_consume_exit(TokenType::ident);

			if (try_consume(TokenType::l_than))
			{
				if (auto expr = parse_expr())
				{
					try_consume_exit(TokenType::g_than);
					dref->expr = expr.value();
				} else {
					EXIT_WARNING("Expression");
				  }
			}
			
			auto un_expr = m_allocater.alloc<NodeUnExpr>();
			un_expr->var = dref;
			
			lhs_expr->var = un_expr;
		}
		
		else {
			return std::nullopt;
		  }

		lhs_expr->expr_type = type;
		return lhs_expr;
	}
	
	//STATEMENTS PARSE

	inline std::optional<NodeIfChain*> parse_if_chain() {
		if (try_consume(TokenType::elif))
		{
			auto elif = m_allocater.alloc<NodeChainElif>();
			try_consume_exit(TokenType::v_bar);
			
			if (auto expr = parse_expr())
				elif->expr = expr.value();
			else {
				EXIT_WARNING("expression in elif statemnet");
			}

			try_consume_exit(TokenType::v_bar);

			if (auto stmt = parse_stmt())
				elif->stmt = stmt.value();
			else {
				EXIT_WARNING("Statment");
			}

			if (auto elif_chain = parse_if_chain())
				elif->chain = elif_chain.value();

			auto chain = m_allocater.alloc<NodeIfChain>();
			chain->var = elif;

			return chain;
		}

		else if (try_consume(TokenType::_else))
		{
			auto _else = m_allocater.alloc<NodeChainElse>();
			if (auto stmt = parse_stmt())
				_else->stmt = stmt.value();
			else {
				EXIT_WARNING("Statement");
			}

			auto chain = m_allocater.alloc<NodeIfChain>();
			chain->var = _else;

			return chain;
		}

		return std::nullopt;
	}

	//MAIN STATEMENT FUNCTION
	inline std::optional<NodeStmt *> parse_stmt() {
		NodeStmt* stmt = m_allocater.alloc<NodeStmt>();

		if (try_consume(TokenType::exit))
		{
			NodeStmtExit* exit = m_allocater.alloc<NodeStmtExit>();

			try_consume_exit(TokenType::open_paren);
			
			if (auto expr = parse_expr())
				exit->expr = expr.value();
			else EXIT_WARNING("Expression");
			
			try_consume_exit(TokenType::close_paren);
			try_consume_exit(TokenType::semi);

			stmt->var = exit;
			return stmt;
		}

		else if (auto data_type = try_consume(TokenType::data_type))
		{
			auto declare = m_allocater.alloc<NodeStmtDeclare>();
			
			DataType type;
			const std::string type_name = data_type.value().value.value();
			if      (type_name == "int")
					type = INT;
			else if (type_name == "char")
					type = CHAR;

			if (try_consume(TokenType::ptr))
			{
				declare->type = PTR;
				declare->pointed_type = type;
			}
			else {declare->type = type;}
			
			if (try_consume(TokenType::l_than))
			{
				auto int_lit = try_consume_exit(TokenType::int_lit);
				try_consume_exit(TokenType::g_than);

				declare->count = (size_t)std::stoi(int_lit.value.value());
			} else {
				declare->count = 1;
			  }

			declare->ident = try_consume_exit(TokenType::ident);
			try_consume_exit(TokenType::semi);
					
			stmt->var = declare;
			return stmt;
		}

		else if (peak().has_value() && (peak().value().type == TokenType::ident || peak().value().type == TokenType::dref))
		{	
			auto assign = m_allocater.alloc<NodeStmtAssign>();
			if (auto lvalue_expr = parse_expr(0, EXPRTYPE::LVALUE))
				assign->lvalue_expr = lvalue_expr.value();
			else 
				EXIT_WARNING("Expression");

			try_consume_exit(TokenType::eq);
			
			if (auto rvalue_expr = parse_expr())
				assign->rvalue_expr = rvalue_expr.value();
			else
				EXIT_WARNING("Expression");

			try_consume_exit(TokenType::semi);

			stmt->var = assign;
			return stmt;
		}

		else if (try_consume(TokenType::open_curly))
		{
			NodeScope* scope = m_allocater.alloc<NodeScope>();
			while (auto stmt = parse_stmt())
				scope->stmts.push_back(stmt.value());

			try_consume_exit(TokenType::close_curly);

			stmt->var = scope;

			return stmt;
		}

		else if (try_consume(TokenType::_if))
		{
			try_consume_exit(TokenType::v_bar);     
			
			NodeStmtIf* if_stmt = m_allocater.alloc<NodeStmtIf>();
			
			if (auto expr = parse_expr())
				if_stmt->expr = expr.value();
			else {
				EXIT_WARNING("Expression");
			}

			try_consume_exit(TokenType::v_bar);

			if (auto stmt = parse_stmt())
				if_stmt->stmt = stmt.value();
			else {
				EXIT_WARNING("Statement");
			}

			if (auto chain = parse_if_chain())
				if_stmt->chain = chain.value();

			stmt->var = if_stmt;
			
			return stmt;
		}

		else if (try_consume(TokenType::loop))
		{
			try_consume_exit(TokenType::v_bar);
			auto loop = m_allocater.alloc<NodeLoop>();
			
			if (auto expr = parse_expr())
				loop->expr = expr.value();
			else {
				EXIT_WARNING("Expression");
			}

			try_consume_exit(TokenType::v_bar);

			if (!peak().has_value() || peak().value().type != TokenType::open_curly)
			{
				EXIT_WARNING("Scope");
			}

			if (auto scope = parse_stmt())
				loop->scope = scope.value();
			else {
				EXIT_WARNING("Scope");
			}

			stmt->var = loop;
			return stmt;
		}

		else if (try_consume(TokenType::write))
		{
			auto write = m_allocater.alloc<NodeStmtWrite>();

			if (auto str_lit = try_consume(TokenType::str_lit))
			{
				write->var = str_lit.value().value.value();	
			}
			else if (try_consume(TokenType::v_bar))
			{
				if (auto expr = parse_expr())
					write->var = expr.value();
				else EXIT_WARNING("Expression");
				try_consume_exit(TokenType::v_bar);
			} else {
				EXIT_WARNING("String literal or an Expression");
			  }
			
			if (try_consume(TokenType::l_than) && try_consume(TokenType::g_than))
				write->nl = true;
			else
				write->nl = false;
			
			try_consume_exit(TokenType::semi);

			stmt->var = write;
			return stmt;
		}

		return std::nullopt;
	}

};
