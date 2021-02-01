#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include <vector>
#include "pseudo.h"
#include "gtree.h"
#include "optlevel.h"

class SimpleAssembler
{
private:
	vector<Pseudo> codes;
	map<string, vector<Var>*> symtable;
	vector<string> mips_text;
	vector<string> mips_data;
	map<string, int> func_addr;
	map<string, int> frame_size;
	map<string, map<string, int>*> var_offset;
	vector<string> treg;
	map<string, int> tmap;
	bool print_log;
	bool print2file;
	int cursor;
	int treg_cursor;
	string current_scope;

	int loop_count;
	int if_count;
	int str_count;
	int func_count;

	int tmp_count;

#ifdef OPTIMIZE_1
	void free_treg(string);
#endif // OPTIMIZE_1
#ifdef OPTIMIZE_2
	void mult_2_power(Pseudo, P, P);
#endif // OPTIMIZE_2
#ifdef OPTIMIZE_4
#define DIV_PREFIX string("DIV_")
	int div_count = 0;
	void div_2_power(Pseudo, P, P);
#endif // OPTIMIZE_4
#ifdef OPTIMIZE_5
	vector<bool> maintain_context_new();
	void recover_context_new(vector<bool>);
#endif // OPTIMIZE_5


	void clear_all();
	void locate_func();
	void locate_globals();
	void _locate_var(Var);
	void _locate_arr1(Var);
	void _locate_arr2(Var);
	int lookup_func(string);
	int lookup_pair(Pseudo, int);
	int _lookup_pair(int, string, string);
	int lookup_paired_else(int);
	void configure_stackframe();
	void print_stack_desc();
	int lookup_treg(string);
	int lookup_sp_offset(string);
	int dist_treg(string);
	void clear_tregs();
	string load_name(string, string);

	void _assemble();
	void _assemble_single(Pseudo);
	void _assemble_functions();
	void after_assembly();
	void initialize_frame(string, bool);
	void maintain_context();
	void recover_context();

	P lchar2lint(P);

	void _add(Pseudo);
	void _sub(Pseudo);
	void _mul(Pseudo);
	void _div(Pseudo);
	void _arr(Pseudo);
	void _loop(Pseudo);
	void _if(Pseudo);
	void _ret(Pseudo);
	void _retn(Pseudo);
	void _call(Pseudo);
	void _caln(Pseudo);
	void _assn(Pseudo);
	void _assr(Pseudo);
	void _read(Pseudo);
	void _disp(Pseudo);

public:
	SimpleAssembler();
	void assemble(vector<Pseudo>, map<string, vector<Var>*>);
	void debug_mode();
	void submit_mode();
};

#endif // !ASSEMBLY_H
