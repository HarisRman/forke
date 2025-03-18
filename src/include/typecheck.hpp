#pragma once

#include "parser.hpp"
#include "arena.hpp"
#include "types.hpp"

#include <utility>

class TypeChecker {
public:	
	struct VarType
	{
		DataType type;
		std::optional<DataType> pointed_type;
	};
	
	inline void check(const NodeProg* prog) {
		for (const NodeStmt* stmt : prog->stmts)
		{
			check_stmt(stmt);
		}	
	}

	inline const std::unordered_map<std::string, VarType>& get_sym_table() {
		return m_sym_table;
	}

private:

	enum Flag 
	{
		RET_TYPE,
		RET_PTED_TYPE
	}; //STUPID workaround bit sleepy rn
	
	#define NO_OF_INCOMPATIBLE_TYPES 1
	
	std::unordered_map<std::string, VarType> m_sym_table;
	
	const std::pair<DataType,DataType> m_incompatible_types[NO_OF_INCOMPATIBLE_TYPES] = { {PTR, CHAR} };

	inline DataType compatible_type(const std::pair<DataType,DataType>& types) {
		for (int i = 0; i < NO_OF_INCOMPATIBLE_TYPES; i++)
		{
			if (m_incompatible_types[i] == types)
			{
				std::cerr << "Incompatible types jackass\n";
				exit(EXIT_FAILURE);
			}
		}

		DataType res = types.first;
		return res > types.second ? res : types.second;
	}


	//STMTS
	inline void check_stmt(const NodeStmt* stmt) {
		struct stmt_Visitor 
		{
			TypeChecker* tc;

			void operator()(const NodeStmtExit* exit) const {
				//Implement a way to verify RVALUE or LVALUE
				exit->expr->type = tc->check_expr(exit->expr);
			}

			void operator()(const NodeStmtDeclare* declare) const {
				//Implement Better Pointer chaining
				std::string ident = declare->ident.value.value();
				if (tc->m_sym_table.contains(ident))
				{
					std::cerr << "Cannot Redeclare: '" << ident << "'\n";
					exit(EXIT_FAILURE);	
				}

				if (declare->count > 1)
				{
					tc->m_sym_table.insert({ident, VarType{.type = PTR, .pointed_type = declare->type}});
				}
			 	else if (declare->type == PTR)
				{
					tc->m_sym_table.insert({ident, 
							   VarType{.type = PTR, .pointed_type = declare->pointed_type.value()}});
				} else {
					tc->m_sym_table.insert({ident, VarType{.type = declare->type}});
				  }
			}

			void operator()(const NodeStmtAssign* assign) const {
				//Implement Rvalue vs Lvalue check
				
				assign->lvalue_expr->type = tc->check_expr(assign->lvalue_expr);
				
				if (assign->rvalue_expr.has_value())
				{
					assign->rvalue_expr.value()->type = tc->check_expr(assign->rvalue_expr.value());
					if (assign->lvalue_expr->type != assign->rvalue_expr.value()->type)
					{
						std::cerr << "Assignment types not matching lil bro\n";
						exit(EXIT_FAILURE);
					}
				}
			}

			void operator()(const NodeStmtScope* scope) const {
				for (const NodeStmt* stmt : scope->stmts)
				{
					tc->check_stmt(stmt);
				}
			}

			void operator()(const NodeStmtIf* if_stmt) const {
				//Implement Rvalue Lvalue check
				if_stmt->expr->type = tc->check_expr(if_stmt->expr);
				tc->check_stmt(if_stmt->stmt);

				if (if_stmt->chain.has_value())
				{
					tc->check_if_chain(if_stmt->chain.value());
				}
			}

			void operator()(const NodeStmtLoop* loop) const {
				loop->expr->type = tc->check_expr(loop->expr);
				tc->check_stmt(loop->scope);
			}

			void operator()(const NodeStmtWrite* write) const {
				tc->check_write_stmt(write);
			}
		};

		stmt_Visitor visitor{.tc = this};
		std::visit(visitor, stmt->var);
	}

	inline void check_if_chain(const NodeIfChain* chain) {
		struct chain_Visitor 
		{
			TypeChecker* tc;

			void operator()(NodeChainElif* elif) const {
				elif->expr->type = tc->check_expr(elif->expr);
				tc->check_stmt(elif->stmt);

				if (elif->chain.has_value())
				{
					tc->check_if_chain(elif->chain.value());
				}
			}

			void operator()(NodeChainElse* _else) const {
				tc->check_stmt(_else->stmt);
			}
		};

		chain_Visitor visitor{.tc = this};
		std::visit(visitor, chain->var);
	}

	inline void check_write_stmt(const NodeStmtWrite* write) {
		struct write_Visitor 
		{
			TypeChecker* tc;

			void operator()(NodeExpr* const expr) const {
				tc->check_expr(expr);
			}
			void operator()(const std::string str) const {
				;
			}
		};

		write_Visitor visitor{.tc = this};
		std::visit(visitor, write->var);
	}

	//Exprs
	inline DataType check_expr(const NodeExpr* expr, Flag flag = RET_TYPE) {
		struct expr_Visitor 
		{
			TypeChecker* tc;
			Flag flag;

			DataType operator()(NodeTerm* const term) const {
				return tc->check_term(term, flag);
			}

			DataType operator()(NodeBinExpr* const bin_expr) const {
				return tc->check_bin_expr(bin_expr);
			}

			DataType operator()(NodeUnExpr* const un_expr) const {
				return tc->check_un_expr(un_expr);
			}
		};

		expr_Visitor visitor{.tc = this, .flag = flag};
		return std::visit(visitor, expr->var);
	}

	inline DataType check_term(const NodeTerm* term, Flag flag) {
		struct term_Visitor
		{
			TypeChecker* tc;
			Flag flag;

			DataType operator()(const NodeTermInt* int_lit) const {
				return INT;
			}

			DataType operator()(const NodeTermChar* char_lit) const {
				return CHAR;
			}

			DataType operator()(const NodeTermIdent* ident_term) const {
				const std::string ident = ident_term->ident.value.value();
				if (!tc->m_sym_table.contains(ident))
				{
					std::cerr << "'" << ident << "' was NEVER declared fucknigga\n";
					exit(EXIT_FAILURE);
				}
				
				if (flag == RET_PTED_TYPE)                   //BAD workaround. Implement pointers better
				{
					if (tc->m_sym_table[ident].type == PTR)
						return tc->m_sym_table[ident].pointed_type.value();
					else 
					{
						std::cerr << "Cannot dereference a non pointer jackass\n";
						exit(EXIT_FAILURE);
					} 
				} else 
					 return tc->m_sym_table[ident].type;
			}

			DataType operator()(const NodeTermParen* paren) const {
				return tc->check_expr(paren->expr);
			}
		};

		term_Visitor visitor{.tc = this, .flag = flag};
		return std::visit(visitor, term->var);
	}

	inline DataType check_bin_expr(const NodeBinExpr* bin_expr) {	
		return std::visit	
			([this](auto obj) 
			{
				obj->lhs->type = check_expr(obj->lhs);
				obj->rhs->type = check_expr(obj->rhs);

				return compatible_type( {obj->lhs->type, obj->rhs->type} );
			},bin_expr->var);
	}

	inline DataType check_un_expr(const NodeUnExpr* un_expr) {
		struct un_Visitor
		{
			TypeChecker* tc;

			DataType operator()(const NodeUnExprDref* dref) const {
				if (dref->rvalue_expr.has_value())
				{
					if ((dref->rvalue_expr.value()->type = tc->check_expr(dref->rvalue_expr.value())) != INT)
					{
						std::cerr << "fuck u tryna do\n";
						exit(EXIT_FAILURE);
					}
				}

				dref->lvalue_expr->type = tc->check_expr(dref->lvalue_expr);

				return tc->check_expr(dref->lvalue_expr, RET_PTED_TYPE);
			}

			DataType operator()(const NodeUnExprIncrement* increment) const {
				if (increment->rvalue_expr.has_value())
				{
					if ((increment->rvalue_expr.value()->type = tc->check_expr(increment->rvalue_expr.value())) != INT)
					{
						std::cerr << "not very sigma :(\n";
						exit(EXIT_FAILURE);
					}
				}

				return increment->lvalue_expr->type = tc->check_expr(increment->lvalue_expr);
			}

			DataType operator()(const NodeUnExprAddr* addr) const {
				addr->lvalue_expr->type = tc->check_expr(addr->lvalue_expr);
				
				return PTR;
			}
		};

		un_Visitor visitor{.tc = this};
		return std::visit(visitor, un_expr->var);
	}

	
};
