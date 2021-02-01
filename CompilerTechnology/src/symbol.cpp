#include "symbol.h"

GrammarFunctionSymbol::GrammarFunctionSymbol(string returns, string name)
{
	this->name = name;
	this->returns = returns;
}

string GrammarFunctionSymbol::get_name()
{
	return this->name;
}

string GrammarFunctionSymbol::get_return()
{
	return this->returns;
}

FunctionSymbol::FunctionSymbol()
{
	this->name = "";
	this->return_type = "";
	this->_is_empty = true;
}

FunctionSymbol::FunctionSymbol(string return_type, string name, vector<string> arg_types)
{
	this->name = name;
	this->return_type = return_type;
	this->arg_types = arg_types;
	this->_is_empty = false;
}

string FunctionSymbol::get_name()
{
	return this->name;
}

string FunctionSymbol::get_return_type()
{
	return this->return_type;
}

vector<string> FunctionSymbol::get_arg_types()
{
	return this->arg_types;
}

string FunctionSymbol::to_string()
{
	string ret = return_type + " " + name + "(";
	for (int i = 0; i < arg_types.size(); i++)
	{
		ret += i == 0 ? arg_types[i] : "," + arg_types[i];
	}
	return ret + ");";
}

bool FunctionSymbol::is_empty()
{
	return _is_empty;
}

VariableSymbol::VariableSymbol()
{
	this->name = "";
	this->type = "";
	this->scope = "";
	this->_is_const = true;
	this->_is_empty = true;
}

VariableSymbol::VariableSymbol(string type, string name, string scope, bool is_const)
{
	this->name = name;
	this->type = type;
	this->scope = scope;
	this->_is_const = is_const;
	this->_is_empty = false;
}

VariableSymbol::VariableSymbol(string type, string name, string scope, bool is_const, int dim1)
	:VariableSymbol(type, name, scope, is_const)
{
	this->dims.push_back(dim1);
}

VariableSymbol::VariableSymbol(string type, string name, string scope, bool is_const, int dim1, int dim2)
	:VariableSymbol(type, name, scope, is_const)
{
	this->dims.push_back(dim1);
	this->dims.push_back(dim2);
}

string VariableSymbol::get_name()
{
	return this->name;
}

string VariableSymbol::get_type()
{
	return this->type;
}

string VariableSymbol::get_scope()
{
	return this->scope;
}

int VariableSymbol::get_dimension()
{
	return this->dims.size();
}

vector<int> VariableSymbol::get_dims()
{
	return this->dims;
}

bool VariableSymbol::is_const()
{
	return this->_is_const;
}

bool VariableSymbol::is_empty()
{
	return this->_is_empty;
}

string VariableSymbol::to_string()
{
	string ret = (_is_const ? string() + "const " : string()) + type + " " + name;
	for (int i = 0; i < dims.size(); i++)
	{
		ret = ret + "[" + std::to_string(dims[i]) + "]";
	}
	return ret + ": " + this->scope + ";";
}
