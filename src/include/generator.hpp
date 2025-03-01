#pragma once

#include <iomanip>

#include "./mod_map.hpp"
#include "./parser.hpp"

class Generator {
public:
	inline Generator(NodeProg* prog)
		: m_prog(std::move(prog))
	{
	}	

	inline std::string gen_prog() {
		//Gen Text
		m_output << "section .text\n\tglobal _start\n_start:\n";
		for (const NodeStmt* stmt : m_prog->stmts)
		{
			gen_stmt(stmt);
		}
		m_output << '\n';
		m_output << "    mov rax, 60" << '\n';
		m_output << "    mov rdi, 0"  << '\n';
		m_output << "    syscall"     << '\n';

		//Gen Bss
		m_output << "\n\nsection .bss\n";
		if (m_byte_buffer)
			m_output << "    buf resb " << m_byte_buffer << '\n';

		//Gen Data
		m_output << "\n\nsection .data\n";
		for (const MsgData& data : m_Messages)
		{
			m_output << '\t' << data.msg_label
				 << " db"
				 << " '"
				 << data.msg
				 << "'";
			if (data.nl)
			{
				m_output << " , 0xA";
			}
			
			m_output << '\n';
		}

		return m_output.str();
	}
private:

	struct Var
	{
		size_t stack_loc;
		DataType type;	
	};

	struct MsgData 
	{
		std::string msg_label;
		std::string msg;
		bool nl;
	};

	//MEMBERS
	NodeProg* m_prog;
	std::stringstream m_output;
	
	size_t m_stack_size = 0;
	size_t m_labels = 1;
	
	std::vector<size_t>  m_scopes;
	std::vector<MsgData> m_Messages;

	Modded_map<Var> m_vars;
	TypeTable m_Table;

	int m_byte_buffer = 0;

	//UTILITY FUNCTIONS
	inline void stack(DataType type, char reg) {
		m_output << "    sub rsp, "  << m_Table[type].type_size   << '\n';
		
		m_output << "    mov "       << m_Table[type].size_asm    << " [rsp], "           
		       	                     << m_Table[type].getReg(reg) << '\n';

		m_stack_size += m_Table[type].type_size;
	}

	inline void unstack(DataType type, char reg) {
		m_output << "    mov " << m_Table[type].getReg(reg)
			 <<       ", " << m_Table[type].size_asm
			 <<   " [rsp]" << '\n';

		m_output << "    add rsp, " << m_Table[type].type_size << '\n';

		m_stack_size -= m_Table[type].type_size;
	}

	inline void begin_scope() {	
		m_scopes.push_back(m_stack_size);
	}

	inline void end_scope() {
		size_t pop_count = m_stack_size - m_scopes.back();
		for (int i = 0; i < pop_count; ) 
			i += m_Table[m_vars.pop_back().type].type_size;	
		
		m_scopes.pop_back();
		
		m_output << "    add rsp, " << pop_count << '\n';
		m_stack_size -= pop_count;
	}

	inline std::string create_label() {
		
		std::stringstream out;
		out << "label" << m_labels++;

		return out.str();
	}

	inline size_t var_offset(size_t loc) {
		return m_stack_size - loc;
	}

	inline void gen_lhs_rhs(NodeExpr* lhs, NodeExpr* rhs) {
		gen_expr(rhs);
		m_output << "    push rax" << '\n';
		m_stack_size += 8;
		gen_expr(lhs);
		m_output << "    pop rbx" << '\n';
		m_stack_size -= 8;
	}
	//generating assembly for EXPRESSIONS	
	
	inline void gen_term(const NodeTerm* term) {
		struct TermVisitor
		{	
			Generator* gen;

			void operator()(const NodeTermInt* int_term) const {
				gen->m_output << "    mov rax, " << int_term->int_lit.value.value() << '\n';
			}

			void operator()(const NodeTermChar* char_term) const {
				int ascii_value = (int)char_term->char_lit.value.value()[0];

				gen->m_output << "    mov rax, " << ascii_value << '\n';
			}

			void operator()(const NodeTermIdent* ident_term) const {
				std::string identifier = ident_term->ident.value.value();
				if (!gen->m_vars.contains(identifier))
				{
					std::cerr << "'" << identifier << "' was not declared\n";
					exit(EXIT_FAILURE);
				}
				
				size_t offset = gen->var_offset(gen->m_vars.at(identifier).stack_loc);
				DataType type = gen->m_vars.at(identifier).type;

				gen->m_output << "    mov " << gen->m_Table[type].getReg('a') 
					      <<      ", "  << gen->m_Table[type].size_asm 
					      <<    " [rsp";
				if (offset) {gen->m_output  << "+" << offset;}
				gen->m_output <<      "]"   << '\n';
				
				if (gen->m_Table[type].type_size < 4)
				{	
					int mask = BITMASK(gen->m_Table[type].type_size);
			        	gen->m_output << "    and rax, 0x" << std::hex << mask << '\n'; 
				}
			}

			void operator()(const NodeTermParen* paren_term) {
				gen->gen_expr(paren_term->expr);
			}
		};

		TermVisitor visitor{.gen = this};
		std::visit(visitor, term->var);
	}

	inline void gen_cmp_expr(const NodeBinExprCmp* cmp) {
		const std::string false_label = create_label();
		const std::string end_label = create_label();

		gen_lhs_rhs(cmp->lhs, cmp->rhs);
		
		m_output << "    cmp rax, rbx" << '\n';

		switch (cmp->cmp_op) 
		{
			case TokenType::g_than :
				m_output << "    jc " << false_label << '\n';
				m_output << "    je " << false_label << '\n';
				break;
			case TokenType::l_than :
				m_output << "    jnc " << false_label << '\n';
				break;
			case TokenType::eq_to :
				m_output << "    jne " << false_label << '\n';
				break;
			case TokenType::not_eq_to :
				m_output << "    je " << false_label << '\n';
				break;
		}
		
		m_output << "    mov rax, 1" << '\n';
		m_output << "    jmp " << end_label << '\n';
		m_output << false_label << ":\n";
		m_output << "    mov rax, 0" << '\n';
		m_output << end_label << ":\n";
	}

	inline void gen_expr(const NodeExpr* expr) {
		struct ExprVisitor 
		{
			Generator* gen;

			void operator()(const NodeTerm* term) const {
				gen->gen_term(term);
			}

			void operator()(const NodeBinExpr* bin_expr) const {
				gen->gen_bin_expr(bin_expr);
			}
		};
		
		ExprVisitor visitor{.gen = this};
		std::visit(visitor, expr->var);	
	}

	inline void gen_bin_expr(const NodeBinExpr* bin_expr) {
		struct BinExprVisitor
		{
			Generator* gen;

			void operator()(const NodeBinExprAdd* add) const {
				gen->gen_lhs_rhs(add->lhs, add->rhs);
				gen->m_output << "    add rax, rbx" << '\n';
			}

			void operator()(const NodeBinExprMulti* multi) const {
				gen->gen_lhs_rhs(multi->lhs, multi->rhs);
				gen->m_output << "    mul rbx" << '\n';
			}

			void operator()(const NodeBinExprSub* sub) const {
				gen->gen_lhs_rhs(sub->lhs, sub->rhs);
				gen->m_output << "    sub rax, rbx" << '\n';
			}

			void operator()(const NodeBinExprDiv* fslash) const {
				gen->gen_lhs_rhs(fslash->lhs, fslash->rhs);
				gen->m_output << "    div rbx" << '\n';
			}

			void operator()(const NodeBinExprCmp* cmp) const {
				gen->gen_cmp_expr(cmp);
			}
		};

		BinExprVisitor visitor{.gen = this};
		std::visit(visitor, bin_expr->var);
	}

	//genrating assembly for STATEMENTS

	inline void gen_stmt(const NodeStmt* stmt) {
		struct StmtVisitor 
		{
			Generator* gen;

			void operator()(const NodeStmtExit* exit_stmt) const{
				
				gen->gen_expr(exit_stmt->expr);
				gen->m_output << "    mov rdi, rax" << '\n'
					      << "    mov rax, 60"  << '\n'
					      << "    syscall"      << '\n';
			}

			void operator()(const NodeStmtDeclare* declare) const {
				std::string identifier = declare->ident.value.value();
				if (gen->m_vars.contains(identifier))
				{
					std::cerr << "Cannot Re-Declare variable: '" << identifier << "'\n";
					exit(EXIT_FAILURE);
				}

				gen->m_output << "    sub rsp, " << gen->m_Table[declare->type].type_size << '\n';
				
				gen->m_stack_size += gen->m_Table[declare->type].type_size;
				gen->m_vars.insert(identifier, Var{.stack_loc = gen->m_stack_size, .type = declare->type});
			}	

			void operator()(const NodeStmtAssign* assign) const {
				std::string identifier = assign->ident.value.value();
				if (!gen->m_vars.contains(identifier))
				{
					std::cerr << "Undeclared variable: '" << identifier << "'\n";
					exit(EXIT_FAILURE);
				}
				
				gen->gen_expr(assign->expr);

				DataType type = gen->m_vars.at(identifier).type;
				size_t offset = gen->var_offset(gen->m_vars.at(identifier).stack_loc);

				gen->m_output << "    mov " 
					      << gen->m_Table[type].size_asm
					      <<   " [rsp";
				if (offset) {gen->m_output  << "+" << offset;}
				gen->m_output <<      "], " 
					      << gen->m_Table[type].getReg('a') << '\n';
				
			}

			void operator()(const NodeScope* scope) const{
				gen->begin_scope();

				for (const NodeStmt* stmt : scope->stmts)
					gen->gen_stmt(stmt);
				
				gen->end_scope();
			}

			void operator()(const NodeStmtIf* if_stmt) const {
				std::string end_label = gen->create_label();
				std::string label = gen->create_label();

				gen->gen_expr(if_stmt->expr);
				
				gen->m_output << "    test rax, rax" << '\n';
				gen->m_output << "    jz " << label << '\n';

				gen->gen_stmt(if_stmt->stmt);
				gen->m_output << "    jmp " << end_label << '\n';
				gen->m_output << label << ":\n";

				if (if_stmt->chain.has_value())
				{
					gen->gen_if_chain(if_stmt->chain.value(), end_label);
				}	

				gen->m_output << end_label << ":\n";
			}

			void operator()(const NodeLoop* loop) const {
				std::string start_label = gen->create_label();
				std::string end_label = gen->create_label();
				
				gen->m_output << start_label << ":\n";

				gen->gen_expr(loop->expr);
				gen->m_output << "    test rax, rax" << '\n';
				gen->m_output << "    jz " << end_label << '\n';

				gen->gen_stmt(loop->scope);
				gen->m_output << "    jmp " << start_label << '\n';
				
				gen->m_output << end_label << ":\n";
			}

			void operator()(const NodeStmtWrite* write) const {
				gen->gen_stmt_write(write);
			}
		};

		StmtVisitor visitor{.gen = this};
		std::visit(visitor, stmt->var);	
	}

	inline void gen_stmt_write(const NodeStmtWrite* write) {
		struct WriteVisitor
		{
			Generator* gen;
			bool nl;
			WriteVisitor(Generator* p_gen, bool p_nl) : gen(p_gen), nl(p_nl) {}

			void operator()(const std::string& str) const {
				std::string msg_label = gen->create_label();
				gen->m_Messages.push_back({msg_label, str, nl});
				
				gen->m_output << "    mov rsi, " << msg_label 		       << '\n'
					      << "    mov rdx, " << str.length() + (nl ? 1 : 0) << '\n';
			}
			void operator()(const NodeExpr* expr) const {
				gen->m_byte_buffer = 5;
				gen->gen_expr(expr);
				
				std::string l1    = gen->create_label();
				std::string l2    = gen->create_label();
				std::string endl2 = gen->create_label();

				gen->m_output << "    mov rdi, 10"  << '\n'
					      << "    mov rcx, 0"   << '\n'
					      << "    mov rdx, 0"   << '\n'
					      
					      << l1 << ":\n"
					      << "    cmp rax, 0"   << '\n'
					      << "    jz " << l2 << '\n'
					      << "    div rdi"	    << '\n'
					      << "    add rdx, 0x30"<< '\n'
					      << "    push rdx"     << '\n' 
					      << "    mov rdx, 0"   << '\n'
					      << "    add rcx, 1"   << '\n'
					      << "    jmp " << l1   << '\n'

					      << l2 << ":\n"
					      << "    cmp rdx, rcx"           << '\n'
					      << "    jz " << endl2 	      << '\n'
					      << "    pop rax"                << '\n'
					      << "    mov byte [buf+rdx], al" << '\n'
					      << "    add rdx, 1"	      << '\n'
					      << "    jmp " << l2             << '\n'
					      << endl2 << ":\n"

					      << "    mov rsi, buf"	      << '\n';
				if (nl)
				{
					gen->m_output << "    mov byte [buf+rdx], 0xA" << '\n'
						      << "    add rdx, 1"	       << '\n';
				}
			}
		};

		WriteVisitor visitor(this, write->nl);
		std::visit(visitor, write->var);

		m_output << "    mov rax, 1" << '\n'
			 << "    mov rdi, 1" << '\n';
		m_output << "    syscall" << '\n';
	}
	
	inline void gen_if_chain(NodeIfChain* chain, const std::string& label) {
		struct ChainVisitor 
		{
			const std::string& end_label;
			Generator* gen;
			
			ChainVisitor(Generator* p_gen, std::string p_label) : gen(p_gen), end_label(p_label) {}

			void operator()(const NodeChainElif* elif) const {
				
				std::string label = gen->create_label();

				gen->gen_expr(elif->expr);
				
				gen->m_output << "    test rax, rax" << '\n';
				gen->m_output << "    jz " << label << '\n';

				gen->gen_stmt(elif->stmt);
				gen->m_output << "    jmp " << end_label << '\n';
				gen->m_output << label << ":\n";

				if (elif->chain.has_value())
				{
					gen->gen_if_chain(elif->chain.value(), end_label);
				}	
			}

			void operator()(const NodeChainElse* _else) {
				gen->gen_stmt(_else->stmt);
			}
		};

		ChainVisitor visitor(this, label);
		std::visit(visitor, chain->var);
	}
};
