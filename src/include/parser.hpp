#pragma once

#include <variant>

#include "./tokenizer.hpp"
#include "./arena.hpp"

struct NodeExpr;

struct NodeTermInt {
	Token int_lit;
};

struct NodeTermIdent {
	Token ident;
};

struct NodeTermParen {
	NodeExpr* expr;
};

struct NodeTerm {
	std::variant<NodeTermInt*, NodeTermIdent*, NodeTermParen*> var;
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

struct NodeBinExpr {
	std::variant<NodeBinExprAdd*, NodeBinExprSub*, NodeBinExprMulti*, NodeBinExprDiv*> var;	
};

struct NodeExpr {
	std::variant<NodeTerm*, NodeBinExpr*> var;
};

struct NodeStmtExit {
	NodeExpr* expr;
};

struct NodeStmtMake {
	Token ident;
	NodeExpr* expr;
};

struct NodeStmt {
	std::variant<NodeStmtExit*, NodeStmtMake*> var;
};


struct NodeProg {
	std::vector<NodeStmt*> stmts;
};

class Parser {
public:
	inline Parser(std::vector<Token> tokens) 
		: m_tokens(std::move(tokens)), m_index(0), m_allocater(1024 * 1024 * 4)
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
	inline std::optional<Token> peak(size_t jump = 0) const {
		if (m_index + jump >= m_tokens.size() )
			return std::nullopt;
		return m_tokens.at(m_index + jump);
	}
	inline Token consume() {
		return m_tokens.at(m_index++);
	}

	inline Token try_consume(TokenType type, std::string err) {
		if (peak().has_value() && peak().value().type == type)
			return consume();
		std::cerr << err << '\n';
		exit(EXIT_FAILURE);
	}

	inline std::optional<Token> try_consume(TokenType type) {
		if (peak().has_value() && peak().value().type == type)
			return consume();
		return std::nullopt;
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
				std::cerr << "Invalid Expression :(\n";
				exit(EXIT_FAILURE);
			}

			try_consume(TokenType::close_paren, "Expected ')' :(\n");
			
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

		else {
			std::cerr << "Unreachable edge case, how tf you get here? anyways expected binary expression :(\n";
			exit(EXIT_FAILURE);
		}

	}

	inline std::optional<NodeExpr*> parse_expr(int min_prec = 0) {
		if (auto lhs_term = parse_term())
		{
			auto lhs_expr = m_allocater.alloc<NodeExpr>();
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

			return lhs_expr;

		} else {
			return std::nullopt;
		  }
	}
	
	//STATEMENTS PARSE

	inline NodeStmtMake* parse_stmt_make() {
		consume();
		NodeStmtMake* make_stmt = m_allocater.alloc<NodeStmtMake>();
		make_stmt->ident = consume();
		consume();

		if (auto expr_node = parse_expr())
		{
			make_stmt->expr = expr_node.value();
		} else {
			std::cerr << "Invalid Expression big dawg\n";
			exit(EXIT_FAILURE);
		  }

		try_consume(TokenType::semi, "Semi ko lun\n");

		return make_stmt;
	}

	inline NodeStmtExit* parse_stmt_exit() {
		consume();
		consume();
		
		auto exit_stmt = m_allocater.alloc<NodeStmtExit>();
		if (auto expr_node = parse_expr())
		{
			exit_stmt->expr = expr_node.value();			
		} else {
			std::cerr << "Invalid Expression!\n"; 
			exit(EXIT_FAILURE);
		  }

		try_consume(TokenType::close_paren, "Expected \")\"\n");

		try_consume(TokenType::semi, "Expected ';' : Semicolunn\n");

		return exit_stmt;

	}

	inline std::optional<NodeStmt *> parse_stmt() {
		if (peak().has_value() &&
		    peak().value().type == TokenType::exit &&
		    peak(1).has_value() &&
		    peak(1).value().type == TokenType::open_paren)
		{
			NodeStmtExit* exit_stmt = parse_stmt_exit();
			NodeStmt* stmt = m_allocater.alloc<NodeStmt>();
			stmt->var = exit_stmt;

			return stmt;
		}

		else if (peak().has_value() &&
			peak().value().type == TokenType::make &&
			peak(1).has_value() &&
			peak(1).value().type == TokenType::ident &&
			peak(2).has_value() &&
			peak(2).value().type == TokenType::eq)
		{
			NodeStmtMake* make_stmt = parse_stmt_make();
			NodeStmt* stmt = m_allocater.alloc<NodeStmt>();
			stmt->var = make_stmt;

			return stmt;
		}

		return std::nullopt;
	}

};
