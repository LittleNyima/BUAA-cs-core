#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <vector>

using namespace std;

class GrammarFunctionSymbol
{
private:
	string name;
	string returns;
	vector<string> args;

public:
	GrammarFunctionSymbol(string, string);
	string get_name();
	string get_return();
};

class FunctionSymbol
{
private:
	string name;
	string return_type;
	vector<string> arg_types;
	bool _is_empty;

public:
	FunctionSymbol();
	FunctionSymbol(string, string, vector<string>);
	string get_name();
	string get_return_type();
	vector<string> get_arg_types();
	string to_string();
	bool is_empty();
};

class VariableSymbol
{
private:
	string name;
	string type;
	string scope;
	bool _is_const;
	bool _is_empty;
	vector<int> dims;

public:
	VariableSymbol();
	VariableSymbol(string, string, string, bool);
	VariableSymbol(string, string, string, bool, int);
	VariableSymbol(string, string, string, bool, int, int);
	string get_name();
	string get_type();
	string get_scope();
	int get_dimension();
	vector<int> get_dims();
	bool is_const();
	bool is_empty();
	string to_string();
};

#endif // !SYMBOL_H
