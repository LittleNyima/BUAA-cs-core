#ifndef GTREE_H
#define GTREE_H

#include <string>
#include <vector>
#include <utility>
#include "tokens.h"
#include "pseudo.h"
#include <map>

using namespace std;

typedef string GToken;
typedef pair<string, string> P;

class GNode
{
private:
	GToken type;
	string content;
	GNode* father;
	vector<GNode*> sons;

public:
	GNode(GToken);
	GNode(GToken, string);
	GNode(GToken, GNode*);
	GNode(GToken, string, GNode*);

	GToken get_type();
	string get_content();
	GNode* get_father();

	void set_type(GToken);
	void set_sons(vector<GNode*>);
	void set_father(GNode*);

	void add_son(GNode*);
	vector<GNode*> get_sons();
	GNode* get_son(int);
};

class Var
{
private:
	string type;
	string name;
	vector<int> dim;
	vector<int> _ivalue;
	vector<char> _cvalue;
	bool _has_value;
	bool _is_const;

public:
	Var(Var*); // Copy construction
	Var(string, string, int, bool); // Single integer with initialization
	Var(string, string, char, bool); // Single char with initialization
	Var(string, string); // Single integer/char without initializations
	Var(string, string, int); // 1-dimensional array without initialization
	Var(string, string, int, int); // 2-dimensional array without initialization
	Var(string, string, int, vector<int>); // 1-dimensional int array with initialzation
	Var(string, string, int, int, vector<int>); // 2-dimensional int array with initialization
	Var(string, string, int, vector<char>); // 1-dimensional char array with initialization
	Var(string, string, int, int, vector<char>); // 2-dimensional char array with initilization
	string get_type();
	string get_name();
	bool has_value();
	int get_value();
	int get_value(int);
	int get_value(int, int);
	bool is_const();
	vector<int> get_dim();
	string to_string();
};

class Func
{
private:
	string ret;
	string name;
	vector<string> arg_type;
	vector<P> args;

public:
	Func(string, string, vector<string>, vector<P>);
	string get_ret();
	string get_name();
	vector<string> get_arg_type();
	vector<P> get_args();
};

class GTree
{
private:
	GNode* root;
	bool print_log;
	bool print2file;
	string current_scope;
	int _new_name_count;

	vector<Pseudo> pseudos;
	map<string, vector<Var>*> var_table;
	vector<Func> func_table;

	void _print_tree_dfs(GNode*, int);
	void after_analysis();
	string gen_new_name();
	Var* lookup_var(string);
	Var* lookup_arg(string);
	Func* lookup_func(string);

	void rearrange_g1arr2(GNode*);
	GNode* index2_convert(GNode*, GNode*, GNode*, int);
	GNode* expr2item(GNode*, GNode*);
	GNode* unsigned2factor(unsigned int, GNode*);
	pair<string, string> const2int(Var);
	pair<string, string> lint2lchar(pair<string, string>);
	pair<string, string> lchar2lint(pair<string, string>);

	void programme(GNode*);
	void const_declaration(GNode*);
	void const_definition(GNode*);
	void const_int_definition(GNode*);
	void const_char_definition(GNode*);
	void variable_declaration(GNode*);
	void variable_definition(GNode*, vector<Var>*);
	void var_mulvarnoinit(GNode*, vector<Var>*);
	void var_g1var(GNode*, string, vector<Var>*);
	void var_g1arr1(GNode*, string, vector<Var>*);
	void var_g1arr2(GNode*, string, vector<Var>*);
	void var_singlevarnoinit(GNode*, vector<Var>*);
	void var_singlevarinit(GNode*, vector<Var>*);
	void var_1dimarrinit(GNode*, vector<Var>*);
	void var_1dimarrnoinit(GNode*, vector<Var>*);
	void var_2dimarrinit(GNode*, vector<Var>*);
	void var_2dimarrnoinit(GNode*, vector<Var>*);
	void non_void_function_definition(GNode*);
	void void_function_definition(GNode*);
	void arg_table(GNode*, vector<string>*, vector<pair<string, string>>*);
	pair<string, string> func_head(GNode*);
	void main_function(GNode*);
	void composite_states(GNode*);
	void state_crowd(GNode*);
	void state_loop(GNode*);
	void state_while(GNode*);
	void state_for(GNode*);
	void for_stride(GNode*);
	void loop_condition(GNode*);
	void state_condition(GNode*);
	void condition(GNode*);
	pair<string, string> state_nonvoid_func_call(GNode*);
	void state_void_func_call(GNode*);
	void value_args(GNode*, vector<pair<string, string>>*);
	void state_assign(GNode*);
	void state_assign_g1var(GNode*);
	void state_assign_g1arr1(GNode*);
	void state_assign_g1arr2(GNode*);
	void state_scanf(GNode*);
	void state_printf(GNode*);
	void state_print_str(GNode*);
	void state_print_expr(GNode*);
	void state_print_str_expr(GNode*);
	void state_switch(GNode*);
	int case_table(GNode*, pair<string, string>);
	void default_case(GNode*);
	void state_return(GNode*);
	pair<string, string> expression(GNode*);
	pair<string, string> item(GNode*);
	pair<string, string> factor(GNode*);
	pair<string, string> factor_g1var(GNode*);
	pair<string, string> factor_g1arr1(GNode*);
	pair<string, string> factor_g1arr2(GNode*);
	pair<string, string> factor_int(GNode*);
	pair<string, string> factor_char(GNode*);
	pair<string, string> gconst(GNode*);
	int integer(GNode*);

public:
	GTree(GNode*);
	void print_tree();
	void analyze();
	void debug_mode();
	void submit_mode();

	vector<Pseudo> get_pcodes();
	map<string, vector<Var>*> get_symtable();
};

#define G_PLUS         "GPLUS"
#define G_MULT         "GMULT"
#define G_REL          "GREL"
#define G_LETTER       "GLETTER"
#define G_DIGIT        "GDIGIT"
#define G_CHAR         "GCHARACTER"
#define G_STR          "GSTRING"
#define G_PROGRAM      "GPROGRAM"
#define G_CONSTDEF     "GCONSTDEF"
#define G_CONSTDEC     "GCONSTDEC"
#define G_UNSIGNED     "GUNSIGNED"
#define G_INT          "GINTEGER"
#define G_IDENTIFR     "IDENFR"
#define G_DECHEADER    "GFUNCHEADER"
#define G_CONST        "GCONST"
#define G_VARDEC       "GVARDEC"
#define G_VARDEF       "GVARDEF"
#define G_VARDEFNOINIT "GVARNOINIT"
#define G_VARDEFINIT   "GVARINIT"
#define G_TYPE         "GTYPE"
#define G_FUNCNONVOID  "GNONVOIDFUNC"
#define G_FUNCVOID     "GVOIDFUNC"
#define G_COMPOSITE    "GCOMPOSITE"
#define G_ARGTABLE     "GARGTABLE"
#define G_MAINFUNC     "GMAINFUNC"
#define G_EXPRESSION   "GEXPRESSION"
#define G_ITEM         "GITEM"
#define G_FACTOR       "GFACTOR"
#define G_STATE        "GSTATE"
#define G_ASSIGNMENT   "GASSIGNMENT"
#define G_CONDISTATE   "GCONDISTATE"
#define G_CONDITION    "GCONDITION"
#define G_LOOPSTATE    "GLOOPSTATE"
#define G_STRIDE       "GSTRIDE"
#define G_SWITCHSTATE  "GSWITCHSTATE"
#define G_CASETABLE    "GCASETABLE"
#define G_CASESUBSTATE "GCASESUBSTAT"
#define G_DEFAULT      "GDEFAULT"
#define G_CALLNONVOID  "GCALLNONVOID"
#define G_CALLVOID     "GCALLVOID"
#define G_VALUEARGS    "GVALUEARGS"
#define G_STATECROWD   "GSTATECROWD"
#define G_SCANFSTATE   "GSCANF"
#define G_PRINTFSTATE  "GPRINTF"
#define G_RETURNSTATE  "GRETURN"
#define G_RESERVERD    "##GRESERVED##"

// G0_XXX is used for marking type of var definition states
#define G0_MULVARNOINIT  "G0MULVARNOINIT"
#define G0_2DIMARRINIT   "G02DIMARRINIT"
#define G0_2DIMARRNOINIT "G02DIMARRNOINIT"
#define G0_1DIMARRINIT   "G01DIMARRINIT"
#define G0_1DIMARRNOINIT "G01DIMARRNOINIT"
#define G0_SINGVARNOINIT "G0SINGVARNOINIT"
#define G0_SINGVARINIT   "G0SINGVARINIT"

// G1_XXX is used for marking type of variable in a `MUL` type
// variable definition statement, or marking the type of reference
// in `factor` analysis
#define G1_VAR    "G1VAR"
#define G1_ARR1   "G1ARR1"
#define G1_ARR2   "G1ARR2"
#define G1_ARR2RE "G1ARR2-RE"

#define G_GLOBALSCOPE "@GLOBAL"
#define G_MAINSCOPE   "MAIN"
#define G_TEMPPREFIX  "$TEMP"

#endif // !GTREE_H

