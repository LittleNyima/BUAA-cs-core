#include "gtree.h"
#include "util.h"
#include <iostream>

#define PRINTLN(m) do{cout<<"[GTREE] "<<m<<endl;}while(0)
#define PRINT(m) do{cout<<"[GTREE] "<<m;}while(0)
#define WARN(m) do{cout<<"[!GTRE] "<<m<<endl;}while(0)

GTree::GTree(GNode* root)
{
	this->root = root;
	this->print_log = false;
	this->print2file = false;
	this->_new_name_count = 0;
}

void GTree::debug_mode()
{
	this->print_log = true;
	this->print2file = true;
}

void GTree::submit_mode()
{
	this->print_log = false;
	this->print2file = true;
}

void GTree::_print_tree_dfs(GNode* node, int depth)
{
	for (int i = 0; i < depth - 1; i++)
	{
		cout << "|   ";
	}
	if (depth != 0)
	{
		cout << "|---";
	}
	cout << node->get_type();
	if (node->get_content() != "")
	{
		cout << ": " << node->get_content();
	}
	cout << endl;
	for (int i = 0; i < node->get_sons().size(); i++)
	{
		_print_tree_dfs(node->get_son(i), depth + 1);
	}
}

void GTree::print_tree()
{
	if (this->root != nullptr)
	{
		this->_print_tree_dfs(this->root, 0);
	}
}

vector<Pseudo> GTree::get_pcodes()
{
	return this->pseudos;
}

map<string, vector<Var>*> GTree::get_symtable()
{
	return this->var_table;
}

string GTree::gen_new_name()
{
	return G_TEMPPREFIX + to_string(_new_name_count++);
}

Var* GTree::lookup_arg(string name)
{
	Func* func = lookup_func(this->current_scope);
	if (func == nullptr) return nullptr;
	vector<P> args = func->get_args();
	for (int i = 0; i < args.size(); i++)
		if (to_lowercase(name) == to_lowercase(args[i].first))
			return new Var(to_lowercase(args[i].second) == "int" ? T_INTTK : T_CHARTK, name);
	return nullptr;
}

Var* GTree::lookup_var(string name)
{
	map<string, vector<Var>*>::iterator it = this->var_table.find(to_lowercase(current_scope));
	if (it != var_table.end())
	{
		vector<Var> vars = *(it->second);
		for (int i = 0; i < vars.size(); i++)
			if (to_lowercase(vars[i].get_name()) == to_lowercase(name))
				return new Var(vars[i]);
	}
	Var* arg = lookup_arg(name);
	if (arg != nullptr) return arg;
	it = this->var_table.find(to_lowercase(G_GLOBALSCOPE));
	if (it != var_table.end())
	{
		vector<Var> vars = *(it->second);
		for (int i = 0; i < vars.size(); i++)
			if (to_lowercase(vars[i].get_name()) == to_lowercase(name))
				return new Var(vars[i]);
	}
	return nullptr;
}

Func* GTree::lookup_func(string name)
{
	for (int i = 0; i < func_table.size(); i++)
		if (to_lowercase(name) == to_lowercase(func_table[i].get_name()))
			return &func_table[i];
	return nullptr;
}

void GTree::rearrange_g1arr2(GNode* curptr)
{
	// notice: we assume that the indexing expressions are all integers
	vector<GNode*> sons = curptr->get_sons();
	vector<GNode*> new_sons;
	Var* v = this->lookup_var(sons[0]->get_content());
	new_sons.push_back(sons[0]);
	new_sons.push_back(index2_convert(sons[1], sons[2], curptr, v->get_dim()[1]));
	curptr->set_type(G1_ARR2RE);
	curptr->set_sons(new_sons);
}

GNode* GTree::index2_convert(GNode* expr1, GNode* expr2, GNode* father, int dim2)
{
	GNode* new_expr = new GNode(G_EXPRESSION, father);
	new_expr->add_son(new GNode(T_INTTK, new_expr));
	new_expr->add_son(new GNode(T_PLUS, new_expr));
	new_expr->add_son(expr2item(expr1, new_expr));
	new_expr->add_son(new GNode(T_PLUS, new_expr));
	new_expr->add_son(expr2item(expr2, new_expr));
	new_expr->get_son(2)->add_son(new GNode(T_MULT, new_expr->get_son(2)));
	new_expr->get_son(2)->add_son(unsigned2factor(dim2, new_expr->get_son(2)));
	return new_expr;
}

GNode* GTree::expr2item(GNode* node, GNode* father)
{
	GNode* item = new GNode(G_ITEM, father);
	GNode* factor = new GNode(G_FACTOR, item);
	item->add_son(factor);
	factor->add_son(node);
	node->set_father(factor);
	return item;
}

GNode* GTree::unsigned2factor(unsigned int v, GNode* father)
{
	GNode* factor = new GNode(G_FACTOR, father);
	GNode* integer = new GNode(G_INT, factor);
	GNode* unsigned_ = new GNode(G_UNSIGNED, to_string(v), integer);
	factor->add_son(integer);
	integer->add_son(unsigned_);
	return factor;
}

string expr_gnode_to_string(GNode*, bool);
string item_gnode_to_string(GNode*);
string factor_gnode_to_string(GNode*);
string integer_to_string(GNode*);

string expr_gnode_to_string(GNode* expr, bool outest)
{
	vector<GNode*> sons = expr->get_sons();
	string ret = "";
	int count = (sons.size() - 1) / 2;
	for (int i = 0; i < count; i++)
	{
		if (!(ret == "" && sons[2 * i + 1]->get_type() == T_PLUS))
			ret += sons[2 * i + 1]->get_type() == T_PLUS ? "+" : "-";
		ret += item_gnode_to_string(sons[2 * i + 2]);
	}
	if (count == 1 || outest) return ret;
	else return "(" + ret + ")";
}

string item_gnode_to_string(GNode* item)
{
	vector<GNode*> sons = item->get_sons();
	string ret = "";
	int count = (sons.size() + 1) / 2;
	for (int i = 0; i < count - 1; i++)
	{
		ret += factor_gnode_to_string(sons[2 * i]);
		ret += sons[2 * i + 1]->get_type() == T_MULT ? "*" : "/";
	}
	ret += factor_gnode_to_string(sons[sons.size() - 1]);
	return ret;
}

string factor_gnode_to_string(GNode* factor)
{
	if (factor->get_type() != G_FACTOR) return "!NOT AN FACTOR";
	if (factor->get_son(0)->get_type() == G1_VAR)
		return factor->get_son(0)->get_son(0)->get_content();
	else if (factor->get_son(0)->get_type() == G_INT)
		return integer_to_string(factor->get_son(0));
	else if (factor->get_son(0)->get_type() == G_CHAR)
		return "'" + factor->get_son(0)->get_content() + "'";
	else if (factor->get_son(0)->get_type() == G_EXPRESSION)
		return expr_gnode_to_string(factor->get_son(0), false);
	else if (factor->get_son(0)->get_type() == G1_ARR1 || \
		factor->get_son(0)->get_son(0)->get_type() == G1_ARR2RE)
		return factor->get_son(0)->get_son(0)->get_content() + "[" + \
		expr_gnode_to_string(factor->get_son(0)->get_son(1), true) + "]";
	else if (factor->get_son(0)->get_type() == G1_ARR2)
		return factor->get_son(0)->get_son(0)->get_content() + "[" + \
		expr_gnode_to_string(factor->get_son(0)->get_son(1), true) + "][" + \
		expr_gnode_to_string(factor->get_son(0)->get_son(2), true) + "]";
	else if (factor->get_son(0)->get_type() == G_CALLNONVOID)
		return factor->get_son(0)->get_son(1)->get_content() + "(..)";
	else return "!UNEXPECTED FALLTHROUGH";
}

string integer_to_string(GNode* integer)
{
	if (integer->get_sons().size() == 1) return integer->get_son(0)->get_content();
	else if (integer->get_son(0)->get_type() == T_MINU)
		return "-" + integer->get_son(1)->get_content();
	return integer->get_son(1)->get_content();
}

void GTree::after_analysis()
{
	if (this->print_log)
	{
		map<string, vector<Var>*>::iterator it;
		for (it = var_table.begin(); it != var_table.end(); it++)
		{
			PRINTLN(">>> FUNCTION: " << it->first);
			vector<Var> vars = *(it->second);
			for (int i = 0; i < vars.size(); i++)
				PRINTLN(vars[i].to_string());
		}
		PRINTLN("+======+");
		PRINTLN("|PCODES|");
		PRINTLN("+======+");
		int indent = 0;
		for (int i = 0; i < pseudos.size(); i++)
		{
			string type = pseudos[i].get_type();
			PRINT("");
			for (int j = 0; j < to_string(pseudos.size() + 1).size() - to_string(i + 1).size(); j++) 
				cout << " ";
			cout << i + 1 << " ";
			if (type == P_FUNC || type == P_IF || type == P_LOOP)
			{
				for (int j = 0; j < indent; j++) cout << "    ";
				cout << pseudos[i].to_string() << endl;
				indent++;
			}
			else if (type == P_ENDIF || type == P_ENDLOOP)
			{
				indent--;
				for (int j = 0; j < indent; j++) cout << "    ";
				cout << pseudos[i].to_string() << endl;
			}
			else if (type == P_ENDFUNC)
			{
				indent--;
				for (int j = 0; j < indent; j++) cout << "    ";
				cout << pseudos[i].to_string() << endl;
				PRINTLN("");
			}
			else if (type == P_ELSE)
			{
				indent--;
				for (int j = 0; j < indent; j++) cout << "    ";
				cout << pseudos[i].to_string() << endl;
				indent++;
			}
			else if (type == P_COMN)
			{
				cout << pseudos[i].to_string() << endl;
			}
			else
			{
				for (int j = 0; j < indent; j++) cout << "    ";
				cout << pseudos[i].to_string() << endl;
			}
		}
	}
}

void GTree::analyze()
{
	this->current_scope = G_GLOBALSCOPE;
	this->programme(root);
	this->pseudos.push_back(Pseudo("@END"));
	this->after_analysis();
}

void GTree::programme(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	int cursor = 0;
	if (cursor < sons.size() && sons[cursor]->get_type() == G_CONSTDEC)
	{
		this->const_declaration(sons[cursor]);
		cursor++;
	}
	if (cursor < sons.size() && sons[cursor]->get_type() == G_VARDEC)
	{
		this->variable_declaration(sons[cursor]);
		cursor++;
	}
	while (cursor < sons.size() && (sons[cursor]->get_type() == G_FUNCNONVOID || \
		sons[cursor]->get_type() == G_FUNCVOID))
	{
		if (sons[cursor]->get_type() == G_FUNCNONVOID)
			this->non_void_function_definition(sons[cursor]);
		else this->void_function_definition(sons[cursor]);
		cursor++;
	}
	if (cursor < sons.size() && sons[cursor]->get_type() == G_MAINFUNC)
	{
		this->main_function(sons[cursor]);
	}
}

void GTree::const_declaration(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	for (int i = 0; i < sons.size(); i++)
		this->const_definition(sons[i]);
}

void GTree::const_definition(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string type = sons[0]->get_type();
	if (type == T_INTTK) this->const_int_definition(curptr);
	else if (type == T_CHARTK) this->const_char_definition(curptr);
}

void GTree::const_int_definition(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	int count = (sons.size() - 1) / 2;
	map<string, vector<Var>*>::iterator it = var_table.find(to_lowercase(current_scope));
	vector<Var>* var_list;
	if (it != var_table.end()) var_list = it->second;
	else
		var_table.insert(pair<string, vector<Var>*>(to_lowercase(current_scope), var_list = new vector<Var>()));
	for (int i = 0; i < count; i++)
	{
		string name = sons[2 * i + 1]->get_content();
		int value = integer(sons[2 * i + 2]);
		Var v = Var(T_INTTK, name, value, true);
		var_list->push_back(v);
	}
}

void GTree::const_char_definition(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	int count = (sons.size() - 1) / 2;
	map<string, vector<Var>*>::iterator it = var_table.find(to_lowercase(current_scope));
	vector<Var>* var_list;
	if (it != var_table.end()) var_list = it->second;
	else
		var_table.insert(pair<string, vector<Var>*>(to_lowercase(current_scope), var_list = new vector<Var>()));
	for (int i = 0; i < count; i++)
	{
		string name = sons[2 * i + 1]->get_content();
		string value = sons[2 * i + 2]->get_content();
		Var v = Var(T_CHARTK, name, value[0], true);
		var_list->push_back(v);
	}
}

void GTree::variable_declaration(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	map<string, vector<Var>*>::iterator it = this->var_table.find(to_lowercase(current_scope));
	vector<Var>* vars;
	if (it == var_table.end())
		var_table.insert(pair<string, vector<Var>*>(to_lowercase(current_scope), vars = new vector<Var>()));
	else vars = it->second;
	for (int i = 0; i < sons.size(); i++)
		this->variable_definition(sons[i], vars);
}

void GTree::variable_definition(GNode* curptr, vector<Var>* vars)
{
	GNode* son = curptr->get_son(0);
	if (son->get_type() == G0_1DIMARRINIT) this->var_1dimarrinit(son, vars);
	else if (son->get_type() == G0_1DIMARRNOINIT) this->var_1dimarrnoinit(son, vars);
	else if (son->get_type() == G0_2DIMARRINIT) this->var_2dimarrinit(son, vars);
	else if (son->get_type() == G0_2DIMARRNOINIT) this->var_2dimarrnoinit(son, vars);
	else if (son->get_type() == G0_MULVARNOINIT) this->var_mulvarnoinit(son, vars);
	else if (son->get_type() == G0_SINGVARINIT) this->var_singlevarinit(son, vars);
	else if (son->get_type() == G0_SINGVARNOINIT) this->var_singlevarnoinit(son, vars);
}

void GTree::var_mulvarnoinit(GNode* curptr, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	string type = sons[0]->get_type();
	for (int i = 1; i < sons.size(); i++)
	{
		if (sons[i]->get_type() == G1_VAR) this->var_g1var(sons[i], type, vars);
		else if (sons[i]->get_type() == G1_ARR1) this->var_g1arr1(sons[i], type, vars);
		else if (sons[i]->get_type() == G1_ARR2) this->var_g1arr2(sons[i], type, vars);
	}
}

void GTree::var_g1var(GNode* curptr, string type, vector<Var>* vars)
{
	vars->push_back(Var(type, curptr->get_son(0)->get_content()));
}

void GTree::var_g1arr1(GNode* curptr, string type, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	vars->push_back(Var(type, sons[0]->get_content(), stoi(sons[1]->get_content())));
}

void GTree::var_g1arr2(GNode* curptr, string type, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	vars->push_back(Var(type, sons[0]->get_content(), stoi(sons[1]->get_content()), stoi(sons[2]->get_content())));
}

void GTree::var_singlevarnoinit(GNode* curptr, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	vars->push_back(Var(sons[0]->get_type(), sons[1]->get_content()));
}

void GTree::var_singlevarinit(GNode* curptr, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	string type = sons[0]->get_type();
	string name = sons[1]->get_content();
	if (type == T_INTTK)
		vars->push_back(Var(type, name, integer(sons[2]), false));
	else if (type == T_CHARTK)
		vars->push_back(Var(type, name, sons[2]->get_content()[0], false));
}

void GTree::var_1dimarrinit(GNode* curptr, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	string type = sons[0]->get_type();
	string name = sons[1]->get_content();
	int dim1 = stoi(sons[2]->get_content());
	if (type == T_INTTK)
	{
		vector<int> value;
		for (int i = 3; i < sons.size(); i++)
			value.push_back(integer(sons[i]));
		vars->push_back(Var(type, name, dim1, value));
	}
	else if (type == T_CHARTK)
	{
		vector<char> value;
		for (int i = 3; i < sons.size(); i++)
			value.push_back(sons[i]->get_content()[0]);
		vars->push_back(Var(type, name, dim1, value));
	}
}

void GTree::var_1dimarrnoinit(GNode* curptr, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	string type = sons[0]->get_type();
	string name = sons[1]->get_content();
	int dim1 = stoi(sons[2]->get_content());
	vars->push_back(Var(type, name, dim1));
}

void GTree::var_2dimarrinit(GNode* curptr, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	string type = sons[0]->get_type();
	string name = sons[1]->get_content();
	int dim1 = stoi(sons[2]->get_content());
	int dim2 = stoi(sons[3]->get_content());
	if (type == T_INTTK)
	{
		vector<int> value;
		for (int i = 4; i < sons.size(); i++)
			value.push_back(integer(sons[i]));
		vars->push_back(Var(type, name, dim1, dim2, value));
	}
	else if (type == T_CHARTK)
	{
		vector<char> value;
		for (int i = 4; i < sons.size(); i++)
			value.push_back(sons[i]->get_content()[0]);
		vars->push_back(Var(type, name, dim1, dim2, value));
	}
}

void GTree::var_2dimarrnoinit(GNode* curptr, vector<Var>* vars)
{
	vector<GNode*> sons = curptr->get_sons();
	string type = sons[0]->get_type();
	string name = sons[1]->get_content();
	int dim1 = stoi(sons[2]->get_content());
	int dim2 = stoi(sons[3]->get_content());
	vars->push_back(Var(type, name, dim1, dim2));
}

pair<string, string> GTree::gconst(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	if (sons[0]->get_type() == G_CHAR)
		return pair<string, string>(sons[0]->get_content(), P_LCHAR);
	else return pair<string, string>(to_string(integer(sons[0])), P_LINT);
}

int GTree::integer(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	if (sons.size() == 1) return stoi(sons[0]->get_content());
	if (sons.size() == 2 && sons[0]->get_type() == T_MINU) return -stoi(sons[1]->get_content());
	return stoi(sons[1]->get_content());
}

void GTree::non_void_function_definition(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	pair<string, string> head = this->func_head(sons[0]);
	this->current_scope = head.second;
	vector<string> atypes;
	vector<pair<string, string>> args;
	this->arg_table(sons[1], &atypes, &args);
	this->func_table.push_back(Func(head.first, head.second, atypes, args));
	this->pseudos.push_back(Pseudo(P_FUNC, pair<string, string>(head.second, head.first), args));
	this->composite_states(sons[2]);
	this->pseudos.push_back(Pseudo(P_ENDFUNC));
	this->current_scope = G_GLOBALSCOPE;
}

void GTree::void_function_definition(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[0]->get_content();
	this->current_scope = name;
	vector<string> atypes;
	vector<pair<string, string>> args;
	this->arg_table(sons[1], &atypes, &args);
	this->func_table.push_back(Func(T_VOIDTK, name, atypes, args));
	this->pseudos.push_back(Pseudo(P_FUNC, pair<string, string>(name, T_VOIDTK), args));
	this->composite_states(sons[2]);
	this->pseudos.push_back(Pseudo(P_ENDFUNC));
	this->current_scope = G_GLOBALSCOPE;
}

void GTree::arg_table(GNode* curptr, vector<string>* atype, vector<pair<string, string>>* args)
{
	vector<GNode*> sons = curptr->get_sons();
	int count = sons.size() / 2;
	for (int i = 0; i < count; i++)
	{
		string type = to_lowercase(sons[2 * i]->get_type()) == "int" ? T_INTTK : T_CHARTK;
		string name = sons[2 * i + 1]->get_content();
		atype->push_back(type);
		args->push_back(pair<string, string>(name, type));
	}
}

pair<string, string> GTree::func_head(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	return pair<string, string>(sons[0]->get_type(), sons[1]->get_content());
}

void GTree::main_function(GNode* curptr)
{
	this->current_scope = G_MAINSCOPE;
	this->func_table.push_back(Func(T_VOIDTK, "main", vector<string>(), vector<P>()));
	this->composite_states(curptr->get_son(0));
}

void GTree::composite_states(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	int cursor = 0;
	if (cursor < sons.size() && sons[cursor]->get_type() == G_CONSTDEC)
	{
		this->const_declaration(sons[cursor]);
		cursor += 1;
	}
	if (cursor < sons.size() && sons[cursor]->get_type() == G_VARDEC)
	{
		this->variable_declaration(sons[cursor]);
		cursor += 1;
	}
	if (cursor < sons.size() && sons[cursor]->get_type() == G_STATECROWD)
	{
		this->state_crowd(sons[cursor]);
	}
}

void GTree::state_crowd(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	for (int i = 0; i < sons.size(); i++)
	{
		string type = sons[i]->get_type();
		if (type == G_LOOPSTATE) this->state_loop(sons[i]);
		else if (type == G_CONDISTATE) this->state_condition(sons[i]);
		else if (type == G_CALLNONVOID) this->state_nonvoid_func_call(sons[i]);
		else if (type == G_CALLVOID) this->state_void_func_call(sons[i]);
		else if (type == G_ASSIGNMENT) this->state_assign(sons[i]);
		else if (type == G_SCANFSTATE) this->state_scanf(sons[i]);
		else if (type == G_PRINTFSTATE) this->state_printf(sons[i]);
		else if (type == G_SWITCHSTATE) this->state_switch(sons[i]);
		else if (type == G_RETURNSTATE) this->state_return(sons[i]);
	}
}

void GTree::state_loop(GNode* curptr)
{
	string type = curptr->get_son(0)->get_type();
	if (type == T_WHILETK) this->state_while(curptr);
	else this->state_for(curptr);
}

void GTree::state_while(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	this->pseudos.push_back(Pseudo(P_STARTLOOP));
	this->loop_condition(sons[1]);
	this->state_crowd(sons[2]);
	this->pseudos.push_back(Pseudo(P_ENDLOOP));
}

void GTree::state_for(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[1]->get_content();
	pair<string, string> expr = expression(sons[2]);
	this->pseudos.push_back(Pseudo(P_ASSN, expr, name));
	this->pseudos.push_back(Pseudo(P_STARTLOOP));
	this->loop_condition(sons[3]);
	this->state_crowd(sons[8]);
	this->for_stride(curptr);
	this->pseudos.push_back(Pseudo(P_ENDLOOP));
}

void GTree::for_stride(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string lvalue = sons[4]->get_content();
	string opr1_name = sons[5]->get_content();
	Var* var = lookup_var(opr1_name);
	pair<string, string> opr1;
	if (var->is_const()) opr1 = pair<string, string>(to_string(var->get_value()), P_LINT);
	else opr1 = pair<string, string>(opr1_name, var->get_type() == T_INTTK ? P_VINT : P_VCHAR);
	pair<string, string> opr2 = pair<string, string>(sons[7]->get_content(), P_LINT);
	if (sons[6]->get_type() == T_PLUS)
		this->pseudos.push_back(Pseudo(P_ADD, opr1, opr2, lvalue));
	else if (sons[6]->get_type() == T_MINU)
		this->pseudos.push_back(Pseudo(P_SUB, opr1, opr2, lvalue));
}

void GTree::loop_condition(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	pair<string, string> opr1 = expression(sons[0]);
	pair<string, string> opr2 = expression(sons[2]);
	string rel = sons[1]->get_son(0)->get_type();
	this->pseudos.push_back(Pseudo(P_LOOP, opr1, rel, opr2));
}

void GTree::state_condition(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	this->condition(sons[0]);
	this->state_crowd(sons[1]);
	if (sons.size() > 2)
	{
		this->pseudos.push_back(Pseudo(P_ELSE));
		this->state_crowd(sons[2]);
	}
	this->pseudos.push_back(Pseudo(P_ENDIF));
}

void GTree::condition(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	pair<string, string> opr1 = expression(sons[0]);
	pair<string, string> opr2 = expression(sons[2]);
	string rel = sons[1]->get_son(0)->get_type();
	this->pseudos.push_back(Pseudo(P_IF, opr1, rel, opr2));
}

pair<string, string> GTree::state_nonvoid_func_call(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[1]->get_content();
	vector<pair<string, string>> v;
	string r = this->gen_new_name();
	Func* func = this->lookup_func(name);
	string rtype = func->get_ret() == T_INTTK ? P_VINT : P_VCHAR;
	this->value_args(sons[2], &v);
	this->pseudos.push_back(Pseudo(P_CALL, name, v, r));
	return pair<string, string>(r, rtype);
}

void GTree::state_void_func_call(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[1]->get_content();
	vector<pair<string, string>> v;
	this->value_args(sons[2], &v);
	this->pseudos.push_back(Pseudo(P_CALN, name, v));
}

void GTree::value_args(GNode* curptr, vector<pair<string, string>>* v)
{
	vector<GNode*> sons = curptr->get_sons();
	for (int i = 0; i < sons.size(); i++)
		v->push_back(expression(sons[i]));
}

void GTree::state_assign(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	if (sons[0]->get_type() == G1_VAR) state_assign_g1var(curptr);
	else if (sons[0]->get_type() == G1_ARR1) state_assign_g1arr1(curptr);
	else if (sons[0]->get_type() == G1_ARR2) state_assign_g1arr2(curptr);
}

void GTree::state_assign_g1var(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[0]->get_son(0)->get_content();
	pair<string, string> p = this->expression(sons[1]);
	this->pseudos.push_back(Pseudo(P_ASSN, p, name));
}

void GTree::state_assign_g1arr1(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[0]->get_son(0)->get_content();
	pair<string, string> offset = expression(sons[0]->get_son(1));
	this->pseudos.push_back(Pseudo(P_ASSR, expression(sons[1]), name, offset));
}

void GTree::state_assign_g1arr2(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	if (sons[0]->get_type() == G1_ARR2) this->rearrange_g1arr2(sons[0]);
	this->state_assign_g1arr1(curptr);
}

void GTree::state_scanf(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[0]->get_content();
	Var* var = lookup_var(name);
	string type = var->get_type() == T_INTTK ? READ_INT : READ_CHAR;
	this->pseudos.push_back(Pseudo(P_READ, pair<string, string>(name, type)));
}

void GTree::state_printf(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	if (sons.size() == 2) this->state_print_str_expr(curptr);
	else if (sons.size() == 1 && sons[0]->get_type() == G_STR) this->state_print_str(curptr);
	else this->state_print_expr(curptr);
}

void GTree::state_print_str(GNode* curptr)
{
	string content = curptr->get_son(0)->get_content();
	this->pseudos.push_back(Pseudo(P_DISP, pair<string, string>(content, DISP_STR), DISP_ENDL));
}

void GTree::state_print_expr(GNode* curptr)
{
	this->pseudos.push_back(Pseudo(P_DISP, expression(curptr->get_son(0)), DISP_ENDL));
}

void GTree::state_print_str_expr(GNode* curptr)
{
	string content = curptr->get_son(0)->get_content();
	this->pseudos.push_back(Pseudo(P_DISP, pair<string, string>(content, DISP_STR), DISP_END));
	this->pseudos.push_back(Pseudo(P_DISP, expression(curptr->get_son(1)), DISP_ENDL));
}

void GTree::state_switch(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	pair<string, string> expr = this->expression(sons[0]);
	int cases = this->case_table(sons[1], expr);
	this->default_case(sons[2]);
	for (int i = 0; i < cases; i++) this->pseudos.push_back(Pseudo(P_ENDIF));
}

int GTree::case_table(GNode* curptr, pair<string, string> expr)
{
	vector<GNode*> sons = curptr->get_sons();
	int count = sons.size() / 2;
	for (int i = 0; i < count; i++)
	{
		pair<string, string> constance = gconst(sons[2 * i]);
		this->pseudos.push_back(Pseudo(P_IF, expr, T_EQL, constance));
		this->state_crowd(sons[2 * i + 1]);
		this->pseudos.push_back(Pseudo(P_ELSE));
	}
	return count;
}

void GTree::default_case(GNode* curptr)
{
	this->state_crowd(curptr->get_son(0));
}

void GTree::state_return(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	if (sons.size() == 0) this->pseudos.push_back(Pseudo(P_RETN));
	else this->pseudos.push_back(Pseudo(P_RET, expression(sons[0])));
}

pair<string, string> GTree::lint2lchar(pair<string, string> p)
{
	return pair<string, string>(string() + (char)stoi(p.first), P_LCHAR);
}

pair<string, string> GTree::lchar2lint(pair<string, string> p)
{
	return pair<string, string>(to_string((int)p.first[0]), P_LINT);
}

pair<string, string> GTree::expression(GNode* curptr)
{
	if (this->print_log) pseudos.push_back(Pseudo(P_COMN, "expr: " + expr_gnode_to_string(curptr, true)));
	vector<GNode*> sons = curptr->get_sons();
	string expr_type = sons[0]->get_type();
	int count = (sons.size() - 1) / 2;
	if (count <= 1 && sons[1]->get_type() == T_PLUS)
	{
		pair<string, string> p = this->item(sons[2]);
		return (expr_type == T_INTTK && p.second == P_LINT) ? p :
			(expr_type == T_INTTK && p.second == P_LCHAR) ? lchar2lint(p) :
			(expr_type == T_CHARTK && p.second == P_LINT) ? lint2lchar(p) : 
			(expr_type == T_CHARTK && p.second == P_LCHAR) ? p :
			(expr_type == T_INTTK) ? pair<string, string>(p.first, P_VINT) :
			/* (expr_type == T_CHARTK) ? */ pair<string, string>(p.first, P_VCHAR);
	}
	else if (count <= 1)
	{
		pair<string, string> p = this->item(sons[2]);
		if (p.second == P_LINT) return pair<string, string>(to_string(-stoi(p.first)), p.second);
		else if (p.second == P_LCHAR) pair<string, string>(to_string(-(int)p.first[0]), P_LINT);
		else
		{
			P r = pair<string, string>(gen_new_name(), P_VINT);
			this->pseudos.push_back(Pseudo(P_SUB, pair<string, string>("0", P_LINT), p, r.first));
			return r;
		}
	}
	pair<string, string> r, last;
	if (sons[1]->get_type() == T_PLUS)
	{
		last = this->item(sons[2]);
	}
	else /* if (sons[1]->get_type() == T_MINU) */
	{
		last = pair<string, string>(gen_new_name(), P_VINT);
		this->pseudos.push_back(Pseudo(P_SUB, pair<string, string>("0", P_LINT), item(sons[2]), last.first));
	}
	for (int i = 1; i < count; i++)
	{
		r = pair<string, string>(gen_new_name(), P_VINT);
		if (sons[2 * i + 1]->get_type() == T_PLUS)
			this->pseudos.push_back(Pseudo(P_ADD, last, item(sons[2 * i + 2]), r.first));
		else this->pseudos.push_back(Pseudo(P_SUB, last, item(sons[2 * i + 2]), r.first));
		last = r;
	}
	return r;
}

pair<string, string> GTree::item(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	int count = (sons.size() + 1) / 2;
	if (count <= 1) return this->factor(sons[0]);
	pair<string, string> r;
	pair<string, string> last = factor(sons[0]);
	for (int i = 0; i < count - 1; i++)
	{
		r = pair<string, string>(gen_new_name(), P_VINT);
		if (sons[2 * i + 1]->get_type() == T_MULT)
			this->pseudos.push_back(Pseudo(P_MUL, last, factor(sons[2 * i + 2]), r.first));
		else this->pseudos.push_back(Pseudo(P_DIV, last, factor(sons[2 * i + 2]), r.first));
		last = r;
	}
	return r;
}

pair<string, string> GTree::factor(GNode* curptr)
{
	GNode* son = curptr->get_son(0);
	if (son->get_type() == G1_VAR) return this->factor_g1var(son);
	else if (son->get_type() == G1_ARR1 || son->get_type() == G1_ARR2RE) 
		return this->factor_g1arr1(son);
	else if (son->get_type() == G1_ARR2) return this->factor_g1arr2(son);
	else if (son->get_type() == G_INT) return this->factor_int(son);
	else if (son->get_type() == G_CHAR) return this->factor_char(son);
	else if (son->get_type() == G_EXPRESSION) return this->expression(son);
	else if (son->get_type() == G_CALLNONVOID) return this->state_nonvoid_func_call(son);
	WARN("Unexpected fallthrough in `factor`");
	return pair<string, string>("0", P_LINT);
}

pair<string, string> GTree::const2int(Var v)
{
	int val = v.get_value();
	string type = (v.get_type() == T_INTTK) ? P_LINT : P_LCHAR;
	string name = (v.get_type() == T_INTTK) ? to_string(val) : string() + (char)val;
	return pair<string, string>(name, type);
}

pair<string, string> GTree::factor_g1var(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[0]->get_content();
	Var* v = this->lookup_var(name);
	if (v->is_const()) return const2int(*v);
	string type = (v->get_type() == T_INTTK) ? P_VINT : P_VCHAR;
	return pair<string, string>(name, type);
}

pair<string, string> GTree::factor_g1arr1(GNode* curptr)
{
	vector<GNode*> sons = curptr->get_sons();
	string name = sons[0]->get_content();
	Var* v = this->lookup_var(name);
	string type = v->get_type() == T_INTTK ? P_VINT : P_VCHAR;
	pair<string, string> ref = this->expression(sons[1]);
	string r = gen_new_name();
	this->pseudos.push_back(Pseudo(P_ARR, name, ref, r));
	return pair<string, string>(r, type);
}

pair<string, string> GTree::factor_g1arr2(GNode* curptr)
{
	this->rearrange_g1arr2(curptr);
	return this->factor_g1arr1(curptr);
}

pair<string, string> GTree::factor_int(GNode* curptr)
{
	return pair<string, string>(to_string(integer(curptr)), P_LINT);
}

pair<string, string> GTree::factor_char(GNode* curptr)
{
	return pair<string, string>(curptr->get_content(), P_LCHAR);
}
