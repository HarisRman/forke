#pragma once


#include "./mod_map.hpp"
#include "./parser.hpp"

class Generator {
public:
	inline Generator(NodeProg* prog)
		: m_prog(std::move(prog))
	{
	}	

	inline std::string generate_prog() {
		
		m_output << "global _start\n_start:\n";

		for (const NodeStmt* stmt : m_prog->stmts)
		{
			generate_stmt(stmt);
		}
		
		m_output << '\n';
		m_output << "    mov rax, 60" << '\n';
		m_output << "    mov rdi, 0"  << '\n';
		m_output << "    syscall"     << '\n';

		return m_output.str();
	}
private:

	struct Var
	{
		size_t stack_loc; 
	};

	//MEMBERS
	NodeProg* m_prog;
	std::stringstream m_output;
	size_t m_stack_size = 0;
	size_t m_labels = 1;
	std::vector<size_t> m_scopes;
	Modded_map<Var> m_vars;


	//UTILITY FUNCTIONS
	inline void push(std::string reg) {
		m_output << "    push " << reg << '\n';
		m_stack_size++;
	}

	inline void pop(std::string reg) {
		m_output << "    pop " << reg << '\n';
		m_stack_size--;
	}

	inline void begin_scope() {	
		m_scopes.push_back(m_vars.size());
	}

	inline void end_scope() {
		size_t pop_count = m_stack_size - m_scopes.back();
		for (int i = 0; i < pop_count; i++) 
			m_vars.pop_back();	
		m_scopes.pop_back();
		
		m_output << "    add rsp, " << pop_count * 8 << '\n';
		m_stack_size -= pop_count;
	}

	inline std::string create_label() {
		
		std::stringstream out;
		out << "label" << m_labels++;

		return out.str();
	}

	inline size_t var_offset(size_t loc) {
		return (m_stack_size - loc) * 8;
	}
	//generating assembly for EXPRESSIONS	
	
	inline void generate_term(const NodeTerm* term) {
		struct TermVisitor
		{	
			Generator* gen;

			void operator()(const NodeTermInt* int_term) const {
				gen->m_output << "    mov rax, " << int_term->int_lit.value.value() << '\n';
				gen->push("rax");
			}

			void operator()(const NodeTermIdent* ident_term) const {
				std::string identifier = ident_term->ident.value.value();
				if (!gen->m_vars.contains(identifier))
				{
					std::cerr << "'" << identifier << "' was not declared\n";
					exit(EXIT_FAILURE);
				}

				std::stringstream offset;
				offset << "Qword [rsp + "
				       << gen->var_offset(gen->m_vars.at(identifier).stack_loc)
				       << "]";

				gen->push(offset.str());
			}

			void operator()(const NodeTermParen* paren_term) {
				gen->generate_expr(paren_term->expr);
			}
		};

		TermVisitor visitor{.gen = this};
		std::visit(visitor, term->var);
	}

	inline void generate_expr(const NodeExpr* expr) {
		struct ExprVisitor 
		{
			Generator* gen;

			void operator()(const NodeTerm* term) const {
				gen->generate_term(term);
			}

			void operator()(const NodeBinExpr* bin_expr) const {
				gen->generate_bin_expr(bin_expr);
			}
		};
		
		ExprVisitor visitor{.gen = this};
		std::visit(visitor, expr->var);	
	}

	inline void generate_bin_expr(const NodeBinExpr* bin_expr) {
		struct BinExprVisitor
		{
			Generator* gen;

			void operator()(const NodeBinExprAdd* add) const {
				gen->generate_expr(add->rhs);
				gen->generate_expr(add->lhs);
				gen->pop("rax");
				gen->pop("rbx");

				gen->m_output << "    add rax, rbx" << '\n';
				gen->push("rax");
			}

			void operator()(const NodeBinExprMulti* multi) const {
				gen->generate_expr(multi->rhs);
				gen->generate_expr(multi->lhs);
				gen->pop("rax");
				gen->pop("rbx");

				gen->m_output << "    mul rbx" << '\n';
				gen->push("rax");
			}

			void operator()(const NodeBinExprSub* sub) const {
				gen->generate_expr(sub->rhs);
				gen->generate_expr(sub->lhs);
				gen->pop("rax");
				gen->pop("rbx");

				gen->m_output << "    sub rax, rbx" << '\n';
				gen->push("rax");
			}

			void operator()(const NodeBinExprDiv* fslash) const {
				gen->generate_expr(fslash->rhs);
				gen->generate_expr(fslash->lhs);
				gen->pop("rax");
				gen->pop("rbx");

				gen->m_output << "    fslash rbx" << '\n';
				gen->push("rax");
			}
		};

		BinExprVisitor visitor{.gen = this};
		std::visit(visitor, bin_expr->var);
	}

	//genrating assembly for STATEMENTS

	inline void generate_stmt(const NodeStmt* stmt) {
		struct StmtVisitor 
		{
			Generator* gen;

			void operator()(const NodeStmtExit* exit_stmt) const{
				
				gen->generate_expr(exit_stmt->expr);
				gen->m_output << "    mov rax, 60" << '\n';
				gen->pop("rdi");
				gen->m_output << "    syscall"     << '\n';
			}

			void operator()(const NodeStmtMake* make_stmt) const{
				
				std::string identifier = make_stmt->ident.value.value();
				if (gen->m_vars.contains(identifier))
				{
					std::cerr << "Cant reinitialize the variable '" << make_stmt->ident.value.value() << "'dumbass\n";
					exit(EXIT_FAILURE);
				}	
				
				gen->generate_expr(make_stmt->expr);
				gen->m_vars.insert(identifier, Var{.stack_loc = gen->m_stack_size});
			}

			void operator()(const NodeStmtAssign* assign) const {
				std::string identifier = assign->ident.value.value();
				if (!gen->m_vars.contains(identifier))
				{
					std::cerr << "Undeclared variable: '" << identifier << "'\n";
					exit(EXIT_FAILURE);
				}

				gen->generate_expr(assign->expr);
				gen->pop("rax");

				gen->m_output << "    mov [rsp + " 
				       	      << gen->var_offset(gen->m_vars.at(identifier).stack_loc)	
				       	      << "], rax" << '\n';	
			}

			void operator()(const NodeScope* scope) const{
				gen->begin_scope();

				for (const NodeStmt* stmt : scope->stmts)
					gen->generate_stmt(stmt);
				
				gen->end_scope();
			}

			void operator()(const NodeStmtIf* if_stmt) const {
				std::string end_label = gen->create_label();
				std::string label = gen->create_label();

				gen->generate_expr(if_stmt->expr);
				gen->pop("rax");
				
				gen->m_output << "    test rax, rax" << '\n';
				gen->m_output << "    jz " << label << '\n';

				gen->generate_stmt(if_stmt->stmt);
				gen->m_output << "    jmp " << end_label << '\n';
				gen->m_output << label << ":\n";

				if (if_stmt->chain.has_value())
				{
					gen->generate_if_chain(if_stmt->chain.value(), end_label);
				}	

				gen->m_output << end_label << ":\n";
			}
		};

		StmtVisitor visitor{.gen = this};
		std::visit(visitor, stmt->var);	
	}
	
	inline void generate_if_chain(NodeIfChain* chain, const std::string& label) {
		struct ChainVisitor 
		{
			const std::string& end_label;
			Generator* gen;
			
			ChainVisitor(Generator* p_gen, std::string p_label) : gen(p_gen), end_label(p_label) {}

			void operator()(const NodeChainElif* elif) const {
				
				std::string label = gen->create_label();

				gen->generate_expr(elif->expr);
				gen->pop("rax");
				
				gen->m_output << "    test rax, rax" << '\n';
				gen->m_output << "    jz " << label << '\n';

				gen->generate_stmt(elif->stmt);
				gen->m_output << "    jmp " << end_label << '\n';
				gen->m_output << label << ":\n";

				if (elif->chain.has_value())
				{
					gen->generate_if_chain(elif->chain.value(), end_label);
				}	
			}

			void operator()(const NodeChainElse* _else) {
				gen->generate_stmt(_else->stmt);
			}
		};

		ChainVisitor visitor(this, label);
		std::visit(visitor, chain->var);
	}
};
