#include "assembly.h"
#include "util.h"
#include <iostream>
#include <fstream>

#define PRINTLN(m) do{cout<<"[ASSEMBLY] "<<m<<endl;}while(0)
#define WARN(m) do{cout<<"[!ASSEMBL] WARNING: "<<m<<endl;}while(0)
#define INDENT "    "
// MEMORY ALLOC
#define GLOB_PREFIX     "GLOB_" // Global vars
#define STR_PREFIX      "STR_"   // Literal strings
// CONTROLLER
#define FUNC_PREFIX     "FUNC_"
#define LOOP_PREFIX     "LOOP_"
#define IF_PREFIX       "IF_"
#define ELSE_PREFIX     "ELSE_"
#define ENDFUNC_PREFIX  "ENDFUNC_"
#define ENDLOOP_PREFIX  "ENDLOOP_"
#define INTOLOOP_PREFIX "INTOLOOP_"
#define ENDIF_PREFIX    "ENDIF_"
#define INTOIF_PREFIX   "INTOIF_"
#define PROGRAM_END     "END_OF_PROGRAMME"
#define MAIN_END        "END_OF_MAIN:"

#define ist(n) starts_with(n, G_TEMPPREFIX)

SimpleAssembler::SimpleAssembler()
{
	this->print_log = false;
	this->print2file = false;
	this->cursor = 0;
	this->treg_cursor = 0;
	this->loop_count = 0;
	this->if_count = 0;
	this->str_count = 0;
	this->func_count = 0;
	this->tmp_count = 0;
}

void SimpleAssembler::debug_mode()
{
	this->print_log = true;
	this->print2file = true;
}

void SimpleAssembler::submit_mode()
{
	this->print_log = false;
	this->print2file = true;
}

void SimpleAssembler::clear_all()
{
	this->mips_text = vector<string>();
	this->mips_data = vector<string>();
	this->func_addr = map<string, int>();
	this->frame_size = map<string, int>();
	this->var_offset = map<string, map<string, int>*>();
	this->treg = vector<string>();
	this->tmap = map<string, int>();
	this->cursor = 0;
	this->treg_cursor = 0;
	this->loop_count = 0;
	this->if_count = 0;
	this->str_count = 0;
	this->func_count = 0;
	this->tmp_count = 0;
}

void SimpleAssembler::locate_func()
{
	for (int i = 0, state = 0; i < codes.size(); i++)
	{
		if (codes[i].get_type() == P_FUNC)
		{
			state = 1;
			func_addr.insert(pair<string, int>(to_lowercase(codes[i].get_header().first), i));
		}
		else if (codes[i].get_type() == P_ENDFUNC) state = 0;
		else if (state == 0 && codes[i].get_type() != P_FUNC)
		{
			func_addr.insert(pair<string, int>(to_lowercase(G_MAINSCOPE), i));
			return;
		}
	}
}

void SimpleAssembler::locate_globals()
{
	map<string, vector<Var>*>::iterator it = symtable.find(to_lowercase(G_GLOBALSCOPE));
	if (it == symtable.end()) return;
	vector<Var> vars = *(it->second);
	for (int i = 0; i < vars.size(); i++)
	{
		if (vars[i].is_const()) continue;
		if (vars[i].get_dim().size() == 0) this->_locate_var(vars[i]);
		else if (vars[i].get_dim().size() == 1) this->_locate_arr1(vars[i]);
		else if (vars[i].get_dim().size() == 2) this->_locate_arr2(vars[i]);
	}
}

void SimpleAssembler::_locate_var(Var v)
{
	if (v.has_value())
		mips_data.push_back(GLOB_PREFIX + to_lowercase(v.get_name()) + ": .word " + to_string(v.get_value()));
	else mips_data.push_back(GLOB_PREFIX + to_lowercase(v.get_name()) + ": .word 0");
}

void SimpleAssembler::_locate_arr1(Var v)
{
	string data = GLOB_PREFIX + to_lowercase(v.get_name()) + ": .word ";
	if (v.has_value())
		for (int i = 0; i < v.get_dim()[0]; i++)
			data += to_string(v.get_value(i)) + " ";
	else for (int i = 0; i < v.get_dim()[0]; i++) data += "0 ";
	mips_data.push_back(data);
}

void SimpleAssembler::_locate_arr2(Var v)
{
	string data = GLOB_PREFIX + to_lowercase(v.get_name()) + ": .word ";
	if (v.has_value())
		for (int i = 0; i < v.get_dim()[0] * v.get_dim()[1]; i++)
			data += to_string(v.get_value(i)) + " ";
	else for (int i = 0; i < v.get_dim()[0] * v.get_dim()[1]; i++)
		data += "0 ";
	mips_data.push_back(data);
}

int SimpleAssembler::lookup_func(string name)
{
	map<string, int>::iterator it = func_addr.find(to_lowercase(name));
	if (it == func_addr.end()) return -1;
	else return it->second;
}

int SimpleAssembler::lookup_pair(Pseudo p, int offset)
{
	if (p.get_type() == P_FUNC) return _lookup_pair(offset, P_FUNC, P_ENDFUNC);
	else if (p.get_type() == P_LOOP) return _lookup_pair(offset, P_LOOP, P_ENDLOOP);
	else if (p.get_type() == P_IF) return _lookup_pair(offset, P_IF, P_ENDIF);
	else return -1;
}

int SimpleAssembler::_lookup_pair(int offset, string positive, string negative)
{
	for (int i = offset, depth = 0; i < codes.size(); i++)
	{
		if (codes[i].get_type() == positive) depth++;
		else if (codes[i].get_type() == negative) depth--;
		if (codes[i].get_type() == negative && depth == 0) return i;
	}
	return -1;
}

int SimpleAssembler::lookup_paired_else(int offset)
{
	for (int i = offset + 1; i < codes.size(); i++)
	{
		if (codes[i].get_type() == P_IF) i = lookup_pair(codes[i], i);
		if (codes[i].get_type() == P_ELSE) return i;
	}
	return -1;
}

void SimpleAssembler::configure_stackframe()
{
	for (map<string, vector<Var>*>::iterator it = symtable.begin(); it != symtable.end(); it++)
	{
		//if (it->first == to_lowercase(G_GLOBALSCOPE)) continue;
		int size = 0;
		vector<Var> vars = *(it->second);
		map<string, int>* syms = new map<string, int>();
		for (int i = 0; i < codes.size(); i++)
		{
			if (codes[i].get_type() == P_FUNC && \
				to_lowercase(codes[i].get_header().first) == to_lowercase(it->first))
			{
				vector<P> args = codes[i].get_args();
				for (int i = 0; i < args.size(); i++)
				{
					syms->insert(pair<string, int>(to_lowercase(args[i].first), size));
					size += 1;
				}
				break;
			}
		}
		for (int i = 0; i < vars.size(); i++)
		{
			if (vars[i].is_const()) continue;
			syms->insert(pair<string, int>(to_lowercase(vars[i].get_name()), size));
			if (vars[i].get_dim().size() == 0) size += 1;
			else if (vars[i].get_dim().size() == 1) size += vars[i].get_dim()[0];
			else size += vars[i].get_dim()[0] * vars[i].get_dim()[1];
		}
		this->frame_size.insert(pair<string, int>(it->first, size));
		this->var_offset.insert(pair<string, map<string, int>*>(it->first, syms));
	}
	for (int i = 0; i < codes.size(); i++)
	{
		if (codes[i].get_type() == P_FUNC && \
			frame_size.find(to_lowercase(codes[i].get_header().first)) == frame_size.end())
		{
			vector<P> args = codes[i].get_args();
			map<string, int>* syms = new map<string, int>();
			for (int j = 0; j < args.size(); j++)
				syms->insert(pair<string, int>(to_lowercase(args[j].first), j));
			this->frame_size.insert(pair<string, int>(to_lowercase(codes[i].get_header().first), args.size()));
			this->var_offset.insert(pair<string, map<string, int>*>(to_lowercase(codes[i].get_header().first), syms));
		}
	}
}

void SimpleAssembler::print_stack_desc()
{
	PRINTLN("+==========+");
	PRINTLN("|STACK DESC|");
	PRINTLN("+==========+");
	for (map<string, vector<Var>*>::iterator it = symtable.begin(); it != symtable.end(); it++)
	{
		if (it->first == to_lowercase(G_GLOBALSCOPE)) continue;
		PRINTLN(">>>FUNCTION - " << it->first << "<<<");
		PRINTLN("- STACK SIZE: " << frame_size.find(it->first)->second);
		PRINTLN("- STACK LAYOUT:");
		map<string, int> syms = *(var_offset.find(it->first)->second);
		for (map<string, int>::iterator it_ = syms.begin(); it_ != syms.end(); it_++)
			PRINTLN(it_->first << ": " << it_->second);
	}
}

int SimpleAssembler::lookup_treg(string name)
{
	for (int i = 0; i < treg.size(); i++)
		if (to_lowercase(name) == to_lowercase(treg[i]))
			return i;
	map<string, int>::iterator it = this->tmap.find(name);
	if (it == tmap.end()) return dist_treg(name);
	else
	{
		treg_cursor = (treg_cursor + 1) % 10;
		map<string, int>::iterator it2 = this->tmap.find(treg[treg_cursor]);
		if (it2 == tmap.end())
		{
			this->mips_text.push_back("sw $t" + to_string(treg_cursor) + ", " + \
				to_string(4 * tmp_count) + "($gp)");
			tmap.insert(pair<string, int>(treg[treg_cursor], tmp_count++));
		}
		else this->mips_text.push_back("sw $t" + to_string(treg_cursor) + ", " + \
			to_string(4 * it2->second) + "($gp)");
		this->mips_text.push_back("lw $t" + to_string(treg_cursor) + ", " + \
			to_string(4 * it->second) + "($gp)");
		treg[treg_cursor] = name;
		return treg_cursor;
	}
}

int SimpleAssembler::lookup_sp_offset(string name)
{
	map<string, map<string, int>*>::iterator it = var_offset.find(to_lowercase(current_scope));
	if (it == var_offset.end()) return -1;
	map<string, int> syms = *(it->second);
	if (syms.find(to_lowercase(name)) == syms.end()) return -1;
	else return syms.find(to_lowercase(name))->second;
}

// This is not compact, pray for it not resulting bugs
// now it caused bugs, fix it
// fixed
int SimpleAssembler::dist_treg(string name)
{
	if (treg.size() < 10)
	{
		treg.push_back(name);
		return treg.size() - 1;
	}
	else
	{
#ifdef OPTIMIZE_1
		for (int i = 0; i < treg.size(); i++)
		{
			if (treg[i] == "")
			{
				treg[i] = name;
				return i;
			}
		}
#endif // OPTIMIZE_1
		treg_cursor = (treg_cursor + 1) % 10;
		map<string, int>::iterator it2 = this->tmap.find(treg[treg_cursor]);
		if (it2 == tmap.end())
		{
			this->mips_text.push_back("sw $t" + to_string(treg_cursor) + ", " + \
				to_string(4 * tmp_count) + "($gp)");
			tmap.insert(pair<string, int>(treg[treg_cursor], tmp_count++));
		}
		else this->mips_text.push_back("sw $t" + to_string(treg_cursor) + ", " + \
			to_string(4 * it2->second) + "($gp)");
		treg[treg_cursor] = name;
		return treg_cursor;
	}
}

#ifdef OPTIMIZE_1
void SimpleAssembler::free_treg(string name)
{
	if (!ist(name)) return;
	for (int i = 0; i < treg.size(); i++)
	{
		if (treg[i] == name)
		{
			treg[i] = "";
		}
	}
}
#endif // OPTIMIZE_1

string SimpleAssembler::load_name(string name, string target)
{
	if (ist(name) && lookup_treg(name) != -1)
		return "$t" + to_string(lookup_treg(name));
	else if (ist(name)) WARN("Register not hit!");
	int offset = this->lookup_sp_offset(name);
	if (offset != -1)
		this->mips_text.push_back("lw " + target + ", " + GLOB_PREFIX + to_lowercase(name) + "($0)");
	else this->mips_text.push_back("lw " + target + ", " + to_string(offset * 4) + "($sp)");
	return target;
}

void SimpleAssembler::assemble(vector<Pseudo> codes, map<string, vector<Var>*> symtable)
{
	this->clear_all();
	this->codes = codes;
	this->symtable = symtable;
	this->locate_func();
	this->locate_globals();
	this->configure_stackframe();
	this->_assemble();
	this->after_assembly();
}

void SimpleAssembler::after_assembly()
{
	if (print_log)
	{
		this->print_stack_desc();
		PRINTLN("+===========+");
		PRINTLN("|MIPS OUTPUT|");
		PRINTLN("+===========+");
		PRINTLN(".data");
		for (int i = 0; i < mips_data.size(); i++)
			PRINTLN(mips_data[i]);
		PRINTLN(".text");
		for (int i = 0; i < mips_text.size(); i++)
			PRINTLN(mips_text[i]);
	}
	if (print2file)
	{
		ofstream fout("mips.txt");
		if (fout.is_open())
		{
			int indent = 0;
			fout << ".data" << endl;
			for (int i = 0; i < mips_data.size(); i++)
				fout << INDENT << mips_data[i] << endl;
			fout << ".text" << endl;
			for (int i = 0; i < mips_text.size(); i++)
			{
				string type = mips_text[i];
				if (starts_with(type, FUNC_PREFIX) || starts_with(type, IF_PREFIX) || starts_with(type, LOOP_PREFIX))
				{
					for (int j = 0; j < indent + 1; j++) fout << INDENT;
					fout << mips_text[i] << endl;
					indent++;
				}
				else if (starts_with(type, ENDIF_PREFIX) || starts_with(type, ENDLOOP_PREFIX))
				{
					indent--;
					for (int j = 0; j < indent + 1; j++) fout << INDENT;
					fout << mips_text[i] << endl;
				}
				else if (starts_with(type, ENDFUNC_PREFIX))
				{
					indent--;
					for (int j = 0; j < indent + 1; j++) fout << INDENT;
					fout << mips_text[i] << endl << endl;
				}
				else if (starts_with(type, INTOIF_PREFIX) || starts_with(type, INTOLOOP_PREFIX) || starts_with(type, ELSE_PREFIX))
				{
					indent--;
					for (int j = 0; j < indent + 1; j++) fout << INDENT;
					fout << mips_text[i] << endl;
					indent++;
				}
				else if (type == MAIN_END)
				{
					for (int j = 0; j < indent + 1; j++) fout << INDENT;
					fout << mips_text[i] << endl << endl;
					indent--;
				}
				else
				{
					for (int j = 0; j < indent + 1; j++) fout << INDENT;
					fout << mips_text[i] << endl;
				}
			}
			fout.close();
		}
	}
}

void SimpleAssembler::_assemble()
{
	this->current_scope = G_MAINSCOPE;
	this->mips_text.push_back(string(FUNC_PREFIX) + G_MAINSCOPE + ":");
	int entrance = this->lookup_func(G_MAINSCOPE);
	if (entrance == -1) return;
	this->cursor = entrance;
	// Initialize variables
	this->initialize_frame(G_MAINSCOPE, true);
	while (cursor < codes.size())
	{
		_assemble_single(codes[cursor]);
		cursor++;
	}
	this->mips_text.push_back("j " + string(PROGRAM_END));
	this->mips_text.push_back(MAIN_END);
	_assemble_functions();
	this->mips_text.push_back(string() + PROGRAM_END + ":");
}

void SimpleAssembler::_assemble_single(Pseudo code)
{
	string type = code.get_type();
	if (code.get_type() != P_COMN) this->mips_text.push_back("# " + code.to_string());
	if (type == P_ADD)       _add(code);
	else if (type == P_SUB)  _sub(code);
	else if (type == P_MUL)  _mul(code);
	else if (type == P_DIV)  _div(code);
	else if (type == P_ARR)  _arr(code);
	else if (type == P_STARTLOOP) _loop(code);
	else if (type == P_IF)   _if(code);
	else if (type == P_RET)  _ret(code);
	else if (type == P_RETN) _retn(code);
	else if (type == P_CALL) _call(code);
	else if (type == P_CALN) _caln(code);
	else if (type == P_ASSN) _assn(code);
	else if (type == P_ASSR) _assr(code);
	else if (type == P_READ) _read(code);
	else if (type == P_DISP) _disp(code);
}

P SimpleAssembler::lchar2lint(P p)
{
	return pair<string, string>(to_string((int)p.first[0]), P_LINT);
}

void SimpleAssembler::_add(Pseudo code)
{
	P opr1 = code.get_opr1().second == P_LCHAR ? lchar2lint(code.get_opr1()) : code.get_opr1();
	P opr2 = code.get_opr2().second == P_LCHAR ? lchar2lint(code.get_opr2()) : code.get_opr2();
START_ADD:
	if (opr1.second == P_LINT && opr2.second == P_LINT)
	{
		int result = stoi(opr1.first) + stoi(opr2.first);
		if (ist(code.get_result()))
			this->mips_text.push_back("li $t" + to_string(lookup_treg(code.get_result())) + ", " + to_string(result));
		else
		{
			int offset = lookup_sp_offset(code.get_result());
			this->mips_text.push_back("li $s0, " + to_string(result));
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
	}
	else if (opr1.second == P_LINT && opr2.second != P_LINT)
	{
		P tmp = opr1;
		opr1 = opr2;
		opr2 = tmp;
		goto START_ADD;
	}
	else if (opr1.second != P_LINT && opr2.second == P_LINT)
	{
		if (ist(opr1.first) && ist(code.get_result()))
			this->mips_text.push_back("addiu $t" + to_string(lookup_treg(code.get_result())) + ", " + \
				"$t" + to_string(lookup_treg(opr1.first)) + ", " + opr2.first);
		else if (ist(opr1.first))
		{
			this->mips_text.push_back("addiu $s0, $t" + to_string(lookup_treg(opr1.first)) + ", " + opr2.first);
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
		else if (ist(code.get_result()))
		{
			int offset = lookup_sp_offset(opr1.first);
			if (offset == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset) + "($sp)");
			this->mips_text.push_back("addiu $t" + to_string(lookup_treg(code.get_result())) + ", $s0, " + opr2.first);
		}
		else
		{
			int offset1 = lookup_sp_offset(opr1.first);
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("addiu $s1, $s0, " + opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
		}
	}
	else if (opr1.second != P_LINT && opr2.second != P_LINT)
	{
		if (ist(opr1.first) && ist(opr2.first) && ist(code.get_result()))
			this->mips_text.push_back("addu $t" + to_string(lookup_treg(code.get_result())) + ", $t" +\
				to_string(lookup_treg(opr1.first)) + ", $t" + to_string(lookup_treg(opr2.first)));
		else if (!ist(opr1.first) && ist(opr2.first) && ist(code.get_result()))
		{
			int offset = lookup_sp_offset(opr1.first);
			if (offset == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset) + "($sp)");
			this->mips_text.push_back("addu $t" + to_string(lookup_treg(code.get_result())) + ", $t" + \
				to_string(lookup_treg(opr2.first)) + ", $s0");
		}
		else if (ist(opr1.first) && !ist(opr2.first) && ist(code.get_result()))
		{
			P tmp = opr1;
			opr1 = opr2;
			opr2 = tmp;
			goto START_ADD;
		}
		else if (!ist(opr1.first) && !ist(opr2.first) && ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			int offset2 = lookup_sp_offset(opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset2) + "($sp)");
			this->mips_text.push_back("addu $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $s1");
		}
		else if (ist(opr1.first) && ist(opr2.first) && !ist(code.get_result()))
		{
			this->mips_text.push_back("addu $s0, $t" + to_string(lookup_treg(opr1.first)) + \
				", $t" + to_string(lookup_treg(opr2.first)));
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
		else if (!ist(opr1.first) && ist(opr2.first) && !ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back(string("addu $s0, $s1, $t") + to_string(lookup_treg(opr2.first)));
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
		else if (ist(opr1.first) && !ist(opr2.first) && !ist(code.get_result()))
		{
			P tmp = opr1;
			opr1 = opr2;
			opr2 = tmp;
			goto START_ADD;
		}
		else
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			int offset2 = lookup_sp_offset(opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset2) + "($sp)");
			this->mips_text.push_back("addu $s2, $s1, $s0");
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s2, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s2, " + to_string(4 * offset) + "($sp)");
		}
	}
#ifdef OPTIMIZE_1
	free_treg(opr1.first);
	free_treg(opr2.first);
#endif // OPTIMIZE_1
}

void SimpleAssembler::_sub(Pseudo code)
{
	P opr1 = code.get_opr1().second == P_LCHAR ? lchar2lint(code.get_opr1()) : code.get_opr1();
	P opr2 = code.get_opr2().second == P_LCHAR ? lchar2lint(code.get_opr2()) : code.get_opr2();
	if (opr1.second == P_LINT && opr2.second == P_LINT)
	{
		int result = stoi(opr1.first) - stoi(opr2.first);
		if (ist(code.get_result()))
			this->mips_text.push_back("li $t" + to_string(lookup_treg(code.get_result())) + ", " + to_string(result));
		else
		{
			int offset = lookup_sp_offset(code.get_result());
			this->mips_text.push_back("li $s0, " + to_string(result));
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
	}
	else if (opr1.second == P_LINT && opr2.second != P_LINT)
	{
		if (ist(opr2.first) && ist(code.get_result()))
		{
			this->mips_text.push_back("li $s0, " + opr1.first);
			this->mips_text.push_back("subu $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $t" + \
				to_string(lookup_treg(opr2.first)));
		}
		else if (ist(opr2.first))
		{
			this->mips_text.push_back("li $s0, " + opr1.first);
			this->mips_text.push_back("subu $s1, $s0, $t" + to_string(lookup_treg(opr2.first)));
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset) + "($sp)");
		}
		else if (ist(code.get_result()))
		{
			this->mips_text.push_back("li $s1, " + opr1.first);
			int offset = lookup_sp_offset(opr2.first);
			if (offset == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset) + "($sp)");
			this->mips_text.push_back("subu $t" + to_string(lookup_treg(code.get_result())) + ", $s1, $s0");
		}
		else
		{
			int offset1 = lookup_sp_offset(opr2.first);
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("li $s2, " + opr1.first);
			this->mips_text.push_back("subu $s1, $s2, $s0");
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
		}
	}
	else if (opr1.second != P_LINT && opr2.second == P_LINT)
	{
		if (ist(opr1.first) && ist(code.get_result()))
			this->mips_text.push_back("subiu $t" + to_string(lookup_treg(code.get_result())) + ", " + \
				"$t" + to_string(lookup_treg(opr1.first)) + ", " + opr2.first);
		else if (ist(opr1.first))
		{
			this->mips_text.push_back("subiu $s1, $t" + to_string(lookup_treg(opr1.first)) + ", " + opr2.first);
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset) + "($sp)");
		}
		else if (ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("subiu $t" + to_string(lookup_treg(code.get_result())) + ", $s0, " + opr2.first);
		}
		else
		{
			int offset1 = lookup_sp_offset(opr1.first);
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("subiu $s1, $s0, " + opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
		}
	}
	else if (opr1.second != P_LINT && opr2.second != P_LINT)
	{
		if (ist(opr1.first) && ist(opr2.first) && ist(code.get_result()))
			this->mips_text.push_back("subu $t" + to_string(lookup_treg(code.get_result())) + ", $t" + \
				to_string(lookup_treg(opr1.first)) + ", $t" + to_string(lookup_treg(opr2.first)));
		else if (!ist(opr1.first) && ist(opr2.first) && ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("subu $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $t" + \
				to_string(lookup_treg(opr2.first)));
		}
		else if (ist(opr1.first) && !ist(opr2.first) && ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr2.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("subu $t" + to_string(lookup_treg(code.get_result())) + ", $t" + \
				to_string(lookup_treg(opr1.first)) + ", $s0");
		}
		else if (!ist(opr1.first) && !ist(opr2.first) && ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			int offset2 = lookup_sp_offset(opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset2) + "($sp)");
			this->mips_text.push_back("subu $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $s1");
		}
		else if (ist(opr1.first) && ist(opr2.first) && !ist(code.get_result()))
		{
			this->mips_text.push_back("subu $s1, $t" + to_string(lookup_treg(opr1.first)) + ", $t" + \
				to_string(lookup_treg(opr2.first)));
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
		}
		else if (!ist(opr1.first) && ist(opr2.first) && !ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("subu $s0, $s1, $t" + to_string(lookup_treg(opr2.first)));
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset2) + "($sp)");
		}
		else if (ist(opr1.first) && !ist(opr2.first) && !ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr2.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("subu $s0, $t" + to_string(lookup_treg(opr1.first)) + ", $s1");
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset2) + "($sp)");
		}
		else
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			int offset2 = lookup_sp_offset(opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset2) + "($sp)");
			this->mips_text.push_back("subu $s2, $s0, $s1");
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s2, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s2, " + to_string(4 * offset) + "($sp)");
		}
	}
#ifdef OPTIMIZE_1
	free_treg(opr1.first);
	free_treg(opr2.first);
#endif // OPTIMIZE_1
}

#ifdef OPTIMIZE_2
void SimpleAssembler::mult_2_power(Pseudo code, P opr1, P opr2)
{
	int shift_bits = log2_(stoi(opr2.first));
	if (ist(opr1.first) && ist(code.get_result()))
	{
		this->mips_text.push_back("sll $t" + to_string(lookup_treg(code.get_result())) + ", " + \
			"$t" + to_string(lookup_treg(opr1.first)) + ", " + to_string(shift_bits));
	}
	else if (ist(opr1.first))
	{
		this->mips_text.push_back("sll $s0, $t" + to_string(lookup_treg(opr1.first)) + ", " + to_string(shift_bits));
		int offset = lookup_sp_offset(code.get_result());
		if (offset == -1)
			this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
		else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
	}
	else if (ist(code.get_result()))
	{
		int offset = lookup_sp_offset(opr1.first);
		if (offset == -1)
			this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
		else this->mips_text.push_back("lw $s0, " + to_string(4 * offset) + "($sp)");
		this->mips_text.push_back("sll $t" + to_string(lookup_treg(code.get_result())) + ", $s0, " + to_string(shift_bits));
	}
	else
	{
		int offset1 = lookup_sp_offset(opr1.first);
		int offset2 = lookup_sp_offset(code.get_result());
		if (offset1 == -1)
			this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
		else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
		this->mips_text.push_back("sll $s1, $s0, " + to_string(shift_bits));
		if (offset2 == -1)
			this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
		else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
	}
}
#endif // OPTIMIZE_2

void SimpleAssembler::_mul(Pseudo code)
{
	P opr1 = code.get_opr1().second == P_LCHAR ? lchar2lint(code.get_opr1()) : code.get_opr1();
	P opr2 = code.get_opr2().second == P_LCHAR ? lchar2lint(code.get_opr2()) : code.get_opr2();
START_MUL:
	if (opr1.second == P_LINT && opr2.second == P_LINT)
	{
		int result = stoi(opr1.first) * stoi(opr2.first);
		if (ist(code.get_result()))
			this->mips_text.push_back("li $t" + to_string(lookup_treg(code.get_result())) + ", " + to_string(result));
		else
		{
			int offset = lookup_sp_offset(code.get_result());
			this->mips_text.push_back("li $s0, " + to_string(result));
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
	}
	else if (opr1.second == P_LINT && opr2.second != P_LINT)
	{
		P tmp = opr1;
		opr1 = opr2;
		opr2 = tmp;
		goto START_MUL;
	}
	else if (opr1.second != P_LINT && opr2.second == P_LINT)
	{
#ifdef OPTIMIZE_2
		if (is_2_pow(stoi(opr2.first)))
		{
			mult_2_power(code, opr1, opr2);
			return;
		}
#endif // OPTIMIZE_2
		if (ist(opr1.first) && ist(code.get_result()))
		{
			this->mips_text.push_back("li $s0, " + opr2.first);
			this->mips_text.push_back("mul $t" + to_string(lookup_treg(code.get_result())) + ", " + \
				"$t" + to_string(lookup_treg(opr1.first)) + ", $s0");
		}
		else if (ist(opr1.first))
		{
			this->mips_text.push_back("li $s1, " + opr2.first);
			this->mips_text.push_back("mul $s0, $t" + to_string(lookup_treg(opr1.first)) + ", $s1");
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
		else if (ist(code.get_result()))
		{
			int offset = lookup_sp_offset(opr1.first);
			if (offset == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset) + "($sp)");
			this->mips_text.push_back("li $s1, " + opr2.first);
			this->mips_text.push_back("mul $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $s1");
		}
		else
		{
			int offset1 = lookup_sp_offset(opr1.first);
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("li $s2, " + opr2.first);
			this->mips_text.push_back("mul $s1, $s0, $s2");
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
		}
	}
	else if (opr1.second != P_LINT && opr2.second != P_LINT)
	{
		if (ist(opr1.first) && ist(opr2.first) && ist(code.get_result()))
			this->mips_text.push_back("mul $t" + to_string(lookup_treg(code.get_result())) + ", $t" + \
				to_string(lookup_treg(opr1.first)) + ", $t" + to_string(lookup_treg(opr2.first)));
		else if (!ist(opr1.first) && ist(opr2.first) && ist(code.get_result()))
		{
			int offset = lookup_sp_offset(opr1.first);
			if (offset == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset) + "($sp)");
			this->mips_text.push_back("mul $t" + to_string(lookup_treg(code.get_result())) + ", $t" + \
				to_string(lookup_treg(opr2.first)) + ", $s0");
		}
		else if (ist(opr1.first) && !ist(opr2.first) && ist(code.get_result()))
		{
			P tmp = opr1;
			opr1 = opr2;
			opr2 = tmp;
			goto START_MUL;
		}
		else if (!ist(opr1.first) && !ist(opr2.first) && ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			int offset2 = lookup_sp_offset(opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset2) + "($sp)");
			this->mips_text.push_back("mul $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $s1");
		}
		else if (ist(opr1.first) && ist(opr2.first) && !ist(code.get_result()))
		{
			this->mips_text.push_back("mul $s0, $t" + to_string(lookup_treg(opr1.first)) + \
				", $t" + to_string(lookup_treg(opr2.first)));
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
		else if (!ist(opr1.first) && ist(opr2.first) && !ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back(string("mul $s0, $s1, $t") + to_string(lookup_treg(opr2.first)));
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
		else if (ist(opr1.first) && !ist(opr2.first) && !ist(code.get_result()))
		{
			P tmp = opr1;
			opr1 = opr2;
			opr2 = tmp;
			goto START_MUL;
		}
		else
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			int offset2 = lookup_sp_offset(opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset2) + "($sp)");
			this->mips_text.push_back("mul $s2, $s1, $s0");
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s2, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s2, " + to_string(4 * offset) + "($sp)");
		}
	}
#ifdef OPTIMIZE_1
	free_treg(opr1.first);
	free_treg(opr2.first);
#endif // OPTIMIZE_1
}

#ifdef OPTIMIZE_4
void SimpleAssembler::div_2_power(Pseudo code, P opr1, P opr2)
{
	int shift_bits = log2_(stoi(opr2.first));
	if (ist(opr1.first) && ist(code.get_result()))
	{
		this->mips_text.push_back("move $s0, $t" + to_string(lookup_treg(opr1.first)));
		this->mips_text.push_back("bgz $s0, " + DIV_PREFIX + to_string(div_count));
		this->mips_text.push_back("addiu $s0, $s0, " + to_string(stoi(opr2.first) - 1));
		this->mips_text.push_back(DIV_PREFIX + to_string(div_count) + ":");
		this->mips_text.push_back("sra $t" + to_string(lookup_treg(code.get_result())) + ", " + \
			"$s0, " + to_string(shift_bits));
	}
	else if (ist(opr1.first))
	{
		this->mips_text.push_back("move $s0, $t" + to_string(lookup_treg(opr1.first)));
		this->mips_text.push_back("bgz $s0, " + DIV_PREFIX + to_string(div_count));
		this->mips_text.push_back("addiu $s0, $s0, " + to_string(stoi(opr2.first) - 1));
		this->mips_text.push_back(DIV_PREFIX + to_string(div_count) + ":");
		this->mips_text.push_back("sra $s1, $s0, " + to_string(shift_bits));
		int offset = lookup_sp_offset(code.get_result());
		if (offset == -1)
			this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
		else this->mips_text.push_back("sw $s1, " + to_string(4 * offset) + "($sp)");
	}
	else if (ist(code.get_result()))
	{
		int offset1 = lookup_sp_offset(opr1.first);
		if (offset1 == -1)
			this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
		else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
		this->mips_text.push_back("bgz $s0, " + DIV_PREFIX + to_string(div_count));
		this->mips_text.push_back("addiu $s0, $s0, " + to_string(stoi(opr2.first) - 1));
		this->mips_text.push_back(DIV_PREFIX + to_string(div_count) + ":");
		this->mips_text.push_back("sra $t" + to_string(lookup_treg(code.get_result())) + ", " + \
			"$s1, " + to_string(shift_bits));
	}
	else
	{
		int offset1 = lookup_sp_offset(opr1.first);
		int offset2 = lookup_sp_offset(code.get_result());
		if (offset1 == -1)
			this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
		else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
		this->mips_text.push_back("bgz $s0, " + DIV_PREFIX + to_string(div_count));
		this->mips_text.push_back("addiu $s0, $s0, " + to_string(stoi(opr2.first) - 1));
		this->mips_text.push_back(DIV_PREFIX + to_string(div_count) + ":");
		this->mips_text.push_back("sra $s1, $s0, " + to_string(shift_bits));
		if (offset2 == -1)
			this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
		else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
	}
	div_count++;
}
#endif // OPTIMIZE_4

void SimpleAssembler::_div(Pseudo code)
{
	P opr1 = code.get_opr1().second == P_LCHAR ? lchar2lint(code.get_opr1()) : code.get_opr1();
	P opr2 = code.get_opr2().second == P_LCHAR ? lchar2lint(code.get_opr2()) : code.get_opr2();
	if (opr1.second == P_LINT && opr2.second == P_LINT)
	{
		int result = stoi(opr1.first) / stoi(opr2.first);
		if (ist(code.get_result()))
			this->mips_text.push_back("li $t" + to_string(lookup_treg(code.get_result())) + ", " + to_string(result));
		else
		{
			int offset = lookup_sp_offset(code.get_result());
			this->mips_text.push_back("li $s0, " + to_string(result));
			if (offset == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset) + "($sp)");
		}
	}
	else if (opr1.second == P_LINT && opr2.second != P_LINT)
	{
		if (ist(opr2.first) && ist(code.get_result()))
			this->mips_text.push_back("div $t" + to_string(lookup_treg(code.get_result())) + ", " + \
				"$t" + to_string(lookup_treg(opr2.first)) + ", " + opr1.first);
		else if (ist(opr2.first))
		{
			this->mips_text.push_back("li $s0, " + opr1.first);
			this->mips_text.push_back("div $s1, $s0, $t" + to_string(lookup_treg(opr2.first)));
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset) + "($sp)");
		}
		else if (ist(code.get_result()))
		{
			this->mips_text.push_back("li $s1, " + opr1.first);
			int offset = lookup_sp_offset(opr2.first);
			if (offset == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset) + "($sp)");
			this->mips_text.push_back("div $t" + to_string(lookup_treg(code.get_result())) + ", $s1, $s0");
		}
		else
		{
			int offset1 = lookup_sp_offset(opr2.first);
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("li $s2, " + opr1.first);
			this->mips_text.push_back("div $s1, $s2, $s0");
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
		}
	}
	else if (opr1.second != P_LINT && opr2.second == P_LINT)
	{
#ifdef OPTIMIZE_4
		if (is_2_pow(stoi(opr2.first)))
		{
			div_2_power(code, opr1, opr2);
			return;
		}
#endif // OPTIMIZE_4
		if (ist(opr1.first) && ist(code.get_result()))
			this->mips_text.push_back("div $t" + to_string(lookup_treg(code.get_result())) + ", " + \
				"$t" + to_string(lookup_treg(opr1.first)) + ", " + opr2.first);
		else if (ist(opr1.first))
		{
			this->mips_text.push_back("li $s0, " + opr2.second);
			this->mips_text.push_back("div $s1, $t" + to_string(lookup_treg(opr1.first)) + ", $s0");
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset) + "($sp)");
		}
		else if (ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("li $s1, " + opr2.first);
			this->mips_text.push_back("div $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $s1");
		}
		else
		{
			int offset1 = lookup_sp_offset(opr1.first);
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("li $s2, " + opr2.first);
			this->mips_text.push_back("div $s1, $s0, $s2");
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
		}
	}
	else if (opr1.second != P_LINT && opr2.second != P_LINT)
	{
		if (ist(opr1.first) && ist(opr2.first) && ist(code.get_result()))
			this->mips_text.push_back("div $t" + to_string(lookup_treg(code.get_result())) + ", $t" + \
				to_string(lookup_treg(opr1.first)) + ", $t" + to_string(lookup_treg(opr2.first)));
		else if (!ist(opr1.first) && ist(opr2.first) && ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("div $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $t" + \
				to_string(lookup_treg(opr2.first)));
		}
		else if (ist(opr1.first) && !ist(opr2.first) && ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr2.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr2.first) +"($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("div $t" + to_string(lookup_treg(code.get_result())) + ", $t" + \
				to_string(lookup_treg(opr1.first)) + ", $s0");
		}
		else if (!ist(opr1.first) && !ist(opr2.first) && ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			int offset2 = lookup_sp_offset(opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset2) + "($sp)");
			this->mips_text.push_back("div $t" + to_string(lookup_treg(code.get_result())) + ", $s0, $s1");
		}
		else if (ist(opr1.first) && ist(opr2.first) && !ist(code.get_result()))
		{
			this->mips_text.push_back("div $s1, $t" + to_string(lookup_treg(opr1.first)) + ", $t" + \
				to_string(lookup_treg(opr2.first)));
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s1, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
		}
		else if (!ist(opr1.first) && ist(opr2.first) && !ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("div $s0, $s1, $t" + to_string(lookup_treg(opr2.first)));
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset2) + "($sp)");
		}
		else if (ist(opr1.first) && !ist(opr2.first) && !ist(code.get_result()))
		{
			int offset1 = lookup_sp_offset(opr2.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset1) + "($sp)");
			this->mips_text.push_back("div $s0, $t" + to_string(lookup_treg(opr1.first)) + ", $s1");
			int offset2 = lookup_sp_offset(code.get_result());
			if (offset2 == -1)
				this->mips_text.push_back(string("sw $s0, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s0, " + to_string(4 * offset2) + "($sp)");
		}
		else
		{
			int offset1 = lookup_sp_offset(opr1.first);
			if (offset1 == -1)
				this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + to_lowercase(opr1.first) + "($0)");
			else this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
			int offset2 = lookup_sp_offset(opr2.first);
			if (offset2 == -1)
				this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + to_lowercase(opr2.first) + "($0)");
			else this->mips_text.push_back("lw $s1, " + to_string(4 * offset2) + "($sp)");
			this->mips_text.push_back("div $s2, $s0, $s1");
			int offset = lookup_sp_offset(code.get_result());
			if (offset == -1)
				this->mips_text.push_back(string("sw $s2, ") + GLOB_PREFIX + to_lowercase(code.get_result()) + "($0)");
			else this->mips_text.push_back("sw $s2, " + to_string(4 * offset) + "($sp)");
		}
	}
#ifdef OPTIMIZE_1
	free_treg(opr1.first);
	free_treg(opr2.first);
#endif // OPTIMIZE_1
}

void SimpleAssembler::_arr(Pseudo code)
{
	string name = code.get_name();
	int offset = this->lookup_sp_offset(name);
	P ref = code.get_ref();
	int offset1 = this->lookup_sp_offset(ref.first);
	int offset2 = this->lookup_sp_offset(code.get_result());
	// ref
	if (ref.second == P_LINT)
		this->mips_text.push_back("li $s0, " + to_string(4 * stoi(ref.first)));
	else if (ref.second == P_LCHAR)
		this->mips_text.push_back("li $s0, " + to_string(4 * (int)ref.first[0]));
	else if (ist(ref.first))
		this->mips_text.push_back("sll $s0, $t" + to_string(lookup_treg(ref.first)) + ", 2");
	else if (offset1 == -1)
	{
		this->mips_text.push_back("lw $s0, " + string(GLOB_PREFIX) + ref.first + "($0)");
		this->mips_text.push_back("sll $s0, $s0, 2");
	}
	else
	{
		this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
		this->mips_text.push_back("sll $s0, $s0, 2");
	}
	// arr
	if (offset == -1)
	{
		this->mips_text.push_back("lw $s1, " + string(GLOB_PREFIX) + to_lowercase(name) + "($s0)");
	}
	else
	{
		this->mips_text.push_back("addu $s0, $s0, $sp");
		this->mips_text.push_back("lw $s1, " + to_string(4 * offset) + "($s0)");
	}
	// lvalue
	if (ist(code.get_result()))
		this->mips_text.push_back("move $t" + to_string(lookup_treg(code.get_result())) + ", $s1");
	else if (offset2 == -1)
		this->mips_text.push_back("sw $s1, " + string(GLOB_PREFIX) + to_lowercase(name) + "($s0)");
	else this->mips_text.push_back("sw $s1, " + to_string(4 * offset2) + "($sp)");
#ifdef OPTIMIZE_1
	free_treg(ref.first);
#endif // OPTIMIZE_1
}

void SimpleAssembler::_loop(Pseudo code)
{
	int num = this->loop_count++;
	this->mips_text.push_back(string(LOOP_PREFIX) + to_string(num) + ":");
	for (cursor++; codes[cursor].get_type() != P_LOOP; cursor++)
		_assemble_single(codes[cursor]);
	string rel = codes[cursor].get_rel();
	P opr1 = codes[cursor].get_opr1();
	P opr2 = codes[cursor].get_opr2();
	int offset1 = lookup_sp_offset(opr1.first);
	int offset2 = lookup_sp_offset(opr2.first);
	if (opr1.second == P_LINT) this->mips_text.push_back(string("li $s0, ") + opr1.first);
	else if (opr1.second == P_LCHAR) this->mips_text.push_back(string("li $s0, ") + (char)opr1.second[0]);
	else if (ist(opr1.first)) this->mips_text.push_back("move $s0, $t" + to_string(lookup_treg(opr1.first)));
	else if (offset1 == -1) this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + \
		to_lowercase(opr1.first) + "($0)");
	else this->mips_text.push_back(string("lw $s0, ") + to_string(4 * offset1) + "($sp)");
	if (opr2.second == P_LINT) this->mips_text.push_back(string("li $s1, ") + opr2.first);
	else if (opr2.second == P_LCHAR) this->mips_text.push_back(string("li $s1, ") + (char)opr2.second[0]);
	else if (ist(opr2.first)) this->mips_text.push_back("move $s1, $t" + to_string(lookup_treg(opr2.first)));
	else if (offset2 == -1) this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + \
		to_lowercase(opr2.first) + "($0)");
	else this->mips_text.push_back(string("lw $s1, ") + to_string(4 * offset2) + "($sp)");
	if (rel == T_EQL)
		this->mips_text.push_back(string("bne $s0, $s1, ") + ENDLOOP_PREFIX + to_string(num));
	else if (rel == T_NEQ)
		this->mips_text.push_back(string("beq $s0, $s1, ") + ENDLOOP_PREFIX + to_string(num));
	else if (rel == T_LSS)
	{
		this->mips_text.push_back("slt $s2, $s0, $s1");
		this->mips_text.push_back(string("beq $s2, $0, ") + ENDLOOP_PREFIX + to_string(num));
	}
	else if (rel == T_LEQ)
	{
		this->mips_text.push_back(string("beq $s0, $s1, ") + INTOLOOP_PREFIX + to_string(num));
		this->mips_text.push_back("slt $s2, $s0, $s1");
		this->mips_text.push_back(string("beq $s2, $0, ") + ENDLOOP_PREFIX + to_string(num));
		this->mips_text.push_back(string(INTOLOOP_PREFIX) + to_string(num) + ":");
	}
	else if (rel == T_GRE)
	{
		this->mips_text.push_back("sgt $s2, $s0, $s1");
		this->mips_text.push_back(string("beq $s2, $0, ") + ENDLOOP_PREFIX + to_string(num));
	}
	else if (rel == T_GEQ)
	{
		this->mips_text.push_back(string("beq $s0, $s1, ") + INTOLOOP_PREFIX + to_string(num));
		this->mips_text.push_back("slt $s2, $s0, $s1");
		this->mips_text.push_back(string("bne $s2, $0, ") + ENDLOOP_PREFIX + to_string(num));
		this->mips_text.push_back(string(INTOLOOP_PREFIX) + to_string(num) + ":");
	}
#ifdef OPTIMIZE_1
	free_treg(opr1.first);
	free_treg(opr2.first);
#endif // OPTIMIZE_1
	int paired = this->lookup_pair(codes[cursor], this->cursor);
	for (cursor++; cursor < paired; cursor++)
		this->_assemble_single(codes[cursor]);
	this->mips_text.push_back(string("j ") + LOOP_PREFIX + to_string(num));
	this->mips_text.push_back(string(ENDLOOP_PREFIX) + to_string(num) + ":");
}

void SimpleAssembler::_if(Pseudo code)
{
	int num = this->if_count++;
	int ofst = this->cursor;
	this->mips_text.push_back(string(IF_PREFIX) + to_string(num) + ":");
	string rel = codes[cursor].get_rel();
	P opr1 = codes[cursor].get_opr1();
	P opr2 = codes[cursor].get_opr2();
	opr1 = opr1.second == P_LCHAR ? lchar2lint(opr1) : opr1;
	opr2 = opr2.second == P_LCHAR ? lchar2lint(opr2) : opr2;
	int offset1 = lookup_sp_offset(opr1.first);
	int offset2 = lookup_sp_offset(opr2.first);
	if (opr1.second == P_LINT) this->mips_text.push_back(string("li $s0, ") + opr1.first);
	else if (opr1.second == P_LCHAR) this->mips_text.push_back(string("li $s0, ") + (char)opr1.second[0]);
	else if (ist(opr1.first)) this->mips_text.push_back("move $s0, $t" + to_string(lookup_treg(opr1.first)));
	else if (offset1 == -1) this->mips_text.push_back(string("lw $s0, ") + GLOB_PREFIX + \
		to_lowercase(opr1.first) + "($0)");
	else this->mips_text.push_back(string("lw $s0, ") + to_string(4 * offset1) + "($sp)");
	if (opr2.second == P_LINT) this->mips_text.push_back(string("li $s1, ") + opr2.first);
	else if (opr2.second == P_LCHAR) this->mips_text.push_back(string("li $s1, ") + (char)opr2.second[0]);
	else if (ist(opr2.first)) this->mips_text.push_back("move $s1, $t" + to_string(lookup_treg(opr2.first)));
	else if (offset2 == -1) this->mips_text.push_back(string("lw $s1, ") + GLOB_PREFIX + \
		to_lowercase(opr2.first) + "($0)");
	else this->mips_text.push_back(string("lw $s1, ") + to_string(4 * offset2) + "($sp)");
	int paired_else = this->lookup_paired_else(this->cursor);
	string prefix = paired_else == -1 ? ENDIF_PREFIX : ELSE_PREFIX;
	if (rel == T_EQL)
		this->mips_text.push_back(string("bne $s0, $s1, ") + prefix + to_string(num));
	else if (rel == T_NEQ)
		this->mips_text.push_back(string("beq $s0, $s1, ") + prefix + to_string(num));
	else if (rel == T_LSS)
	{
		this->mips_text.push_back("slt $s2, $s0, $s1");
		this->mips_text.push_back(string("beq $s2, $0, ") + prefix + to_string(num));
	}
	else if (rel == T_LEQ)
	{
		this->mips_text.push_back(string("beq $s0, $s1, ") + INTOIF_PREFIX + to_string(num));
		this->mips_text.push_back("slt $s2, $s0, $s1");
		this->mips_text.push_back(string("beq $s2, $0, ") + prefix + to_string(num));
		this->mips_text.push_back(string(INTOIF_PREFIX) + to_string(num) + ":");
	}
	else if (rel == T_GRE)
	{
		this->mips_text.push_back("sgt $s2, $s0, $s1");
		this->mips_text.push_back(string("beq $s2, $0, ") + prefix + to_string(num));
	}
	else if (rel == T_GEQ)
	{
		this->mips_text.push_back(string("beq $s0, $s1, ") + INTOIF_PREFIX + to_string(num));
		this->mips_text.push_back("slt $s2, $s0, $s1");
		this->mips_text.push_back(string("bne $s2, $0, ") + prefix + to_string(num));
		this->mips_text.push_back(string(INTOIF_PREFIX) + to_string(num) + ":");
	}
	if (paired_else != -1)
	{
		for (cursor++; cursor < paired_else; cursor++)
			this->_assemble_single(codes[cursor]);
		this->mips_text.push_back(string("j ") + ENDIF_PREFIX + to_string(num));
		this->mips_text.push_back(string(ELSE_PREFIX) + to_string(num) + ":");
		this->cursor--;
	}
#ifdef OPTIMIZE_1
	//free_treg(opr1.first);
	//free_treg(opr2.first);
#endif // OPTIMIZE_1
	int paired = this->lookup_pair(code, ofst);
	for (cursor++; cursor < paired; cursor++)
		this->_assemble_single(codes[cursor]);
	this->cursor--;
	this->mips_text.push_back(string(ENDIF_PREFIX) + to_string(num) + ":");
}

void SimpleAssembler::_ret(Pseudo code)
{
	if (to_lowercase(current_scope) == to_lowercase(G_GLOBALSCOPE))
		this->mips_text.push_back(string() + "j " + PROGRAM_END);
	else
	{
		P ret = code.get_ret();
		int offset = lookup_sp_offset(ret.first);
		if (ret.second == P_LINT) this->mips_text.push_back("li $v0, " + ret.first);
		else if (ret.second == P_LCHAR) this->mips_text.push_back(string("li $v0, ") + to_string((int)ret.first[0]));
		else if (ist(ret.first)) this->mips_text.push_back("move $v0, $t" + to_string(lookup_treg(ret.first)));
		else if (offset == -1) this->mips_text.push_back("lw $v0, " + string(GLOB_PREFIX) + to_lowercase(ret.first) + \
			"($0)");
		else this->mips_text.push_back("lw $v0, " + to_string(4 * offset) + "($sp)");
		this->mips_text.push_back(string() + "jr $ra");
#ifdef OPTIMIZE_1
		free_treg(ret.first);
#endif // OPTIMIZE_1
	}
}

void SimpleAssembler::_retn(Pseudo code)
{
	if (this->current_scope == G_MAINSCOPE) this->mips_text.push_back("j " + string(PROGRAM_END));
	else this->mips_text.push_back("jr $ra");
}

void SimpleAssembler::maintain_context()
{
	this->mips_text.push_back("addiu $sp, $sp, -48");
	for (int i = 0; i < 10; i++)
		this->mips_text.push_back("sw $t" + to_string(i) + ", " + to_string(4 * i) + "($sp)");
	this->mips_text.push_back("sw $ra, 40($sp)");
	this->mips_text.push_back("sw $v0, 44($sp)");
}

void SimpleAssembler::recover_context()
{
	this->mips_text.push_back("lw $v0, 44($sp)");
	this->mips_text.push_back("lw $ra, 40($sp)");
	for (int i = 0; i < 10; i++)
		this->mips_text.push_back("lw $t" + to_string(i) + ", " + to_string(4 * i) + "($sp)");
	this->mips_text.push_back("addiu $sp, $sp, 48");
}

#ifdef OPTIMIZE_5
vector<bool> SimpleAssembler::maintain_context_new()
{
	vector<bool> v;
	this->mips_text.push_back("addiu $sp, $sp, -48");
	for (int i = 0; i < 10; i++)
	{
		v.push_back(false);
		if (i < treg.size() && treg[i] != "")
		{
			this->mips_text.push_back("sw $t" + to_string(i) + ", " + to_string(4 * i) + "($sp)");
			v[i] = true;
		}
	}
	this->mips_text.push_back("sw $ra, 40($sp)");
	this->mips_text.push_back("sw $v0, 44($sp)");
	return v;
}

void SimpleAssembler::recover_context_new(vector<bool> v)
{
	this->mips_text.push_back("lw $v0, 44($sp)");
	this->mips_text.push_back("lw $ra, 40($sp)");
	for (int i = 0; i < 10; i++)
		if (v[i]) 
			this->mips_text.push_back("lw $t" + to_string(i) + ", " + to_string(4 * i) + "($sp)");
	this->mips_text.push_back("addiu $sp, $sp, 48");
}
#endif // OPTIMIZE_5

void SimpleAssembler::_call(Pseudo code)
{
#ifndef OPTIMIZE_5
	this->maintain_context();
#else
	vector<bool> v = this->maintain_context_new();
#endif // !OPTIMIZE_5
	int frame_size = this->frame_size.find(to_lowercase(code.get_name()))->second;
	this->mips_text.push_back("addiu $sp, $sp, -" + to_string(4 * frame_size));
	for (int i = 0; i < code.get_args().size(); i++)
	{
		P arg = code.get_args()[i];
		int offset = this->lookup_sp_offset(arg.first);
		if (arg.second == P_LINT)
		{
			this->mips_text.push_back("li $s0, " + arg.first);
			this->mips_text.push_back("sw $s0, " + to_string(4 * i) + "($sp)");
		}
		else if (arg.second == P_LCHAR)
		{
			this->mips_text.push_back("li $s0, " + to_string((int)arg.first[0]));
			this->mips_text.push_back("sw $s0, " + to_string(4 * i) + "($sp)");
		}
		else if (ist(arg.first))
			this->mips_text.push_back("sw $t" + to_string(lookup_treg(arg.first)) + ", " + \
				to_string(4 * i) + "($sp)");
		else if (offset == -1)
		{
			this->mips_text.push_back("lw $s0, " + string(GLOB_PREFIX) + to_lowercase(arg.first) + "($0)");
			this->mips_text.push_back("sw $s0, " + to_string(4 * i) + "($sp)");
		}
		else
		{
			this->mips_text.push_back("lw $s0, " + to_string(4 * (offset + frame_size + 12)) + "($sp)");
			this->mips_text.push_back("sw $s0, " + to_string(4 * i) + "($sp)");
		}
#ifdef OPTIMIZE_1
		free_treg(arg.first);
#endif // OPTIMIZE_1
	}
	this->mips_text.push_back("jal " + string(FUNC_PREFIX) + to_lowercase(code.get_name()));
	this->mips_text.push_back("addiu $sp, $sp, " + to_string(4 * frame_size));
	string result = code.get_result();
	int offset = lookup_sp_offset(result);
	if (ist(result)) 
	{
		this->mips_text.push_back("sw $v0, " + to_string(4 * lookup_treg(result)) + "($sp)");
#ifdef OPTIMIZE_5
		v[lookup_treg(result)] = true;
#endif // OPTIMIZE_5
	}
	else if (offset == -1) this->mips_text.push_back("sw $v0, " + string(GLOB_PREFIX) + to_lowercase(result) + "($0)");
	else this->mips_text.push_back("sw $v0, " + to_string(4 * (offset + 12)) + "($sp)");
#ifndef OPTIMIZE_5
	this->recover_context();
#else
	this->recover_context_new(v);
#endif // !OPTIMIZE_5
}

void SimpleAssembler::_caln(Pseudo code)
{
	this->maintain_context();
	int frame_size = this->frame_size.find(to_lowercase(code.get_name()))->second;
	this->mips_text.push_back("addiu $sp, $sp, -" + to_string(4 * frame_size));
	for (int i = 0; i < code.get_args().size(); i++)
	{
		P arg = code.get_args()[i];
		int offset = this->lookup_sp_offset(arg.first);
		if (arg.second == P_LINT)
		{
			this->mips_text.push_back("li $s0, " + arg.first);
			this->mips_text.push_back("sw $s0, " + to_string(4 * i) + "($sp)");
		}
		else if (arg.second == P_LCHAR)
		{
			this->mips_text.push_back("li $s0, " + to_string((int)arg.first[0]));
			this->mips_text.push_back("sw $s0, " + to_string(4 * i) + "($sp)");
		}
		else if (ist(arg.first))
			this->mips_text.push_back("sw $t" + to_string(lookup_treg(arg.first)) + ", " + \
				to_string(4 * i) + "($sp)");
		else if (offset == -1)
		{
			this->mips_text.push_back("lw $s0, " + string(GLOB_PREFIX) + to_lowercase(arg.first) + "($0)");
			this->mips_text.push_back("sw $s0, " + to_string(4 * i) + "($sp)");
		}
		else
		{
			this->mips_text.push_back("lw $s0, " + to_string(4 * (offset + frame_size + 12)) + "($sp)");
			this->mips_text.push_back("sw $s0, " + to_string(4 * i) + "($sp)");
		}
#ifdef OPTIMIZE_1
		free_treg(arg.first);
#endif // OPTIMIZE_1
	}
	this->mips_text.push_back("jal " + string(FUNC_PREFIX) + to_lowercase(code.get_name()));
	this->mips_text.push_back("addiu $sp, $sp, " + to_string(4 * frame_size));
	this->recover_context();
}

void SimpleAssembler::_assn(Pseudo code)
{
	P rvalue = code.get_opr1().second == P_LCHAR ? lchar2lint(code.get_opr1()) : code.get_opr1();
	string lvalue = code.get_result();
	int roffset = this->lookup_sp_offset(rvalue.first);
	int loffset = this->lookup_sp_offset(lvalue);
	if (rvalue.second == P_LINT)
		this->mips_text.push_back("li $s0, " + rvalue.first);
	else if (ist(rvalue.first))
		this->mips_text.push_back("move $s0, $t" + to_string(lookup_treg(rvalue.first)));
	else if (roffset == -1)
		this->mips_text.push_back("lw $s0, " + string(GLOB_PREFIX) + to_lowercase(rvalue.first) + "($0)");
	else this->mips_text.push_back("lw $s0, " + to_string(4 * roffset) + "($sp)");
	if (ist(lvalue))
		this->mips_text.push_back("move $t" + to_string(lookup_treg(lvalue)) + ", $s0");
	else if (loffset == -1)
		this->mips_text.push_back("sw $s0, " + string(GLOB_PREFIX) + to_lowercase(lvalue) + "($0)");
	else this->mips_text.push_back("sw $s0, " + to_string(4 * loffset) + "($sp)");
#ifdef OPTIMIZE_1
	free_treg(rvalue.first);
#endif // OPTIMIZE_1
}

void SimpleAssembler::_assr(Pseudo code)
{
	string name = code.get_name();
	P ref = code.get_offset();
	P value = code.get_opr1();
	value = value.second == P_LCHAR ? lchar2lint(value) : value;
	int roffset = this->lookup_sp_offset(value.first);
	int offset = this->lookup_sp_offset(name);
	int offset1 = this->lookup_sp_offset(ref.first);
	// rvalue
	if (value.second == P_LINT)
		this->mips_text.push_back("li $s1, " + value.first);
	else if (ist(value.first))
		this->mips_text.push_back("move $s1, $t" + to_string(lookup_treg(value.first)));
	else if (roffset == -1)
		this->mips_text.push_back("lw $s1, " + string(GLOB_PREFIX) + to_lowercase(value.first) + "($0)");
	else this->mips_text.push_back("lw $s1, " + to_string(4 * roffset) + "($sp)");
	// ref
	if (ref.second == P_LINT)
		this->mips_text.push_back("li $s0, " + to_string(4 * stoi(ref.first)));
	else if (ref.second == P_LCHAR)
		this->mips_text.push_back("li $s0, " + to_string(4 * (int)ref.first[0]));
	else if (ist(ref.first))
		this->mips_text.push_back("sll $s0, $t" + to_string(lookup_treg(ref.first)) + ", 2");
	else if (offset1 == -1)
	{
		this->mips_text.push_back("lw $s0, " + string(GLOB_PREFIX) + ref.first + "($0)");
		this->mips_text.push_back("sll $s0, $s0, 2");
	}
	else
	{
		this->mips_text.push_back("lw $s0, " + to_string(4 * offset1) + "($sp)");
		this->mips_text.push_back("sll $s0, $s0, 2");
	}
	if (offset == -1)
	{
		this->mips_text.push_back("sw $s1, " + string(GLOB_PREFIX) + to_lowercase(name) + "($s0)");
	}
	else
	{
		this->mips_text.push_back("addu $s0, $s0, $sp");
		this->mips_text.push_back("sw $s1, " + to_string(4 * offset) + "($s0)");
	}
#ifdef OPTIMIZE_1
	free_treg(ref.first);
	free_treg(value.first);
#endif // OPTIMIZE_1
}

void SimpleAssembler::_read(Pseudo code)
{
	P read = code.get_io();
	if (read.second == READ_INT) this->mips_text.push_back("li $v0, 5");
	else if (read.second == READ_CHAR) this->mips_text.push_back("li $v0, 12");
	this->mips_text.push_back("syscall");
	string name = read.first;
	int offset = this->lookup_sp_offset(name);
	if (offset == -1) this->mips_text.push_back(string("sw $v0, ") + GLOB_PREFIX + to_lowercase(name) + "($0)");
	else this->mips_text.push_back("sw $v0, " + to_string(4 * offset) + "($sp)");
}

void SimpleAssembler::_disp(Pseudo code)
{
	P disp = code.get_io();
	string endl = code.get_endl();
	string buffer = "";
	if (disp.second == DISP_LINT || disp.second == DISP_LCHAR || disp.second == DISP_STR)
	{
		if (disp.second == DISP_LINT) buffer += disp.first;
		else if (disp.second == DISP_LCHAR) buffer += disp.first == "\\" ? "\\\\" : disp.first;
		else for (int i = 0; i < disp.first.size(); i++)
			buffer += (disp.first[i] == '\\') ? "\\\\" : string() + disp.first[i];
		if (endl == DISP_ENDL) buffer += "\\n";
		this->mips_data.push_back(STR_PREFIX + to_string(str_count) + ": .asciiz \"" + \
			buffer + "\"");
		this->mips_text.push_back(string() + "la $a0, " + STR_PREFIX + to_string(str_count));
		this->mips_text.push_back("li $v0, 4");
		this->mips_text.push_back("syscall");
		str_count++;
	}
	else if (disp.second == DISP_VINT || disp.second == DISP_VCHAR)
	{
		int offset = this->lookup_sp_offset(disp.first);
		if (ist(disp.first)) 
			this->mips_text.push_back(string("move $a0, $t") + to_string(lookup_treg(disp.first)));
		else if (offset == -1)
			this->mips_text.push_back(string("lw $a0, ") + GLOB_PREFIX + to_lowercase(disp.first) + "($0)");
		else this->mips_text.push_back(string("lw $a0, ") + to_string(4 * offset) + "($sp)");
		if (disp.second == DISP_VINT) this->mips_text.push_back("li $v0, 1");
		else this->mips_text.push_back("li $v0, 11");
		this->mips_text.push_back("syscall");
		if (endl == DISP_ENDL)
		{
			this->mips_data.push_back(STR_PREFIX + to_string(str_count) + ": .asciiz \"\\n\"");
			this->mips_text.push_back("li $v0, 4");
			this->mips_text.push_back("la $a0, " + string(STR_PREFIX) + to_string(str_count));
			this->mips_text.push_back("syscall");
			str_count++;
		}
	}
#ifdef OPTIMIZE_1
	free_treg(disp.first);
#endif // OPTIMIZE_1
}

void SimpleAssembler::clear_tregs()
{
	for (int i = 0; i < treg.size(); i++) treg[i] = "";
}

void SimpleAssembler::_assemble_functions()
{
	this->cursor = 0;
	while (cursor < this->func_addr.find(to_lowercase(G_MAINSCOPE))->second)
	{
		if (codes[cursor].get_type() == P_FUNC)
		{
			this->mips_text.push_back(string(FUNC_PREFIX) + to_lowercase(codes[cursor].get_header().first) + ":");
			this->current_scope = codes[cursor].get_header().first;
			this->clear_tregs();
			this->initialize_frame(current_scope, false);
		}
		else if (codes[cursor].get_type() == P_ENDFUNC)
		{
			this->mips_text.push_back("jr $ra");
			this->mips_text.push_back(string(ENDFUNC_PREFIX) + to_string(func_count++) + ":");
		}
		else this->_assemble_single(codes[cursor]);
		cursor++;
	}
}

void SimpleAssembler::initialize_frame(string scope, bool _malloc)
{
	map<string, vector<Var>*>::iterator it = symtable.find(to_lowercase(scope));
	if (it == symtable.end()) return;
	map<string, int> syms = *(var_offset.find(to_lowercase(scope))->second);
	int size = frame_size.find(to_lowercase(scope))->second;
	if (_malloc) this->mips_text.push_back("addiu $sp, $sp, -" + to_string(4 * size));
	vector<Var> vars = *(it->second);
	for (int i = 0; i < vars.size(); i++)
	{
		if (vars[i].has_value() && !vars[i].is_const())
		{
			if (vars[i].get_dim().size() == 0)
			{
				this->mips_text.push_back("li $t0, " + to_string(vars[i].get_value()));
				this->mips_text.push_back("sw $t0, " + \
					to_string(4 * syms.find(to_lowercase(vars[i].get_name()))->second) + \
					"($sp)");
			}
			else if (vars[i].get_dim().size() == 1)
			{
				for (int j = 0; j < vars[i].get_dim()[0]; j++)
				{
					this->mips_text.push_back("li $t0, " + to_string(vars[i].get_value(j)));
					this->mips_text.push_back("sw $t0, " + \
						to_string(4 * (syms.find(to_lowercase(vars[i].get_name()))->second + j)) + \
						"($sp)");
				}
			}
			else if (vars[i].get_dim().size() == 2)
			{
				for (int j = 0; j < vars[i].get_dim()[0] * vars[i].get_dim()[1]; j++)
				{
					this->mips_text.push_back("li $t0, " + to_string(vars[i].get_value(j)));
					this->mips_text.push_back("sw $t0, " + \
						to_string(4 * (syms.find(to_lowercase(vars[i].get_name()))->second + j)) + \
						"($sp)");
				}
			}
		}
	}
}
