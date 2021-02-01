#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <vector>
#include <stack>
#include "tokens.h"
#include "symbol.h"
#include "merror.h"
#include "gtree.h"

#define GLOBAL_SCOPE "@global"

class Grammar
{
private:
	int offset;
	int length;
	bool print2file;
	bool print_log;
	bool print_err2file;
	vector<BaseToken> tokens;
	stack<string> foutput;
	vector<string> grammar_tokens;
	vector<GrammarFunctionSymbol> grammar_function_table;

	vector<FunctionSymbol> function_table;
	vector<VariableSymbol> variable_table;
	FunctionSymbol search_for_function(string);
	VariableSymbol search_for_variable(string, string);
	string current_scope;
	bool have_return_flag;

	ErrorReporter reporter;

	void error();
	void error(string, string, int);

	void after_parse();
	BaseToken fetch_token();
	void give_back_token();
	void push_grammar_token(string, string);
	void print_symbol_table();

	GNode* root;
	GNode* current;

	void plus_operator();
	void mult_operator();
	string rel_operator(); // Returns: type of rel opr
	void letter();
	void digit();
	void character();
	void string_();
	void program();
	void const_declaration();
	void const_definition();
	int unsigned_int();
	int int_();
	void identifier();
	vector<string> declaration_header(); // Returns: function return type & name
	string constance(); // Returns: type of the constance
	void variable_declaration();
	void variable_definition();
	void variable_definition_without_initialization();
	void variable_definition_with_initialization();
	void type();
	void function_definition_with_return_value();
	void function_definition_without_return_value();
	void composite_statements();
	vector<string> arguments(); // Returns: types of arguments
	void main_function();
	string expression(); // Returns: type of expression
	string item(); // Returns: type of item
	string factor(); // Returns: type of factor
	void statement();
	void assign_statement();
	void condition_statement();
	void condition();
	void loop_statement();
	void stride();
	void case_statement();
	void case_table(string);
	void case_substatement(string);
	void default_();
	string function_call(); // Returns: return type
	void function_call_with_return_value();
	void function_call_without_return_value();
	vector<string> value_arguments(); // Returns: types of the value args
	void statements_crowd();
	void scanf_();
	void printf_();
	void return_();

public:
	Grammar();
	void parse(vector<BaseToken>);
	void debug_mode();
	void submit_mode(string);
	vector<ErrorDescripter> get_errors();
	GTree build_tree();
};

#endif // !GRAMMAR_H
