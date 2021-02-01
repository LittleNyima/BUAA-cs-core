#include "grammar.h"
#include <iostream>
#include <fstream>
#include "util.h"

#define LOG(m) do{if(this->print_log){cout<<m;}}while(0)
#define LOGLN(m) do{if(this->print_log){cout<<m<<endl;}}while(0)

Grammar::Grammar()
{
	this->length = 0;
	this->offset = 0;
	this->print2file = false;
	this->print_log = false;
	this->print_err2file = false;
	this->have_return_flag = false;
	this->current_scope = GLOBAL_SCOPE;
}

void Grammar::debug_mode()
{
	this->print2file = true;
	this->print_log = true;
	this->print_err2file = true;
}

void Grammar::submit_mode(string mode)
{
	if (mode == "grammar")
	{
		this->print_log = false;
		this->print2file = true;
		this->print_err2file = false;
	}
	else if (mode == "errorproc")
	{
		this->print_log = false;
		this->print2file = false;
		this->print_err2file = true;
	}
}

vector<ErrorDescripter> Grammar::get_errors()
{
	return this->reporter.get_errors();
}

GTree Grammar::build_tree()
{
	return GTree(this->root);
}

void Grammar::error()
{
	// undefined api
}

void Grammar::error(string expected, string got, int line)
{
	LOG("Compile error occurs at line ");
	LOG(line);
	LOG(" in grammar.cpp, got ");
	LOGLN(got + " when we expected " + expected + ".");
}

void Grammar::parse(vector<BaseToken> tokens)
{
	this->tokens = tokens;
	this->offset = 0;
	this->length = tokens.size();
	this->root = new GNode(G_PROGRAM);
	this->current = root;
	program();
	after_parse();
}

void Grammar::after_parse()
{
	if (this->print2file)
	{
		ofstream fout("output.txt");
		if (fout.is_open())
		{
			stack<string> inv;
			while (!this->foutput.empty())
			{
				inv.push(this->foutput.top());
				this->foutput.pop();
			}
			while (!inv.empty())
			{
				fout << inv.top() << endl;
				inv.pop();
			}
			fout.close();
		}
	}
	if (this->print_log)
	{
		this->print_symbol_table();
	}
}

BaseToken Grammar::fetch_token()
{
	if (this->offset < this->length)
	{
		BaseToken token = this->tokens[this->offset];
		this->foutput.push(token.to_string());
		LOG("[LEX] Push: ");
		LOGLN(token.to_string());
		this->offset += 1;
		return token;
	}
	else
	{
		LOGLN("[WARNING] Fetch reached EOF");
		return BaseToken("EOF", "", -1, -1);
	}
}

void Grammar::give_back_token()
{
	if (!this->foutput.empty() && this->offset > 0)
	{
		LOG("[LEX] Pop: ");
		LOGLN(foutput.top());
		this->foutput.pop();
		if (foutput.empty() || !starts_with(foutput.top(), "EOF"))
		{
			this->offset -= 1;
		}
	}
}

void Grammar::push_grammar_token(string token, string alias)
{
	this->foutput.push(token);
	this->grammar_tokens.push_back(token);
	LOG("[GRAMMAR] Push: ");
	LOGLN(alias);
}

void Grammar::print_symbol_table()
{
	LOGLN("[AFTER] Function Table");
	for (int i = 0; i < function_table.size(); i++)
	{
		LOGLN("[AFTER]  - " << function_table[i].to_string());
	}
	LOGLN("[AFTER] Variable Table");
	for (int i = 0; i < variable_table.size(); i++)
	{
		LOGLN("[AFTER]  - " << variable_table[i].to_string());
	}
}

FunctionSymbol Grammar::search_for_function(string name)
{
	string _name = to_lowercase(name);
	for (int i = 0; i < this->function_table.size(); i++)
	{
		FunctionSymbol fs = function_table[i];
		string fname = to_lowercase(fs.get_name());
		if (_name == fname) return function_table[i];
	}
	return FunctionSymbol();
}

VariableSymbol Grammar::search_for_variable(string name, string scope)
{
	string _name = to_lowercase(name);
	string _scope = to_lowercase(scope);
	for (int i = 0; i < this->variable_table.size(); i++)
	{
		VariableSymbol vs = variable_table[i];
		string vname = to_lowercase(vs.get_name());
		string vscope = to_lowercase(vs.get_scope());
		if (_name == vname && _scope == vscope) return variable_table[i];
	}
	return VariableSymbol();
}

void Grammar::plus_operator()
{
	// This function shouldn't be called.
	LOGLN("[CALL] Grammar::plus_operator");
	LOGLN("[ERROR] This shouldn't be called!");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_PLUS && type != T_MINU)
	{
		error(string() + T_PLUS + " or " + T_MINU, type, __LINE__);
	}
}

void Grammar::mult_operator()
{
	// This function shouldn't be called.
	LOGLN("[CALL] Grammar::mult_operator()");
	LOGLN("[ERROR] This shouldn't be called!");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_MULT && type != T_DIV)
	{
		error(string() + T_MULT + " or " + T_DIV, type, __LINE__);
	}
}

string Grammar::rel_operator()
{
	LOGLN("[CALL] Grammar::rel_operator");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_LSS && type != T_LEQ && type != T_GRE && \
		type != T_GEQ && type != T_EQL && type != T_NEQ)
	{
		error("REL_OP", type, __LINE__);
	}
	this->current->add_son(new GNode(type, current));
	return type;
}

void Grammar::letter()
{
	// This function shouldn't be called.
	LOGLN("[CALL] Grammar::letter");
	LOGLN("[ERROR] This shouldn't be called!");
}

void Grammar::digit()
{
	// This function shouldn't be called.
	LOGLN("[CALL] Grammar::digit");
	LOGLN("[ERROR] This shouldn't be called!");
}

void Grammar::character()
{
	// This function shouldn't be called.
	LOGLN("[CALL] Grammar::character");
	LOGLN("[ERROR] This shouldn't be called!");
}

void Grammar::string_()
{
	LOGLN("[CALL] Grammar::string_");
	BaseToken token = this->fetch_token();
	if (token.get_type() != T_STRCON)
	{
		error(T_STRCON, token.get_type(), __LINE__);
	}
	this->current->add_son(new GNode(G_STR, token.get_content(), current));
	this->push_grammar_token("<字符串>", "<String>");
}

void Grammar::program()
{
	LOGLN("[CALL] Grammar::program");

	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	this->give_back_token();
	if (type0 == T_CONSTTK)
	{
		GNode* node = new GNode(G_CONSTDEC, current);
		this->current->add_son(node);
		this->current = node;
		this->const_declaration();
		this->current = current->get_father();
	}

	token0 = this->fetch_token();
	BaseToken token1 = this->fetch_token();
	BaseToken token2 = this->fetch_token();
	type0 = token0.get_type();
	string type1 = token1.get_type();
	string type2 = token2.get_type();
	this->give_back_token();
	this->give_back_token();
	this->give_back_token();
	if (!(type0 == T_VOIDTK && type1 == T_MAINTK)/* not main */ && \
		!((type0 == T_INTTK || type0 == T_CHARTK || type0 == T_VOIDTK) && \
			type1 == T_IDENFR && type2 == T_LPARENT)/* not function */)
	{
		GNode* node = new GNode(G_VARDEC, current);
		this->current->add_son(node);
		this->current = node;
		this->variable_declaration();
		this->current = current->get_father();
	}

	while (true)
	{
		token0 = this->fetch_token();
		token1 = this->fetch_token();
		type0 = token0.get_type();
		type1 = token1.get_type();
		this->give_back_token();
		this->give_back_token();
		if ((type0 == T_INTTK || type0 == T_CHARTK) && type1 == T_IDENFR)
		{
			GNode* node = new GNode(G_FUNCNONVOID, current);
			this->current->add_son(node);
			this->current = node;
			this->function_definition_with_return_value();
			this->current = current->get_father();
		}
		else if (type0 == T_VOIDTK && type1 == T_IDENFR)
		{
			GNode* node = new GNode(G_FUNCVOID, current);
			this->current->add_son(node);
			this->current = node;
			this->function_definition_without_return_value();
			this->current = current->get_father();
		}
		else
		{
			break;
		}
	}

	GNode* node = new GNode(G_MAINFUNC, current);
	this->current->add_son(node);
	this->current = node;
	main_function();
	this->current = current->get_father();

	if (!starts_with(this->fetch_token().get_type(), "EOF"))
	{
		error("EOF", "!EOF", __LINE__);
	}

	this->push_grammar_token("<程序>", "<Program>");
}

void Grammar::const_declaration()
{
	LOGLN("[CALL] Grammar::const_declaration");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_CONSTTK)
	{
		error(T_CONSTTK, type0, __LINE__);
	}

	GNode* node = new GNode(G_CONSTDEF, current);
	this->current->add_son(node);
	this->current = node;
	const_definition();
	this->current = current->get_father();

	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_SEMICN)
	{
		error(T_SEMICN, type0, __LINE__);
		{
			// Exception: Missing semicon when expected.
			// We simply give back the token and continue the analysis.
			this->give_back_token();
			this->give_back_token();
			token0 = this->fetch_token();
			// Notice that the error should be reported at the token ahead of
			// the position where semicon is expected, so we give back twice,
			// and then we restore the context after reporting the error.
			reporter.add_error(E_EXPECTSEMICN, token0.get_start_line(), "Missing semicon" + to_string(__LINE__));
			token0 = this->fetch_token();
			this->give_back_token();
		}
	}

	while (true)
	{
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_CONSTTK)
		{
			this->give_back_token();
			break;
		}

		GNode* node = new GNode(G_CONSTDEF, current);
		this->current->add_son(node);
		this->current = node;
		const_definition();
		this->current = current->get_father();

		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_SEMICN)
		{
			error(T_SEMICN, type0, __LINE__);
			{
				// Exception: Missing semicon when expected.
				// We simply give back the token and continue the analysis.
				this->give_back_token();
				this->give_back_token();
				token0 = this->fetch_token();
				// Notice that the error should be reported at the token ahead of
				// the position where semicon is expected, so we give back twice,
				// and then we restore the context after reporting the error.
				reporter.add_error(E_EXPECTSEMICN, token0.get_start_line(), "Missing semicon" + to_string(__LINE__));
				token0 = this->fetch_token();
				this->give_back_token();
			}
		}
	}

	this->push_grammar_token("<常量说明>", "<Const Declaration>");
}

void Grammar::const_definition()
{
	LOGLN("[CALL] Grammar::const_definition");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_INTTK && type0 != T_CHARTK)
	{
		error(string("") + T_INTTK + " " + T_CHARTK, type0, __LINE__);
	}
	else if (type0 == T_INTTK)
	{
		this->current->add_son(new GNode(T_INTTK, current));
		string type_ = token0.get_content();
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_IDENFR)
		{
			error(T_IDENFR, type0, __LINE__);
		}
		string name_ = token0.get_content();
		this->current->add_son(new GNode(T_IDENFR, name_, current));
		{
			// Check: redefinition
			if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) ||\
				!search_for_variable(name_, current_scope).is_empty())
			{
				reporter.add_error(E_REDEFINITION, token0.get_start_line(), name_ + " redefined." + to_string(__LINE__));
			}
		}
		this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, true));
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 !=  T_ASSIGN)
		{
			error(T_ASSIGN, type0, __LINE__);
		}
		GNode* node = new GNode(G_INT, this->current);
		this->current->add_son(node);
		this->current = node;
		this->int_();
		this->current = current->get_father();

		while (true)
		{
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_COMMA)
			{
				this->give_back_token();
				break;
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_IDENFR)
			{
				error(T_IDENFR, type0, __LINE__);
			}
			name_ = token0.get_content();
			this->current->add_son(new GNode(T_IDENFR, name_, current));
			{
				// Check: redefinition
				if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
					!search_for_variable(name_, current_scope).is_empty())
				{
					reporter.add_error(E_REDEFINITION, token0.get_start_line(), string() + name_ + " redefined." + to_string(__LINE__));
				}
			}
			this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, true));
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_ASSIGN)
			{
				error(T_ASSIGN, type0, __LINE__);
			}
			node = new GNode(G_INT, this->current);
			this->current->add_son(node);
			this->current = node;
			this->int_();
			this->current = current->get_father();
		}
		this->push_grammar_token("<常量定义>", "<Const Definition>");
	}
	else if (type0 == T_CHARTK)
	{
		this->current->add_son(new GNode(T_CHARTK, current));
		string type_ = token0.get_content();
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_IDENFR)
		{
			error(T_IDENFR, type0, __LINE__);
		}
		string name_ = token0.get_content();
		this->current->add_son(new GNode(T_IDENFR, name_, current));
		{
			// Check: redefinition
			if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
				!search_for_variable(name_, current_scope).is_empty())
			{
				reporter.add_error(E_REDEFINITION, token0.get_start_line(), string() + name_ + " redefined." + to_string(__LINE__));
			}
		}
		this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, true));
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_ASSIGN)
		{
			error(T_ASSIGN, type0, __LINE__);
		}
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_CHARCON)
		{
			error(T_CHARCON, type0, __LINE__);
		}
		this->current->add_son(new GNode(G_CHAR, token0.get_content(), current));

		while (true)
		{
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_COMMA)
			{
				this->give_back_token();
				break;
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_IDENFR)
			{
				error(T_IDENFR, type0, __LINE__);
			}
			name_ = token0.get_content();
			this->current->add_son(new GNode(T_IDENFR, name_, current));
			{
				// Check: redefinition
				if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
					!search_for_variable(name_, current_scope).is_empty())
				{
					reporter.add_error(E_REDEFINITION, token0.get_start_line(), string() + name_ + " redefined." + to_string(__LINE__));
				}
			}
			this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, true));
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_ASSIGN)
			{
				error(T_ASSIGN, type0, __LINE__);
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_CHARCON)
			{
				error(T_CHARCON, type0, __LINE__);
			}
			this->current->add_son(new GNode(G_CHAR, token0.get_content(), current));
		}
		this->push_grammar_token("<常量定义>", "<Const Definition>");
	}
}

int Grammar::unsigned_int()
{
	LOGLN("[CALL] Grammar::unsigned_int");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_INTCON)
	{
		error(T_INTCON, type0, __LINE__);
	}

	this->push_grammar_token("<无符号整数>", "<Unsigned Int>");
	return stoi(token0.get_content());
}

int Grammar::int_()
{
	LOGLN("[CALL] Grammar::int_");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	char symbol = '+';
	if (type0 != T_PLUS && type0 != T_MINU)
	{
		this->give_back_token();
	}
	else
	{
		this->current->add_son(new GNode(type0, token0.get_content(), current));
		symbol = type0 == T_MINU ? '-' : '+';
	}
	int value = this->unsigned_int();
	this->current->add_son(new GNode(G_UNSIGNED, to_string(value), current));

	this->push_grammar_token("<整数>", "<Int>");
	return symbol == '+' ? value : -value;
}

void Grammar::identifier()
{
	// This function shouldn't be called.
	LOGLN("[CALL] Grammar::identifier");
	LOGLN("[ERROR] This shouldn't be called!");
}

vector<string> Grammar::declaration_header()
{
	LOGLN("[CALL] Grammar::declaration_header");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_INTTK && type0 != T_CHARTK)
	{
		error(string() + T_INTTK + " or " + T_CHARTK, type0, __LINE__);
	}
	this->current->add_son(new GNode(type0, current));
	BaseToken token1 = this->fetch_token();
	string type1 = token1.get_type();
	if (type1 != T_IDENFR)
	{
		error(T_IDENFR, type0, __LINE__);
	}
	this->current->add_son(new GNode(T_IDENFR, token1.get_content()));
	{
		// Check: redefinition
		string name_ = token1.get_content();
		if (!search_for_variable(name_, GLOBAL_SCOPE).is_empty() || !search_for_function(name_).is_empty())
		{
			reporter.add_error(E_REDEFINITION, token1.get_start_line(), string() + name_ + " redefined." + to_string(__LINE__));
		}
	}
	this->grammar_function_table.push_back(GrammarFunctionSymbol(to_lowercase(token0.get_content()), token1.get_content()));
	this->push_grammar_token("<声明头部>", "<Declaration Header>");
	vector<string> ret;
	ret.push_back(token0.get_content());
	ret.push_back(token1.get_content());
	return ret;
}

string Grammar::constance()
{
	LOGLN("[CALL] Grammar::constance");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	string type_;
	if (type0 == T_INTCON || type0 == T_PLUS || type0 == T_MINU)
	{
		this->give_back_token();
		GNode* node = new GNode(G_INT, current);
		current->add_son(node);
		current = node;
		this->int_();
		current = current->get_father();
		type_ = "int";
	}
	else if (type0 != T_CHARCON)
	{
		error(string() + T_INTCON + " or " + T_CHARCON, type0, __LINE__);
		type_ = "undefined";
	}
	else
	{
		type_ = "char";
		this->current->add_son(new GNode(G_CHAR, token0.get_content(), current));
	}
	this->push_grammar_token("<常量>", "<Constance>");
	return type_;
}

void Grammar::variable_declaration()
{
	LOGLN("[CALL] Grammar::variable_declaration");
	GNode* node = new GNode(G_VARDEF, current);
	this->current->add_son(node);
	this->current = node;
	variable_definition();
	this->current = current->get_father();
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_SEMICN)
	{
		error(T_SEMICN, type0, __LINE__);
		{
			// Exception: Missing semicon when expected.
			// We simply give back the token and continue the analysis.
			this->give_back_token();
			this->give_back_token();
			token0 = this->fetch_token();
			// Notice that the error should be reported at the token ahead of
			// the position where semicon is expected, so we give back twice,
			// and then we restore the context after reporting the error.
			reporter.add_error(E_EXPECTSEMICN, token0.get_start_line(), "Missing semicon" + to_string(__LINE__));
			token0 = this->fetch_token();
			this->give_back_token();
		}
	}

	while (true)
	{
		token0 = this->fetch_token();
		string type0 = token0.get_type();
		BaseToken token1 = this->fetch_token();
		string type1 = token1.get_type();
		BaseToken token2 = this->fetch_token();
		string type2 = token2.get_type();
		this->give_back_token();
		this->give_back_token();
		this->give_back_token();
		if ((type0 != T_INTTK && type0 != T_CHARTK) || type1 != T_IDENFR || \
			type2 == T_LPARENT)
		{
			break;
		}

		GNode* node = new GNode(G_VARDEF, current);
		this->current->add_son(node);
		this->current = node;
		variable_definition();
		this->current = current->get_father();

		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_SEMICN)
		{
			error(T_SEMICN, type0, __LINE__);
			{
				// Exception: Missing semicon when expected.
				// We simply give back the token and continue the analysis.
				this->give_back_token();
				this->give_back_token();
				token0 = this->fetch_token();
				// Notice that the error should be reported at the token ahead of
				// the position where semicon is expected, so we give back twice,
				// and then we restore the context after reporting the error.
				reporter.add_error(E_EXPECTSEMICN, token0.get_start_line(), "Missing semicon" + to_string(__LINE__));
				token0 = this->fetch_token();
				this->give_back_token();
			}
		}
	}

	this->push_grammar_token("<变量说明>", "<Variable Declaration>");
}

/*
 * A simple hint of the control flow of this function:
 *  branch 1: main
 *   |- branch 1.1: 1-dimensional array
 *   |   |- branch 1.1.1: 2-dimensional array
 *   |   |   |- branch 1.1.1.1**: multiple variables definition without initialization
 *   |   |   |- branch 1.1.1.2* : single 2-dimensional array definition with initialization
 *   |   |   |- branch 1.1.1.3* : single 2-dimensional array definition without initialization
 *   |   |- branch 1.1.2**: multiple variables definition without initialization
 *   |   |- branch 1.1.3* : single 1-dimensional array definition with initialization
 *   |   |- branch 1.1.4* : single 1-dimensional array definition without initialization
 *   |- branch 1.2**: multiple variables definition without initialization
 *   |- branch 1.3* : single variable definition with initialization
 *   |- branch 1.4* : single variable definition without initialization
 */
void Grammar::variable_definition()
{
	// This is a combination of the following 2 functions.
	LOGLN("[CALL] Grammar::variable_definition");
	GNode* temp_root = new GNode(G_RESERVERD);
	this->current->add_son(temp_root);
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_INTTK && type0 != T_CHARTK)
	{
		error(string() + T_INTTK + " or " + T_CHARTK, type0, __LINE__);
	}
	string type_ = token0.get_content();
	temp_root->add_son(new GNode(type0, temp_root));

	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_IDENFR)
	{
		error(T_IDENFR, type0, __LINE__);
	}
	string name_ = token0.get_content();
	//temp_root->add_son(new GNode(T_IDENFR, name_, temp_root));
	int ln_ = token0.get_start_line();

	token0 = this->fetch_token();
	type0 = token0.get_type();
	/* branch 1.1 */if (type0 == T_LBRACK)
	{
		// 1-dimensional array
		int dim1 = this->unsigned_int();
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_RBRACK)
		{
			error(T_RBRACK, type0, __LINE__);
			{
				// Exception: Missing right bracket when expected.
				// We simply give back the token and continue the analysis.
				reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
				this->give_back_token();
			}
		}
		token0 = this->fetch_token();
		type0 = token0.get_type();

		/* branch 1.1.1 */if (type0 == T_LBRACK)
		{
			// 2-dimensional array
			int dim2 = this->unsigned_int();
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_RBRACK)
			{
				error(T_RBRACK, type0, __LINE__);
				{
					// Exception: Missing right bracket when expected.
					// We simply give back the token and continue the analysis.
					reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
					this->give_back_token();
				}
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			
			/* branch 1.1.1.1 */if (type0 == T_COMMA)
			{
				// multiple variables definition without initialization
				GNode* node1 = new GNode(G1_ARR2, temp_root);
				temp_root->add_son(node1);
				node1->add_son(new GNode(T_IDENFR, name_, node1));
				node1->add_son(new GNode(G_UNSIGNED, to_string(dim1), node1));
				node1->add_son(new GNode(G_UNSIGNED, to_string(dim2), node1));
				this->give_back_token();
				{
					// Check: redefinition
					if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) ||\
						!search_for_variable(name_, current_scope).is_empty())
					{
						reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
					}
				}
				this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1, dim2));
				while (true)
				{
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_COMMA)
					{
						this->give_back_token();
						break;
					}
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_IDENFR)
					{
						error(T_IDENFR, type0, __LINE__);
					}
					name_ = token0.get_content();
					ln_ = token0.get_start_line();
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_LBRACK)
					{
						node1 = new GNode(G1_VAR, temp_root);
						temp_root->add_son(node1);
						node1->add_son(new GNode(T_IDENFR, name_, node1));
						this->give_back_token();
						{
							// Check: redefinition
							if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
								!search_for_variable(name_, current_scope).is_empty())
							{
								reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
							}
						}
						this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false));
						continue;
					}
					dim1 = this->unsigned_int();
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_RBRACK)
					{
						error(T_RBRACK, type0, __LINE__);
						{
							// Exception: Missing right bracket when expected.
							// We simply give back the token and continue the analysis.
							reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
							this->give_back_token();
						}
					}
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_LBRACK)
					{
						node1 = new GNode(G1_ARR1, temp_root);
						temp_root->add_son(node1);
						node1->add_son(new GNode(T_IDENFR, name_, node1));
						node1->add_son(new GNode(G_UNSIGNED, to_string(dim1), node1));
						this->give_back_token();
						{
							// Check: redefinition
							if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
								!search_for_variable(name_, current_scope).is_empty())
							{
								reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
							}
						}
						this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1));
						continue;
					}
					dim2 = this->unsigned_int();
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_RBRACK)
					{
						error(T_RBRACK, type0, __LINE__);
						{
							// Exception: Missing right bracket when expected.
							// We simply give back the token and continue the analysis.
							reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
							this->give_back_token();
						}
					}
					node1 = new GNode(G1_ARR2, temp_root);
					temp_root->add_son(node1);
					node1->add_son(new GNode(T_IDENFR, name_, node1));
					node1->add_son(new GNode(G_UNSIGNED, to_string(dim1), node1));
					node1->add_son(new GNode(G_UNSIGNED, to_string(dim2), node1));
					{
						// Check: redefinition
						if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
							!search_for_variable(name_, current_scope).is_empty())
						{
							reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
						}
					}
					this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1, dim2));
				}
				temp_root->set_type(G0_MULVARNOINIT);
				this->push_grammar_token("<变量定义无初始化>", "Variable No Initialize");
			}
			/* branch 1.1.1.2 */else if (type0 == T_ASSIGN)
			{
				// single 2-dimensional array definition with initialization
				temp_root->add_son(new GNode(T_IDENFR, name_, temp_root));
				temp_root->add_son(new GNode(G_UNSIGNED, to_string(dim1), temp_root));
				temp_root->add_son(new GNode(G_UNSIGNED, to_string(dim2), temp_root));
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_LBRACE)
				{
					error(T_LBRACE, type0, __LINE__);
				}
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_LBRACE)
				{
					error(T_LBRACE, type0, __LINE__);
				}
				int this_dim1 = 0, this_dim2 = 0;
				GNode* old_current = this->current;
				this->current = temp_root;
				string this_type = this->constance();
				this->current = old_current;
				{
					// Check: variable type matches
					if (to_lowercase(this_type) != to_lowercase(type_))
					{
						this->give_back_token();
						BaseToken token_ = this->fetch_token();
						reporter.add_error(E_ILLEGALCONST, token_.get_start_line(), "Variable type mismatches");
					}
				}
				this_dim2++;
				while (true)
				{
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_COMMA)
					{
						this->give_back_token();
						break;
					}
					old_current = this->current;
					this->current = temp_root;
					this_type = this->constance();
					this->current = old_current;
					{
						// Check: variable type matches
						if (to_lowercase(this_type) != to_lowercase(type_))
						{
							this->give_back_token();
							BaseToken token_ = this->fetch_token();
							reporter.add_error(E_ILLEGALCONST, token_.get_start_line(), "Variable type mismatches");
						}
					}
					this_dim2++;
				}
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_RBRACE)
				{
					error(T_RBRACE, type0, __LINE__);
				}
				bool error_reported = false;
				{
					// Check: this_dim2 should equals to dim2
					if (!error_reported && dim2 != this_dim2)
					{
						reporter.add_error(E_ILLEGALARRAY, token0.get_start_line(), "array dimension mismatch" + to_string(__LINE__));
					}
				}
				this_dim1++;
				this_dim2 = 0;
				while (true)
				{
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_COMMA)
					{
						this->give_back_token();
						break;
					}
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_LBRACE)
					{
						error(T_LBRACE, type0, __LINE__);
					}
					old_current = this->current;
					this->current = temp_root;
					this_type = this->constance();
					this->current = old_current;
					{
						// Check: variable type matches
						if (to_lowercase(this_type) != to_lowercase(type_))
						{
							this->give_back_token();
							BaseToken token_ = this->fetch_token();
							reporter.add_error(E_ILLEGALCONST, token_.get_start_line(), "Variable type mismatches");
						}
					}
					this_dim2++;
					while (true)
					{
						token0 = this->fetch_token();
						type0 = token0.get_type();
						if (type0 != T_COMMA)
						{
							this->give_back_token();
							break;
						}
						old_current = this->current;
						this->current = temp_root;
						this_type = this->constance();
						this->current = old_current;
						{
							// Check: variable type matches
							if (to_lowercase(this_type) != to_lowercase(type_))
							{
								this->give_back_token();
								BaseToken token_ = this->fetch_token();
								reporter.add_error(E_ILLEGALCONST, token_.get_start_line(), "Variable type mismatches");
							}
						}
						this_dim2++;
					}
					token0 = this->fetch_token();
					type0 = token0.get_type();
					if (type0 != T_RBRACE)
					{
						error(T_RBRACE, type0, __LINE__);
					}
					{
						// Check: this_dim2 should equals to dim2
						if (!error_reported && dim2 != this_dim2)
						{
							reporter.add_error(E_ILLEGALARRAY, token0.get_start_line(), "array dimension mismatch" + to_string(__LINE__));
						}
					}
					this_dim1++;
					this_dim2 = 0;
				}
				{
					// Check: this_dim1 should equals to dim1
					if (!error_reported && dim1 != this_dim1)
					{
						reporter.add_error(E_ILLEGALARRAY, token0.get_start_line(), "array dimension mismatch" + to_string(__LINE__));
					}
				}
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_RBRACE)
				{
					error(T_RBRACE, type0, __LINE__);
				}
				{
					// Check: redefinition
					if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
						!search_for_variable(name_, current_scope).is_empty())
					{
						reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
					}
				}
				this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1, dim2));
				temp_root->set_type(G0_2DIMARRINIT);
				this->push_grammar_token("<变量定义及初始化>", "Variable Initialize");
			}
			/* branch 1.1.1.3 */else
			{
				// single 2-dimensional array definition without initialization
				this->give_back_token();
				{
					// Check: redefinition
					if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
						!search_for_variable(name_, current_scope).is_empty())
					{
						reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
					}
				}
				this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1, dim2));
				temp_root->add_son(new GNode(T_IDENFR, name_, temp_root));
				temp_root->add_son(new GNode(G_UNSIGNED, to_string(dim1), temp_root));
				temp_root->add_son(new GNode(G_UNSIGNED, to_string(dim2), temp_root));
				temp_root->set_type(G0_2DIMARRNOINIT);
				this->push_grammar_token("<变量定义无初始化>", "Variable No Initialize");
			}
		}
		/* branch 1.1.2 */else if (type0 == T_COMMA)
		{
			// multiple variables definition without initialization
			GNode* node1 = new GNode(G1_ARR1, temp_root);
			temp_root->add_son(node1);
			node1->add_son(new GNode(T_IDENFR, name_, node1));
			node1->add_son(new GNode(G_UNSIGNED, to_string(dim1), node1));
			this->give_back_token();
			{
				// Check: redefinition
				if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
					!search_for_variable(name_, current_scope).is_empty())
				{
					reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
				}
			}
			this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1));
			while (true)
			{
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_COMMA)
				{
					this->give_back_token();
					break;
				}
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_IDENFR)
				{
					error(T_IDENFR, type0, __LINE__);
				}
				name_ = token0.get_content();
				ln_ = token0.get_start_line();
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_LBRACK)
				{
					node1 = new GNode(G1_VAR, temp_root);
					temp_root->add_son(node1);
					node1->add_son(new GNode(T_IDENFR, name_, node1));
					this->give_back_token();
					{
						// Check: redefinition
						if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
							!search_for_variable(name_, current_scope).is_empty())
						{
							reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
						}
					}
					this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false));
					continue;
				}
				dim1 = this->unsigned_int();
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_RBRACK)
				{
					error(T_RBRACK, type0, __LINE__);
					{
						// Exception: Missing right bracket when expected.
						// We simply give back the token and continue the analysis.
						reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
						this->give_back_token();
					}
				}
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_LBRACK)
				{
					node1 = new GNode(G1_ARR1, temp_root);
					temp_root->add_son(node1);
					node1->add_son(new GNode(T_IDENFR, name_, node1));
					node1->add_son(new GNode(G_UNSIGNED, to_string(dim1), node1));
					this->give_back_token();
					{
						// Check: redefinition
						if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
							!search_for_variable(name_, current_scope).is_empty())
						{
							reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
						}
					}
					this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1));
					continue;
				}
				int dim2 = this->unsigned_int();
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_RBRACK)
				{
					error(T_RBRACK, type0, __LINE__);
					{
						// Exception: Missing right bracket when expected.
						// We simply give back the token and continue the analysis.
						reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
						this->give_back_token();
					}
				}
				node1 = new GNode(G1_ARR2, temp_root);
				temp_root->add_son(node1);
				node1->add_son(new GNode(T_IDENFR, name_, node1));
				node1->add_son(new GNode(G_UNSIGNED, to_string(dim1), node1));
				node1->add_son(new GNode(G_UNSIGNED, to_string(dim2), node1));
				{
					// Check: redefinition
					if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
						!search_for_variable(name_, current_scope).is_empty())
					{
						reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
					}
				}
				this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1, dim2));
			}
			temp_root->set_type(G0_MULVARNOINIT);
			this->push_grammar_token("<变量定义无初始化>", "Variable No Initialize");
		}
		/* branch 1.1.3 */else if (type0 == T_ASSIGN)
		{
			// single array definition with initialization
			temp_root->add_son(new GNode(T_IDENFR, name_, temp_root));
			temp_root->add_son(new GNode(G_UNSIGNED, to_string(dim1), temp_root));
			int this_dim1 = 0;
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_LBRACE)
			{
				error(T_LBRACE, type0, __LINE__);
			}
			GNode* old_current = this->current;
			this->current = temp_root;
			string this_type = this->constance();
			this->current = old_current;
			{
				// Check: variable type matches
				if (to_lowercase(this_type) != to_lowercase(type_))
				{
					this->give_back_token();
					BaseToken token_ = this->fetch_token();
					reporter.add_error(E_ILLEGALCONST, token_.get_start_line(), "Variable type mismatches");
				}
			}
			this_dim1++;
			while (true)
			{
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_COMMA)
				{
					this->give_back_token();
					break;
				}
				old_current = this->current;
				this->current = temp_root;
				this_type = this->constance();
				this->current = old_current;
				{
					// Check: variable type matches
					if (to_lowercase(this_type) != to_lowercase(type_))
					{
						this->give_back_token();
						BaseToken token_ = this->fetch_token();
						reporter.add_error(E_ILLEGALCONST, token_.get_start_line(), "Variable type mismatches");
					}
				}
				this_dim1++;
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_RBRACE)
			{
				error(T_LBRACE, type0, __LINE__);
			}
			{
				// Check: this_dim1 should equals to dim1
				if (dim1 != this_dim1)
				{
					reporter.add_error(E_ILLEGALARRAY, token0.get_start_line(), "array dimension mismatch" + to_string(__LINE__));
				}
			}
			{
				// Check: redefinition
				if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
					!search_for_variable(name_, current_scope).is_empty())
				{
					reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
				}
			}
			this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1));
			temp_root->set_type(G0_1DIMARRINIT);
			this->push_grammar_token("<变量定义及初始化>", "Variable Initialize");
		}
		/* branch 1.1.4 */else
		{
			// single array definition without initialization
			temp_root->add_son(new GNode(T_IDENFR, name_, temp_root));
			temp_root->add_son(new GNode(G_UNSIGNED, to_string(dim1), temp_root));
			this->give_back_token();
			{
				// Check: redefinition
				if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
					!search_for_variable(name_, current_scope).is_empty())
				{
					reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
				}
			}
			this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1));
			temp_root->set_type(G0_1DIMARRNOINIT);
			this->push_grammar_token("<变量定义无初始化>", "Variable No Initialize");
		}
	}
	/* branch 1.2 */else if (type0 == T_COMMA)
	{
		// multiple variables definition without initialization
		GNode* node1 = new GNode(G1_VAR, temp_root);
		temp_root->add_son(node1);
		node1->add_son(new GNode(T_IDENFR, name_, node1));
		{
			// Check: redefinition
			if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
				!search_for_variable(name_, current_scope).is_empty())
			{
				reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
			}
		}
		this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false));
		this->give_back_token();
		while (true)
		{
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_COMMA)
			{
				this->give_back_token();
				break;
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_IDENFR)
			{
				error(T_IDENFR, type0, __LINE__);
			}
			name_ = token0.get_content();
			ln_ = token0.get_start_line();
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_LBRACK)
			{
				node1 = new GNode(G1_VAR, temp_root);
				temp_root->add_son(node1);
				node1->add_son(new GNode(T_IDENFR, name_, node1));
				this->give_back_token();
				{
					// Check: redefinition
					if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
						!search_for_variable(name_, current_scope).is_empty())
					{
						reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
					}
				}
				this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false));
				continue;
			}
			int dim1 = this->unsigned_int();
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_RBRACK)
			{
				error(T_RBRACK, type0, __LINE__);
				{
					// Exception: Missing right bracket when expected.
					// We simply give back the token and continue the analysis.
					reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
					this->give_back_token();
				}
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_LBRACK)
			{
				node1 = new GNode(G1_ARR1, temp_root);
				temp_root->add_son(node1);
				node1->add_son(new GNode(T_IDENFR, name_, node1));
				node1->add_son(new GNode(G_UNSIGNED, to_string(dim1), node1));
				this->give_back_token();
				{
					// Check: redefinition
					if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
						!search_for_variable(name_, current_scope).is_empty())
					{
						reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
					}
				}
				this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1));
				continue;
			}
			int dim2 = this->unsigned_int();
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_RBRACK)
			{
				error(T_RBRACK, type0, __LINE__);
				{
					// Exception: Missing right bracket when expected.
					// We simply give back the token and continue the analysis.
					reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
					this->give_back_token();
				}
			}
			node1 = new GNode(G1_ARR2, temp_root);
			temp_root->add_son(node1);
			node1->add_son(new GNode(T_IDENFR, name_, node1));
			node1->add_son(new GNode(G_UNSIGNED, to_string(dim1), node1));
			node1->add_son(new GNode(G_UNSIGNED, to_string(dim2), node1));
			{
				// Check: redefinition
				if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
					!search_for_variable(name_, current_scope).is_empty())
				{
					reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
				}
			}
			this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false, dim1, dim2));
		}
		temp_root->set_type(G0_MULVARNOINIT);
		this->push_grammar_token("<变量定义无初始化>", "Variable No Initialize");
	}
	/* branch 1.3 */else if (type0 == T_ASSIGN)
	{
		// single variable definition with initialization
		temp_root->add_son(new GNode(T_IDENFR, name_, temp_root));
		GNode* old_current = this->current;
		this->current = temp_root;
		string this_type = this->constance();
		this->current = old_current;
		{
			// Check: variable type matches
			if (to_lowercase(this_type) != to_lowercase(type_))
			{
				this->give_back_token();
				BaseToken token_ = this->fetch_token();
				reporter.add_error(E_ILLEGALCONST, token_.get_start_line(), "Variable type mismatches");
			}
		}
		{
			// Check: redefinition
			if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
				!search_for_variable(name_, current_scope).is_empty())
			{
				reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
			}
		}
		this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false));
		temp_root->set_type(G0_SINGVARINIT);
		this->push_grammar_token("<变量定义及初始化>", "Variable Initialize");
	}
	/* branch 1.4 */else
	{
	// single variable definition without initialization
	temp_root->add_son(new GNode(T_IDENFR, name_, temp_root));
	this->give_back_token();
	{
		// Check: redefinition
		if ((current_scope == GLOBAL_SCOPE && !search_for_function(name_).is_empty()) || \
			!search_for_variable(name_, current_scope).is_empty())
		{
			reporter.add_error(E_REDEFINITION, ln_, name_ + "  redefined" + to_string(__LINE__));
		}
	}
	this->variable_table.push_back(VariableSymbol(type_, name_, this->current_scope, false));
	temp_root->set_type(G0_SINGVARNOINIT);
	this->push_grammar_token("<变量定义无初始化>", "Variable No Initialize");
	}
	this->push_grammar_token("<变量定义>", "Variable Definition");
}

void Grammar::variable_definition_without_initialization()
{
	// This shouldn't be called.
	LOGLN("[CALL] Grammar::variable_definition_without_initialization");
	LOGLN("[ERROR] This shouldn't be called!");
}

void Grammar::variable_definition_with_initialization()
{
	// This shouldn't be called.
	LOGLN("[CALL] Grammar::variable_definition_with_initialization");
	LOGLN("[ERROR] This shouldn't be called!");
}

void Grammar::type()
{
	// This shouldn't be called, instead use it inline.
	LOGLN("[CALL] Grammar::type");
	LOGLN("[ERROR] This shouldn't be called!");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_INTTK && type0 != T_CHARTK)
	{
		error(string() + T_INTTK + " or " + T_CHARTK, type0, __LINE__);
	}
}

void Grammar::function_definition_with_return_value()
{
	LOGLN("[CALL] Grammar::function_definition_with_return_value");
	GNode* node = new GNode(G_DECHEADER, current);
	this->current->add_son(node);
	this->current = node;
	vector<string> ret_name = this->declaration_header();
	this->current = current->get_father();
	// redefinition check is finished in subprogramme `declaration_header`
	if (ret_name.size() != 2)
	{
		LOGLN("[ERROR] Vector at " << __LINE__ - 3 << " should have size of 2.");
	}
	string return_ = ret_name[0];
	string name_ = ret_name[1];

	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_LPARENT)
	{
		error(T_LPARENT, type0, __LINE__);
	}
	this->current_scope = name_;
	node = new GNode(G_ARGTABLE, current);
	this->current->add_son(node);
	this->current = node;
	vector<string> arg_types = this->arguments();
	this->current = current->get_father();
	this->function_table.push_back(FunctionSymbol(return_, name_, arg_types));
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_RPARENT)
	{
		error(T_RPARENT, type0, __LINE__);
		{
			// Exception: Missing right parentheses when expected.
			// We simply give back the token and continue the analysis.
			reporter.add_error(E_EXPECTPARENT, token0.get_start_line(), "Missing r-parent" + to_string(__LINE__));
			this->give_back_token();
		}
	}
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_LBRACE)
	{
		error(T_LBRACE, type0, __LINE__);
	}
	this->have_return_flag = false;
	node = new GNode(G_COMPOSITE, current);
	this->current->add_son(node);
	this->current = node;
	this->composite_statements();
	this->current = this->current->get_father();
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (!this->have_return_flag)
	{
		// Exception: Missing return statement
		reporter.add_error(E_ILLEGALRETURN, token0.get_start_line(), "Missing return statement" + to_string(__LINE__));
	}
	if (type0 != T_RBRACE)
	{
		error(T_RBRACE, type0, __LINE__);
	}
	this->current_scope = GLOBAL_SCOPE;
	this->push_grammar_token("<有返回值函数定义>", "<Function With Return>");
}

void Grammar::function_definition_without_return_value()
{
	LOGLN("[CALL] Grammar::function_definition_without_return_value");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_VOIDTK)
	{
		error(T_VOIDTK, type0, __LINE__);
	}
	string return_ = token0.get_content();
	BaseToken token1 = this->fetch_token();
	string type1 = token1.get_type();
	if (type1 != T_IDENFR)
	{
		error(T_IDENFR, type1, __LINE__);
	}
	string name_ = token1.get_content();
	this->current->add_son(new GNode(T_IDENFR, name_, current));
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_LPARENT)
	{
		error(T_LPARENT, type0, __LINE__);
	}
	this->current_scope = name_;
	GNode* node = new GNode(G_ARGTABLE, current);
	this->current->add_son(node);
	this->current = node;
	vector<string> arg_types = this->arguments();
	this->current = current->get_father();
	{
		// Check: redefinition
		if (!search_for_variable(name_, GLOBAL_SCOPE).is_empty() || !search_for_function(name_).is_empty())
		{
			reporter.add_error(E_REDEFINITION, token1.get_start_line(), string() + name_ + " redefined." + to_string(__LINE__));
		}
	}
	this->function_table.push_back(FunctionSymbol(return_, name_, arg_types));
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_RPARENT)
	{
		error(T_RPARENT, type0, __LINE__);
		{
			// Exception: Missing right parentheses when expected.
			// We simply give back the token and continue the analysis.
			reporter.add_error(E_EXPECTPARENT, token0.get_start_line(), "Missing r-parent" + to_string(__LINE__));
			this->give_back_token();
		}
	}
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_LBRACE)
	{
		error(T_LBRACE, type0, __LINE__);
	}
	this->current_scope = name_;
	node = new GNode(G_COMPOSITE, current);
	this->current->add_son(node);
	this->current = node;
	this->composite_statements();
	this->current = this->current->get_father();
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_RBRACE)
	{
		error(T_RBRACE, type0, __LINE__);
	}
	this->current_scope = GLOBAL_SCOPE;
	this->grammar_function_table.push_back(GrammarFunctionSymbol("void", token1.get_content()));
	this->push_grammar_token("<无返回值函数定义>", "<Function Without Return>");
}

void Grammar::composite_statements()
{
	LOGLN("[CALL] Grammar::composite_statements");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	this->give_back_token();
	if (type0 == T_CONSTTK)
	{
		GNode* node = new GNode(G_CONSTDEC, current);
		this->current->add_son(node);
		this->current = node;
		this->const_declaration();
		this->current = this->current->get_father();
	}
	token0 = this->fetch_token();
	type0 = token0.get_type();
	this->give_back_token();
	if (type0 == T_INTTK || type0 == T_CHARTK)
	{
		GNode* node = new GNode(G_VARDEC, current);
		this->current->add_son(node);
		this->current = node;
		this->variable_declaration();
		this->current = this->current->get_father();
	}
	GNode* node = new GNode(G_STATECROWD, current);
	this->current->add_son(node);
	this->current = node;
	this->statements_crowd();
	this->current = this->current->get_father();
	this->push_grammar_token("<复合语句>", "<Composite Statements>");
}

vector<string> Grammar::arguments()
{
	LOGLN("[CALL] Grammar::arguments");

	vector<string> types;

	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_INTTK && type != T_CHARTK)
	{
		this->give_back_token();
		this->push_grammar_token("<参数表>", "<Arguments>");
		return types;
	}
	string type_ = token.get_content();
	types.push_back(token.get_content());
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_IDENFR)
	{
		error(string() + T_IDENFR + " or " + T_CHARTK, type, __LINE__);
	}
	string name_ = token.get_content();
	this->variable_table.push_back(VariableSymbol(type_, name_, current_scope, false));
	this->current->add_son(new GNode(type_, current));
	this->current->add_son(new GNode(T_IDENFR, name_, current));
	while (true)
	{
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_COMMA)
		{
			this->give_back_token();
			break;
		}
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_INTTK && type != T_CHARTK)
		{
			error(string() + T_INTTK + " or " + T_CHARTK, type, __LINE__);
		}
		type_ = token.get_content();
		types.push_back(token.get_content());
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_IDENFR)
		{
			error(string() + T_IDENFR + " or " + T_CHARTK, type, __LINE__);
		}
		name_ = token.get_content();
		this->current->add_son(new GNode(type_, current));
		this->current->add_son(new GNode(T_IDENFR, name_, current));
		this->variable_table.push_back(VariableSymbol(type_, name_, current_scope, false));
	}
	this->push_grammar_token("<参数表>", "<Arguments>");
	return types;
}

void Grammar::main_function()
{
	LOGLN("[CALL] Grammar::main_function");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_VOIDTK)
	{
		error(T_VOIDTK, type, __LINE__);
	}
	string return_ = token.get_content();
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_MAINTK)
	{
		error(T_MAINTK, type, __LINE__);
	}
	string name_ = token.get_content();
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_LPARENT)
	{
		error(T_LPARENT, type, __LINE__);
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_RPARENT)
	{
		error(T_RPARENT, type, __LINE__);
		{
			// Exception: Missing right parentheses when expected.
			// We simply give back the token and continue the analysis.
			reporter.add_error(E_EXPECTPARENT, token.get_start_line(), "Missing r-parent" + to_string(__LINE__));
			this->give_back_token();
		}
	}
	token = this->fetch_token();
	type = token.get_type();
	vector<string> arg_types;
	this->function_table.push_back(FunctionSymbol(return_, name_, arg_types));
	if (type != T_LBRACE)
	{
		error(T_LBRACE, type, __LINE__);
	}
	this->current_scope = name_;
	GNode* node = new GNode(G_COMPOSITE, current);
	this->current->add_son(node);
	this->current = node;
	this->composite_statements();
	this->current = this->current->get_father();
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_RBRACE)
	{
		error(T_RBRACE, type, __LINE__);
	}
	this->current_scope = GLOBAL_SCOPE;
	this->push_grammar_token("<主函数>", "<Main Function>");
}

string Grammar::expression()
{
	LOGLN("[CALL] Grammar::expression");
	GNode* type_node = new GNode(G_RESERVERD, current);
	this->current->add_son(type_node);
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_PLUS && type != T_MINU)
	{
		this->give_back_token();
		this->current->add_son(new GNode(T_PLUS, current));
	}
	else
	{
		this->current->add_son(new GNode(type, current));
	}
	GNode* node = new GNode(G_ITEM, current);
	this->current->add_son(node);
	this->current = node;
	string type_ = this->item();
	this->current = this->current->get_father();
	while (true)
	{
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_PLUS && type != T_MINU)
		{
			this->give_back_token();
			break;
		}
		else
		{
			this->current->add_son(new GNode(type, current));
		}
		GNode* node = new GNode(G_ITEM, current);
		this->current->add_son(node);
		this->current = node;
		type_ = this->item();
		this->current = this->current->get_father();
		type_ = type_ == "undefined" ? "undefined" : "int";
	}
	this->push_grammar_token("<表达式>", "<Expression>");
	if (type_ == "char")
	{
		type_node->set_type(T_CHARTK);
	}
	else
	{
		type_node->set_type(T_INTTK);
	}
	return type_;
}

string Grammar::item()
{
	LOGLN("[CALL] Grammar::item");
	GNode* node = new GNode(G_FACTOR, current);
	this->current->add_son(node);
	this->current = node;
	string type_ = this->factor();
	this->current = this->current->get_father();
	while (true)
	{
		BaseToken token = this->fetch_token();
		string type = token.get_type();
		if (type != T_MULT && type != T_DIV)
		{
			this->give_back_token();
			break;
		}
		else
		{
			this->current->add_son(new GNode(type, current));
		}
		node = new GNode(G_FACTOR, current);
		this->current->add_son(node);
		this->current = node;
		type_ = this->factor();
		this->current = this->current->get_father();
		type_ = type_ == "undefined" ? "undefined" : "int";
	}
	this->push_grammar_token("<项>", "<Item>");
	return type_;
}

string Grammar::factor()
{
	LOGLN("[CALL] Grammar::factor");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	string type_ = "undefined";
	if (type0 == T_IDENFR)
	{
		string name_ = token0.get_content();
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 == T_LPARENT)
		{
			// <function>
			this->give_back_token();
			this->give_back_token();
			type_ = this->function_call();
		}
		else if (type0 == T_LBRACK)
		{
			// <identifier>[<expression>] & <identifier>[<expression>][<expression>]
			{
				// Check: undefined reference
				// Further we get the type of the identifier
				this->give_back_token();
				this->give_back_token();
				token0 = this->fetch_token();
				string name_ = token0.get_content();
				VariableSymbol vs_local = search_for_variable(name_, current_scope);
				VariableSymbol vs_global = search_for_variable(name_, GLOBAL_SCOPE);
				token0 = this->fetch_token();
				if (vs_local.is_empty() && vs_global.is_empty())
				{
					reporter.add_error(E_UNDEFINITION, token0.get_start_line(), name_ + " undefined" + to_string(__LINE__));
					type_ = "undefined";
				}
				else if (!vs_local.is_empty())
				{
					type_ = to_lowercase(vs_local.get_type());
				}
				else if (!vs_global.is_empty())
				{
					type_ = to_lowercase(vs_global.get_type());
				}
			}
			GNode* temp_root = new GNode(G_RESERVERD, current);
			this->current->add_son(temp_root);
			this->current = temp_root;
			temp_root->add_son(new GNode(T_IDENFR, name_, temp_root));
			GNode* node = new GNode(G_EXPRESSION, current);
			temp_root->add_son(node);
			this->current = node;
			string this_type = this->expression();
			this->current = this->current->get_father();
			{
				// Check: array index should be integer
				if (to_lowercase(this_type) != "int")
				{
					this->give_back_token();
					token0 = this->fetch_token();
					reporter.add_error(E_ILLEGALINDEX, token0.get_start_line(), "Non-integer array index");
				}
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_RBRACK)
			{
				error(T_RBRACK, type0, __LINE__);
				{
					// Exception: Missing right bracket when expected.
					// We simply give back the token and continue the analysis.
					reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
					this->give_back_token();
				}
			}
			token0 = this->fetch_token();
			type0 = token0.get_type();
			if (type0 != T_LBRACK)
			{
				// <identifier>[<expression>]
				temp_root->set_type(G1_ARR1);
				this->give_back_token();
			}
			else
			{
				// <identifier>[<expression>][<expression>]
				GNode* node = new GNode(G_EXPRESSION, temp_root);
				temp_root->add_son(node);
				this->current = node;
				this_type = this->expression();
				this->current = this->current->get_father();
				{
					// Check: array index should be integer
					if (to_lowercase(this_type) != "int")
					{
						this->give_back_token();
						token0 = this->fetch_token();
						reporter.add_error(E_ILLEGALINDEX, token0.get_start_line(), "Non-integer array index");
					}
				}
				token0 = this->fetch_token();
				type0 = token0.get_type();
				if (type0 != T_RBRACK)
				{
					error(T_RBRACK, type0, __LINE__);
					{
						// Exception: Missing right bracket when expected.
						// We simply give back the token and continue the analysis.
						reporter.add_error(E_EXPECTBRACK, token0.get_start_line(), "Missing r-brack" + to_string(__LINE__));
						this->give_back_token();
					}
				}
				temp_root->set_type(G1_ARR2);
			}
			this->current = temp_root->get_father();
		}
		else
		{
			// <identifier>
			GNode* temp_root = new GNode(G1_VAR, current);
			this->current->add_son(temp_root);
			temp_root->add_son(new GNode(T_IDENFR, name_, current));
			{
				// Check: undefined reference
				// Further we get the type of the identifier
				this->give_back_token();
				this->give_back_token();
				token0 = this->fetch_token();
				string name_ = token0.get_content();
				VariableSymbol vs_local = search_for_variable(name_, current_scope);
				VariableSymbol vs_global = search_for_variable(name_, GLOBAL_SCOPE);
				if (vs_local.is_empty() && vs_global.is_empty())
				{
					reporter.add_error(E_UNDEFINITION, token0.get_start_line(), name_ + " undefined" + to_string(__LINE__));
					type_ = "undefined";
				}
				else if (!vs_local.is_empty())
				{
					type_ = to_lowercase(vs_local.get_type());
				}
				else if (!vs_global.is_empty())
				{
					type_ = to_lowercase(vs_global.get_type());
				}
				token0 = this->fetch_token();
			}
			this->give_back_token();
		}
	}
	else if (type0 == T_INTCON || type0 == T_PLUS || type0 == T_MINU)
	{
		// <integer>
		this->give_back_token();
		GNode* node = new GNode(G_INT, current);
		this->current->add_son(node);
		this->current = node;
		this->int_();
		this->current = this->current->get_father();
		type_ = "int";
	}
	else if (type0 == T_CHARCON)
	{
		// <charactor>
		// nothing to do
		GNode* node = new GNode(G_CHAR, token0.get_content(), current);
		this->current->add_son(node);
		type_ = "char";
	}
	else if (type0 == T_LPARENT)
	{
		// (<expression>)
		GNode* node = new GNode(G_EXPRESSION, current);
		this->current->add_son(node);
		this->current = node;
		type_ = this->expression();
		this->current = current->get_father();
		type_ = type_ == "char" ? "int" : type_;
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_RPARENT)
		{
			error(T_RPARENT, type0, __LINE__);
			{
				// Exception: Missing right parentheses when expected.
				// We simply give back the token and continue the analysis.
				reporter.add_error(E_EXPECTPARENT, token0.get_start_line(), "Missing r-parent" + to_string(__LINE__));
				this->give_back_token();
			}
		}
	}
	else
	{
		error("FACTOR", type0, __LINE__);
	}
	this->push_grammar_token("<因子>", "<Factor>");
	return type_;
}

void Grammar::statement()
{
	LOGLN("[CALL] Grammar::statement");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	this->give_back_token();
	if (type == T_WHILETK || type == T_FORTK)
	{
		GNode* node = new GNode(G_LOOPSTATE, current);
		this->current->add_son(node);
		this->current = node;
		this->loop_statement();
		this->current = this->current->get_father();
	}
	else if (type == T_IFTK)
	{
		GNode* node = new GNode(G_CONDISTATE, current);
		this->current->add_son(node);
		this->current = node;
		this->condition_statement();
		this->current = this->current->get_father();
	}
	else if (type == T_IDENFR)
	{
		token = this->fetch_token();
		token = this->fetch_token();
		type = token.get_type();
		this->give_back_token();
		this->give_back_token();
		if (type == T_LPARENT)
		{
			this->function_call();
		}
		else
		{
			GNode* node = new GNode(G_ASSIGNMENT, current);
			this->current->add_son(node);
			this->current = node;
			this->assign_statement();
			this->current = this->current->get_father();
		}
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_SEMICN)
		{
			error(T_SEMICN, type, __LINE__);
			{
				// Exception: Missing semicon when expected.
				// We simply give back the token and continue the analysis.
				this->give_back_token();
				this->give_back_token();
				token = this->fetch_token();
				// Notice that the error should be reported at the token ahead of
				// the position where semicon is expected, so we give back twice,
				// and then we restore the context after reporting the error.
				reporter.add_error(E_EXPECTSEMICN, token.get_start_line(), "Missing semicon" + to_string(__LINE__));
				token = this->fetch_token();
				this->give_back_token();
			}
		}
	}
	else if (type == T_SCANFTK)
	{
		GNode* node = new GNode(G_SCANFSTATE, current);
		this->current->add_son(node);
		this->current = node;
		this->scanf_();
		this->current = this->current->get_father();
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_SEMICN)
		{
			error(T_SEMICN, type, __LINE__);
			{
				// Exception: Missing semicon when expected.
				// We simply give back the token and continue the analysis.
				this->give_back_token();
				this->give_back_token();
				token = this->fetch_token();
				// Notice that the error should be reported at the token ahead of
				// the position where semicon is expected, so we give back twice,
				// and then we restore the context after reporting the error.
				reporter.add_error(E_EXPECTSEMICN, token.get_start_line(), "Missing semicon" + to_string(__LINE__));
				token = this->fetch_token();
				this->give_back_token();
			}
		}
	}
	else if (type == T_PRINTFTK)
	{
		GNode* node = new GNode(G_PRINTFSTATE, current);
		this->current->add_son(node);
		this->current = node;
		this->printf_();
		this->current = this->current->get_father();
		token = this->fetch_token();
		//token = this->fetch_token();
		type = token.get_type();
		if (type != T_SEMICN)
		{
			error(T_SEMICN, type, __LINE__);
			{
				// Exception: Missing semicon when expected.
				// We simply give back the token and continue the analysis.
				this->give_back_token();
				this->give_back_token();
				token = this->fetch_token();
				// Notice that the error should be reported at the token ahead of
				// the position where semicon is expected, so we give back twice,
				// and then we restore the context after reporting the error.
				reporter.add_error(E_EXPECTSEMICN, token.get_start_line(), "Missing semicon" + to_string(__LINE__));
				token = this->fetch_token();
				this->give_back_token();
			}
		}
	}
	else if (type == T_SWITCHTK)
	{
		GNode* node = new GNode(G_SWITCHSTATE, current);
		this->current->add_son(node);
		this->current = node;
		this->case_statement();
		this->current = this->current->get_father();
	}
	else if (type == T_SEMICN)
	{
		this->fetch_token();
	}
	else if (type == T_RETURNTK)
	{
	GNode* node = new GNode(G_RETURNSTATE, current);
	this->current->add_son(node);
	this->current = node;
		this->return_();
		this->current = this->current->get_father();
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_SEMICN)
		{
			error(T_SEMICN, type, __LINE__);
			{
				// Exception: Missing semicon when expected.
				// We simply give back the token and continue the analysis.
				this->give_back_token();
				this->give_back_token();
				token = this->fetch_token();
				// Notice that the error should be reported at the token ahead of
				// the position where semicon is expected, so we give back twice,
				// and then we restore the context after reporting the error.
				reporter.add_error(E_EXPECTSEMICN, token.get_start_line(), "Missing semicon" + to_string(__LINE__));
				token = this->fetch_token();
				this->give_back_token();
			}
		}
	}
	else if (type == T_LBRACE)
	{
		this->fetch_token();
		this->statements_crowd();
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_RBRACE)
		{
			error(T_RBRACE, type, __LINE__);
		}
	}
	else
	{
		error("STATEMENT", type, __LINE__);
	}
	this->push_grammar_token("<语句>", "<Statement>");
}

void Grammar::assign_statement()
{
	LOGLN("[CALL] Grammar::assign_statement");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_IDENFR)
	{
		error(T_IDENFR, type, __LINE__);
	}
	GNode* temp_root = new GNode(G_RESERVERD, current);
	this->current->add_son(temp_root);
	this->current = temp_root;
	this->current->add_son(new GNode(T_IDENFR, token.get_content(), current));
	{
		// Check: undefined reference
		string name_ = token.get_content();
		VariableSymbol vs_local = search_for_variable(name_, current_scope);
		VariableSymbol vs_global = search_for_variable(name_, GLOBAL_SCOPE);
		if (vs_local.is_empty() && vs_global.is_empty())
		{
			reporter.add_error(E_UNDEFINITION, token.get_start_line(), name_ + " undefined" + to_string(__LINE__));
		}
		// Check: assign to const var
		else if (!vs_local.is_empty())
		{
			if (vs_local.is_const())
			{
				reporter.add_error(E_ASSIGNTOCONST, token.get_start_line(), "assign to const" + to_string(__LINE__));
			}
		}
		else if (!vs_global.is_empty())
		{
			if (vs_global.is_const())
			{
				reporter.add_error(E_ASSIGNTOCONST, token.get_start_line(), "assign to const" + to_string(__LINE__));
			}
		}
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type == T_ASSIGN)
	{
		temp_root->set_type(G1_VAR);
		this->current = temp_root->get_father();
		GNode* node = new GNode(G_EXPRESSION, current);
		current->add_son(node);
		current = node;
		this->expression();
		current = current->get_father();
	}
	else if (type == T_LBRACK)
	{
		GNode* node = new GNode(G_EXPRESSION, current);
		current->add_son(node);
		current = node;
		string this_type = this->expression();
		current = current->get_father();
		{
			// Check: array index should be integer
			if (to_lowercase(this_type) != "int")
			{
				this->give_back_token();
				token = this->fetch_token();
				reporter.add_error(E_ILLEGALINDEX, token.get_start_line(), "Non-integer array index");
			}
		}
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_RBRACK)
		{
			error(T_RBRACK, type, __LINE__);
			{
				// Exception: Missing right bracket when expected.
				// We simply give back the token and continue the analysis.
				reporter.add_error(E_EXPECTBRACK, token.get_start_line(), "Missing r-brack" + to_string(__LINE__));
				this->give_back_token();
			}
		}
		token = this->fetch_token();
		type = token.get_type();
		if (type == T_LBRACK)
		{
			node = new GNode(G_EXPRESSION, current);
			current->add_son(node);
			current = node;
			this_type = this->expression();
			current = current->get_father();
			temp_root->set_type(G1_ARR2);
			this->current = temp_root->get_father();
			{
				// Check: array index should be integer
				if (to_lowercase(this_type) != "int")
				{
					this->give_back_token();
					token = this->fetch_token();
					reporter.add_error(E_ILLEGALINDEX, token.get_start_line(), "Non-integer array index");
				}
			}
			token = this->fetch_token();
			type = token.get_type();
			if (type != T_RBRACK)
			{
				error(T_RBRACK, type, __LINE__);
				{
					// Exception: Missing right bracket when expected.
					// We simply give back the token and continue the analysis.
					reporter.add_error(E_EXPECTBRACK, token.get_start_line(), "Missing r-brack" + to_string(__LINE__));
					this->give_back_token();
				}
			}
			token = this->fetch_token();
			type = token.get_type();
			if (type != T_ASSIGN)
			{
				error(T_ASSIGN, type, __LINE__);
			}
			node = new GNode(G_EXPRESSION, current);
			current->add_son(node);
			current = node;
			this->expression();
			current = current->get_father();
		}
		else if (type == T_ASSIGN)
		{
			temp_root->set_type(G1_ARR1);
			current = temp_root->get_father();
			node = new GNode(G_EXPRESSION, current);
			current->add_son(node);
			current = node;
			this->expression();
			current = current->get_father();
		}
		else
		{
			error("LBRACK or ASSIGN", type, __LINE__);
		}
	}
	else
	{
		error(string() + T_ASSIGN + " or " T_LBRACK, type, __LINE__);
	}
	this->push_grammar_token("<赋值语句>", "<Assign Statement>");
}

void Grammar::condition_statement()
{
	LOGLN("[CALL] Grammar::condition_statement");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_IFTK)
	{
		error(T_IFTK, type, __LINE__);
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_LPARENT)
	{
		error(T_LPARENT, type, __LINE__);
	}
	GNode* node = new GNode(G_CONDITION, current);
	this->current->add_son(node);
	this->current = node;
	this->condition();
	this->current = this->current->get_father();
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_RPARENT)
	{
		error(T_RPARENT, type, __LINE__);
		{
			// Exception: Missing right parentheses when expected.
			// We simply give back the token and continue the analysis.
			reporter.add_error(E_EXPECTPARENT, token.get_start_line(), "Missing r-parent" + to_string(__LINE__));
			this->give_back_token();
		}
	}
	node = new GNode(G_STATE, current);
	this->current->add_son(node);
	this->current = node;
	this->statement();
	this->current = this->current->get_father();
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_ELSETK)
	{
		this->give_back_token();
	}
	else
	{
		node = new GNode(G_STATE, current);
		this->current->add_son(node);
		this->current = node;
		this->statement();
		this->current = this->current->get_father();
	}
	this->push_grammar_token("<条件语句>", "<Condition Statement>");
}

void Grammar::condition()
{
	LOGLN("[CALL] Grammar::condition");
	GNode* node = new GNode(G_EXPRESSION, current);
	this->current->add_son(node);
	this->current = node;
	string type1 = this->expression();
	this->current = this->current->get_father();
	{
		// Check: type of expressions in condition should be integer
		if (to_lowercase(type1) != "int")
		{
			this->give_back_token();
			BaseToken token = this->fetch_token();
			reporter.add_error(E_JUDGETYPEWRONG, token.get_start_line(), "Expression in condition isn't integer");
		}
	}
	node = new GNode(G_REL, current);
	this->current->add_son(node);
	this->current = node;
	string rel_type = this->rel_operator();
	this->current = this->current->get_father();
	node = new GNode(G_EXPRESSION, current);
	this->current->add_son(node);
	this->current = node;
	string type2 = this->expression();
	this->current = this->current->get_father();
	{
		// Check: type of expressions in condition should be integer
		if (to_lowercase(type2) != "int")
		{
			this->give_back_token();
			BaseToken token = this->fetch_token();
			reporter.add_error(E_JUDGETYPEWRONG, token.get_start_line(), "Expression in condition isn't integer");
		}
	}
	this->push_grammar_token("<条件>", "<Condition>");
}

void Grammar::loop_statement()
{
	LOGLN("[CALL] Grammar::loop_statement");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 == T_WHILETK)
	{
		this->current->add_son(new GNode(T_WHILETK, current));
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_LPARENT)
		{
			error(T_LPARENT, type0, __LINE__);
		}
		GNode* node = new GNode(G_CONDITION, current);
		this->current->add_son(node);
		this->current = node;
		this->condition();
		this->current = this->current->get_father();
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_RPARENT)
		{
			error(T_RPARENT, type0, __LINE__);
			{
				// Exception: Missing right parentheses when expected.
				// We simply give back the token and continue the analysis.
				reporter.add_error(E_EXPECTPARENT, token0.get_start_line(), "Missing r-parent" + to_string(__LINE__));
				this->give_back_token();
			}
		}
		node = new GNode(G_STATE, current);
		this->current->add_son(node);
		this->current = node;
		this->statement();
		this->current = this->current->get_father();
		this->push_grammar_token("<循环语句>", "<Loop Statement>");
	}
	else if (type0 == T_FORTK)
	{
		this->current->add_son(new GNode(T_FORTK, current));
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_LPARENT)
		{
			error(T_LPARENT, type0, __LINE__);
		}
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_IDENFR)
		{
			error(T_IDENFR, type0, __LINE__);
		}
		this->current->add_son(new GNode(T_IDENFR, token0.get_content(), current));
		{
			// Check: undefined reference
			string name_ = token0.get_content();
			VariableSymbol vs_local = search_for_variable(name_, current_scope);
			VariableSymbol vs_global = search_for_variable(name_, GLOBAL_SCOPE);
			if (vs_local.is_empty() && vs_global.is_empty())
			{
				reporter.add_error(E_UNDEFINITION, token0.get_start_line(), name_ + " undefined" + to_string(__LINE__));
			}
			// Check: assign to const var
			else if (!vs_local.is_empty())
			{
				if (vs_local.is_const())
				{
					reporter.add_error(E_ASSIGNTOCONST, token0.get_start_line(), "assign to const" + to_string(__LINE__));
				}
			}
			else if (!vs_global.is_empty())
			{
				if (vs_global.is_const())
				{
					reporter.add_error(E_ASSIGNTOCONST, token0.get_start_line(), "assign to const" + to_string(__LINE__));
				}
			}
		}
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_ASSIGN)
		{
			error(T_ASSIGN, type0, __LINE__);
		}
		GNode* node = new GNode(G_EXPRESSION, current);
		this->current->add_son(node);
		this->current = node;
		this->expression();
		this->current = this->current->get_father();
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_SEMICN)
		{
			error(T_SEMICN, type0, __LINE__);
			{
				// Exception: Missing semicon when expected.
				// We simply give back the token and continue the analysis.
				this->give_back_token();
				this->give_back_token();
				token0 = this->fetch_token();
				// Notice that the error should be reported at the token ahead of
				// the position where semicon is expected, so we give back twice,
				// and then we restore the context after reporting the error.
				reporter.add_error(E_EXPECTSEMICN, token0.get_start_line(), "Missing semicon" + to_string(__LINE__));
				token0 = this->fetch_token();
				this->give_back_token();
			}
		}
		node = new GNode(G_CONDITION, current);
		this->current->add_son(node);
		this->current = node;
		this->condition();
		this->current = this->current->get_father();
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_SEMICN)
		{
			error(T_SEMICN, type0, __LINE__);
			{
				// Exception: Missing semicon when expected.
				// We simply give back the token and continue the analysis.
				this->give_back_token();
				this->give_back_token();
				token0 = this->fetch_token();
				// Notice that the error should be reported at the token ahead of
				// the position where semicon is expected, so we give back twice,
				// and then we restore the context after reporting the error.
				reporter.add_error(E_EXPECTSEMICN, token0.get_start_line(), "Missing semicon" + to_string(__LINE__));
				token0 = this->fetch_token();
				this->give_back_token();
			}
		}
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_IDENFR)
		{
			error(T_IDENFR, type0, __LINE__);
		}
		this->current->add_son(new GNode(T_IDENFR, token0.get_content(), current));
		{
			// Check: undefined reference
			string name_ = token0.get_content();
			VariableSymbol vs_local = search_for_variable(name_, current_scope);
			VariableSymbol vs_global = search_for_variable(name_, GLOBAL_SCOPE);
			if (vs_local.is_empty() && vs_global.is_empty())
			{
				reporter.add_error(E_UNDEFINITION, token0.get_start_line(), name_ + " undefined" + to_string(__LINE__));
			}
			// Check: assign to const var
			else if (!vs_local.is_empty())
			{
				if (vs_local.is_const())
				{
					reporter.add_error(E_ASSIGNTOCONST, token0.get_start_line(), "assign to const" + to_string(__LINE__));
				}
			}
			else if (!vs_global.is_empty())
			{
				if (vs_global.is_const())
				{
					reporter.add_error(E_ASSIGNTOCONST, token0.get_start_line(), "assign to const" + to_string(__LINE__));
				}
			}
		}
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_ASSIGN)
		{
			error(T_ASSIGN, type0, __LINE__);
		}
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_IDENFR)
		{
			error(T_IDENFR, type0, __LINE__);
		}
		this->current->add_son(new GNode(T_IDENFR, token0.get_content(), current));
		{
			// Check: undefined reference
			string name_ = token0.get_content();
			if (search_for_variable(name_, current_scope).is_empty() && \
				search_for_variable(name_, GLOBAL_SCOPE).is_empty())
			{
				reporter.add_error(E_UNDEFINITION, token0.get_start_line(), name_ + " undefined" + to_string(__LINE__));
			}
		}
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_PLUS && type0 != T_MINU)
		{
			error("PLUS or MINU", type0, __LINE__);
		}
		this->current->add_son(new GNode(type0, current));
		this->stride();
		token0 = this->fetch_token();
		type0 = token0.get_type();
		if (type0 != T_RPARENT)
		{
			error(T_RPARENT, type0, __LINE__);
			{
				// Exception: Missing right parentheses when expected.
				// We simply give back the token and continue the analysis.
				reporter.add_error(E_EXPECTPARENT, token0.get_start_line(), "Missing r-parent" + to_string(__LINE__));
				this->give_back_token();
			}
		}
		node = new GNode(G_STATE, current);
		this->current->add_son(node);
		this->current = node;
		this->statement();
		this->current = this->current->get_father();
		this->push_grammar_token("<循环语句>", "<Loop Statement>");
	}
	else
	{
		error(string() + T_WHILETK + " or " + T_FORTK, type0, __LINE__);
	}
}

void Grammar::stride()
{
	LOGLN("[CALL] Grammar::stride");
	int value = this->unsigned_int();
	GNode* node = new GNode(G_UNSIGNED, to_string(value), current);
	this->current->add_son(node);
	this->current = node;
	this->current = this->current->get_father();
	this->push_grammar_token("<步长>", "<Stride>");
}

void Grammar::case_statement()
{
	LOGLN("[CALL] Grammar::case_statement");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_SWITCHTK)
	{
		error(T_SWITCHTK, type0, __LINE__);
	}
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_LPARENT)
	{
		error(T_LPARENT, type0, __LINE__);
	}
	GNode* node = new GNode(G_EXPRESSION, current);
	this->current->add_son(node);
	this->current = node;
	string this_type = this->expression();
	this->current = this->current->get_father();
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_RPARENT)
	{
		error(T_RPARENT, type0, __LINE__);
		{
			// Exception: Missing right parentheses when expected.
			// We simply give back the token and continue the analysis.
			reporter.add_error(E_EXPECTPARENT, token0.get_start_line(), "Missing r-parent" + to_string(__LINE__));
			this->give_back_token();
		}
	}
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_LBRACE)
	{
		error(T_LBRACE, type0, __LINE__);
	}
	node = new GNode(G_CASETABLE, current);
	this->current->add_son(node);
	this->current = node;
	this->case_table(this_type);
	this->current = this->current->get_father();
	node = new GNode(G_DEFAULT, current);
	this->current->add_son(node);
	this->current = node;
	this->default_();
	this->current = this->current->get_father();
	token0 = this->fetch_token();
	type0 = token0.get_type();
	if (type0 != T_RBRACE)
	{
		error(T_RBRACE, type0, __LINE__);
	}
	this->push_grammar_token("<情况语句>", "<Case Statement>");
}

void Grammar::case_table(string exp_type)
{
	LOGLN("[CALL] Grammar::case_table");
	this->case_substatement(exp_type);
	while (true)
	{
		BaseToken token = this->fetch_token();
		string type = token.get_type();
		this->give_back_token();
		if (type != T_CASETK)
		{
			break;
		}
		this->case_substatement(exp_type);
	}
	this->push_grammar_token("<情况表>", "<Case Table>");
}

void Grammar::case_substatement(string exp_type)
{
	LOGLN("[CALL] Grammar::case_substatement");
	BaseToken token = this->fetch_token();
	string type0 = token.get_type();
	if (type0 != T_CASETK)
	{
		error(T_CASETK, type0, __LINE__);
	}
	GNode* node = new GNode(G_CONST, current);
	this->current->add_son(node);
	this->current = node;
	string type_ = this->constance();
	this->current = this->current->get_father();
	{
		// Check: const type should match expression type
		if (to_lowercase(type_) != to_lowercase(exp_type))
		{
			this->give_back_token();
			BaseToken token_ = this->fetch_token();
			reporter.add_error(E_ILLEGALCONST, token.get_start_line(), "const type in case mismatches expression");
		}
	}
	token = this->fetch_token();
	type0 = token.get_type();
	if (type0 != T_COLON)
	{
		error(T_COLON, type0, __LINE__);
	}
	node = new GNode(G_STATE, current);
	this->current->add_son(node);
	this->current = node;
	this->statement();
	this->current = this->current->get_father();
	this->push_grammar_token("<情况子语句>", "<Case Substatement>");
}

void Grammar::default_()
{
	LOGLN("[CALL] Grammar::default_");
	BaseToken token = this->fetch_token();
	string type0 = token.get_type();
	if (type0 != T_DEFAULTTK)
	{
		error(T_DEFAULTTK, type0, __LINE__);
		{
			// Exception: Missing default statement
			// We assume that the default token and its afiliates
			// will be missing together, which means after this
			// the case statement should end immediately. Otherwise
			// a series of problems may be caused.
			// Further skip-read should be done if need.
			reporter.add_error(E_DEFAULTMISSING, token.get_start_line(), "Missing default" + to_string(__LINE__));
			this->give_back_token();
			return;
		}
	}
	token = this->fetch_token();
	type0 = token.get_type();
	if (type0 != T_COLON)
	{
		error(T_COLON, type0, __LINE__);
	}
	GNode* node = new GNode(G_STATE, current);
	this->current->add_son(node);
	this->current = node;
	this->statement();
	this->current = this->current->get_father();
	this->push_grammar_token("<缺省>", "<Default>");
}

string Grammar::function_call()
{
	LOGLN("[CALL] Grammar::function_call");
	BaseToken token0 = this->fetch_token();
	string type0 = token0.get_type();
	if (type0 != T_IDENFR)
	{
		error(T_IDENFR, type0, __LINE__);
	}
	string name_ = token0.get_content();
	string type_;
	FunctionSymbol fs;
	{
		// Check: undefined reference
		// Further, we get the return type of the function.
		fs = search_for_function(name_);
		if (fs.is_empty())
		{
			reporter.add_error(E_UNDEFINITION, token0.get_start_line(), name_ + " undefined." + to_string(__LINE__));
			type_ = "undefined";
		}
		else
		{
			type_ = to_lowercase(fs.get_return_type());
		}
	}
	if (type_ == "int" || type_ == "char")
	{
		GNode* node = new GNode(G_CALLNONVOID, current);
		this->current->add_son(node);
		this->current = node;
		this->current->add_son(new GNode(type_ == "int" ? T_INTTK : T_CHARTK, current));
		this->current->add_son(new GNode(T_IDENFR, name_, current));
	}
	else if (type_ == "void")
	{
		GNode* node = new GNode(G_CALLVOID, current);
		this->current->add_son(node);
		this->current = node;
		this->current->add_son(new GNode(T_VOIDTK, current));
		this->current->add_son(new GNode(T_IDENFR, name_, current));
	}
	BaseToken token1 = this->fetch_token();
	string type1 = token1.get_type();
	if (type1 != T_LPARENT)
	{
		error(T_LPARENT, type1, __LINE__);
	}
	GNode* node = new GNode(G_VALUEARGS, current);
	this->current->add_son(node);
	this->current = node;
	vector<string> arg_types = this->value_arguments();
	this->current = current->get_father();
	this->current = current->get_father();
	{
		// Check: arg number and types match the function
		if (!fs.is_empty())
		{
			this->give_back_token();
			token1 = this->fetch_token();
			vector<string> types = fs.get_arg_types();
			if (arg_types.size() != types.size())
			{
				reporter.add_error(E_ARGNUMMISMATCH, token1.get_start_line(), "Arg num mismatch" + to_string(__LINE__));
			}
			else
			{
				for (int i = 0; i < arg_types.size(); i++)
				{
					if (to_lowercase(arg_types[i]) != to_lowercase(types[i]))
					{
						reporter.add_error(E_ARGTYPEMISMATCH, token1.get_start_line(), "Arg type mismatch" + to_string(__LINE__));
						break;
					}
				}
			}
		}
	}
	token1 = this->fetch_token();
	type1 = token1.get_type();
	if (type1 != T_RPARENT)
	{
		error(T_RPARENT, type1, __LINE__);
		{
			// Exception: Missing right parentheses when expected.
			// We simply give back the token and continue the analysis.
			reporter.add_error(E_EXPECTPARENT, token1.get_start_line(), "Missing r-parent" + to_string(__LINE__));
			this->give_back_token();
		}
	}
	for (int i = 0; i < this->grammar_function_table.size(); i++)
	{
		if (token0.get_content() == this->grammar_function_table[i].get_name())
		{
			if (this->grammar_function_table[i].get_return() == "void")
			{
				this->push_grammar_token("<无返回值函数调用语句>", "<Func Call Return>");
				break;
			}
			else
			{
				this->push_grammar_token("<有返回值函数调用语句>", "<Func Call No Return>");
				break;
			}
		}
	}
	//LOGLN("[Error] Function " << token0.get_content() <<" undefined!");
	return type_;
}

void Grammar::function_call_with_return_value()
{
	// This function shouldn't be called.
	LOGLN("[CALL] Grammar::function_call_with_return_value");
	LOGLN("[ERROR] This shouldn't be called!");
}

void Grammar::function_call_without_return_value()
{
	// This function shouldn't be called.
	LOGLN("[CALL] Grammar::function_call_with_return_value");
	LOGLN("[ERROR] This shouldn't be called!");
}

vector<string> Grammar::value_arguments()
{
	LOGLN("[CALL] Grammar::value_arguments");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	this->give_back_token();
	vector<string> types;
	if (type == T_PLUS || type == T_MINU || type == T_IDENFR || \
		type == T_INTCON || type == T_CHARCON || type == T_LPARENT)
	{
		GNode* node = new GNode(G_EXPRESSION, current);
		this->current->add_son(node);
		this->current = node;
		string this_type = this->expression();
		this->current = current->get_father();
		types.push_back(this_type);
		while (true)
		{
			token = this->fetch_token();
			string type = token.get_type();
			if (type != T_COMMA)
			{
				this->give_back_token();
				break;
			}
			GNode* node = new GNode(G_EXPRESSION, current);
			this->current->add_son(node);
			this->current = node;
			this_type = this->expression();
			this->current = current->get_father();
			types.push_back(this_type);
		}
	}
	this->push_grammar_token("<值参数表>", "<Value arguments>");
	return types;
}

void Grammar::statements_crowd()
{
	LOGLN("[CALL] Grammar::statements_crowd");
	while (true)
	{
		BaseToken token = this->fetch_token();
		string type = token.get_type();
		this->give_back_token();
		if (type != T_WHILETK && type != T_FORTK && type != T_IFTK && \
			type != T_IDENFR && type != T_SCANFTK && type != T_PRINTFTK && \
			type != T_SWITCHTK && type != T_SEMICN && type != T_RETURNTK && \
			type != T_LBRACE)
		{
			break;
		}
		this->statement();
	}
	this->push_grammar_token("<语句列>", "<Statements Crowd>");
}

void Grammar::scanf_()
{
	LOGLN("[CALL] Grammar::scanf_");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_SCANFTK)
	{
		error(T_SCANFTK, type, __LINE__);
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_LPARENT)
	{
		error(T_LPARENT, type, __LINE__);
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_IDENFR)
	{
		error(T_IDENFR, type, __LINE__);
	}
	this->current->add_son(new GNode(T_IDENFR, token.get_content(), current));
	{
		// Check: undefined reference
		string name_ = token.get_content();
		VariableSymbol vs_local = search_for_variable(name_, current_scope);
		VariableSymbol vs_global = search_for_variable(name_, GLOBAL_SCOPE);
		if (vs_local.is_empty() && vs_global.is_empty())
		{
			reporter.add_error(E_UNDEFINITION, token.get_start_line(), name_ + " undefined" + to_string(__LINE__));
		}
		// Check: assign to const var
		else if (!vs_local.is_empty())
		{
			if (vs_local.is_const())
			{
				reporter.add_error(E_ASSIGNTOCONST, token.get_start_line(), "assign to const" + to_string(__LINE__));
			}
		}
		else if (!vs_global.is_empty())
		{
			if (vs_global.is_const())
			{
				reporter.add_error(E_ASSIGNTOCONST, token.get_start_line(), "assign to const" + to_string(__LINE__));
			}
		}
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_RPARENT)
	{
		error(T_RPARENT, type, __LINE__);
		{
			// Exception: Missing right parentheses when expected.
			// We simply give back the token and continue the analysis.
			reporter.add_error(E_EXPECTPARENT, token.get_start_line(), "Missing r-parent" + to_string(__LINE__));
			this->give_back_token();
		}
	}
	this->push_grammar_token("<读语句>", "<Scanf>");
}

void Grammar::printf_()
{
	LOGLN("[CALL] Grammar::printf_");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_PRINTFTK)
	{
		error(T_PRINTFTK, type, __LINE__);
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_LPARENT)
	{
		error(T_LPARENT, type, __LINE__);
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_STRCON)
	{
		// <expression>
		this->give_back_token();
		GNode* node = new GNode(G_EXPRESSION, current);
		this->current->add_son(node);
		this->current = node;
		this->expression();
		this->current = this->current->get_father();
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_RPARENT)
		{
			error(T_RPARENT, type, __LINE__);
			{
				// Exception: Missing right parentheses when expected.
				// We simply give back the token and continue the analysis.
				reporter.add_error(E_EXPECTPARENT, token.get_start_line(), "Missing r-parent" + to_string(__LINE__));
				this->give_back_token();
			}
		}
	}
	else
	{
		this->give_back_token();
		this->string_();
		token = this->fetch_token();
		type = token.get_type();
		if (type == T_COMMA)
		{
			// <string>,<expression>
			GNode* node = new GNode(G_EXPRESSION, current);
			this->current->add_son(node);
			this->current = node;
			this->expression();
			this->current = this->current->get_father();
			token = this->fetch_token();
			type = token.get_type();
			if (type != T_RPARENT)
			{
				error(T_RPARENT, type, __LINE__);
				{
					// Exception: Missing right parentheses when expected.
					// We simply give back the token and continue the analysis.
					reporter.add_error(E_EXPECTPARENT, token.get_start_line(), "Missing r-parent" + to_string(__LINE__));
					this->give_back_token();
				}
			}
		}
		else if (type == T_RPARENT)
		{
			// <string>
		}
		else
		{
			error(string() + T_COMMA + " or " + T_RPARENT, type, __LINE__);
			{
				// Exception: Missing right parentheses when expected.
				// We simply give back the token and continue the analysis.
				// Notice: I'm not quite sure whether I should report the error
				// here. It remains to be seen it works or not.
				reporter.add_error(E_EXPECTPARENT, token.get_start_line(), "Missing r-parent at printf" + to_string(__LINE__));
				this->give_back_token();
			}
		}
	}
	this->push_grammar_token("<写语句>", "<Print Statement>");
}

void Grammar::return_()
{
	LOGLN("[CALL] Grammar::return_");
	BaseToken token = this->fetch_token();
	string type = token.get_type();
	if (type != T_RETURNTK)
	{
		error(T_RETURNTK, type, __LINE__);
	}
	token = this->fetch_token();
	type = token.get_type();
	if (type != T_LPARENT)
	{
		// return;
		// ENSURES: current scope is name of a existing function
		FunctionSymbol fs = search_for_function(this->current_scope);
		string return_ = fs.get_return_type();
		if (to_lowercase(return_) != "void")
		{
			reporter.add_error(E_ILLEGALRETURN, token.get_start_line(), "`return;` in non-void function" + to_string(__LINE__));
		}
		this->give_back_token();
	}
	else
	{
		// return (<expression>);
		// ENSURES: current scope is name of a existing function
		FunctionSymbol fs = search_for_function(this->current_scope);
		string return_ = fs.get_return_type();
		if (to_lowercase(return_) == "void")
		{
			// Exception: `void` function with non-empty return statement.
			reporter.add_error(E_VOIDFUNCRETURNS, token.get_start_line(), "Non-empty return statement" + to_string(__LINE__));
		}
		GNode* node = new GNode(G_EXPRESSION, current);
		this->current->add_son(node);
		this->current = node;
		string type_ = this->expression();
		this->current = this->current->get_father();
		{
			// Check: whether return expression type matches function type
			if ((to_lowercase(return_) == "int" || to_lowercase(return_) == "char") && to_lowercase(type_) != to_lowercase(return_))
			{
				this->give_back_token();
				token = this->fetch_token();
				reporter.add_error(E_ILLEGALRETURN, token.get_start_line(), "Illegal return type, expected " + \
					return_ + " got " + type_ + to_string(__LINE__));
			}
		}
		token = this->fetch_token();
		type = token.get_type();
		if (type != T_RPARENT)
		{
			error(T_RPARENT, type, __LINE__);
			{
				// Exception: Missing right parentheses when expected.
				// We simply give back the token and continue the analysis.
				reporter.add_error(E_EXPECTPARENT, token.get_start_line(), "Missing r-parent" + to_string(__LINE__));
				this->give_back_token();
			}
		}
	}
	this->have_return_flag = true;
	this->push_grammar_token("<返回语句>", "<Return>");
}
