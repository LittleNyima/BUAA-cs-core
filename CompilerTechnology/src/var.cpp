#include "gtree.h"

Var::Var(Var* another)
{
	this->type = another->type;
	this->name = another->name;
	this->dim = another->dim;
	this->_ivalue = another->_ivalue;
	this->_cvalue = another->_cvalue;
	this->_has_value = another->_has_value;
	this->_is_const = another->_is_const;
}

Var::Var(string type, string name, int value, bool _is_const)
{
	this->type = type;
	this->name = name;
	this->_ivalue.push_back(value);
	this->_has_value = true;
	this->_is_const = _is_const;
}

Var::Var(string type, string name, char value, bool _is_const)
{
	this->type = type;
	this->name = name;
	this->_cvalue.push_back(value);
	this->_has_value = true;
	this->_is_const = _is_const;
}

Var::Var(string type, string name)
{
	this->type = type;
	this->name = name;
	this->_has_value = false;
	this->_is_const = false;
}

Var::Var(string type, string name, int dim1)
{
	this->type = type;
	this->name = name;
	this->dim.push_back(dim1);
	this->_has_value = false;
	this->_is_const = false;
}

Var::Var(string type, string name, int dim1, int dim2)
{
	this->type = type;
	this->name = name;
	this->dim.push_back(dim1);
	this->dim.push_back(dim2);
	this->_has_value = false;
	this->_is_const = false;
}

Var::Var(string type, string name, int dim1, vector<int> values)
{
	this->type = type;
	this->name = name;
	this->dim.push_back(dim1);
	this->_ivalue = values;
	this->_has_value = true;
	this->_is_const = false;
}

Var::Var(string type, string name, int dim1, int dim2, vector<int> values)
{
	this->type = type;
	this->name = name;
	this->dim.push_back(dim1);
	this->dim.push_back(dim2);
	this->_ivalue = values;
	this->_has_value = true;
	this->_is_const = false;
}

Var::Var(string type, string name, int dim1, vector<char> values)
{
	this->type = type;
	this->name = name;
	this->dim.push_back(dim1);
	this->_cvalue = values;
	this->_has_value = true;
	this->_is_const = false;
}

Var::Var(string type, string name, int dim1, int dim2, vector<char> values)
{
	this->type = type;
	this->name = name;
	this->dim.push_back(dim1);
	this->dim.push_back(dim2);
	this->_cvalue = values;
	this->_has_value = true;
	this->_is_const = false;
}

string Var::get_type()
{
	return this->type;
}

string Var::get_name()
{
	return this->name;
}

bool Var::has_value()
{
	return this->_has_value;
}

int Var::get_value()
{
	if (this->type == T_INTTK) return this->_ivalue[0];
	else if (this->type == T_CHARTK) return this->_cvalue[0];
	return -10492; // ENSURE: NEVER HAPPENS
}

int Var::get_value(int dim1)
{
	if (this->type == T_INTTK) return this->_ivalue[dim1];
	else if (this->type == T_CHARTK) return this->_cvalue[dim1];
	return -10492; // ENSURE: NEVER HAPPENS
}

int Var::get_value(int dim1, int dim2)
{
	int offset = dim1 * this->dim[1] + dim2;
	if (this->type == T_INTTK) return this->_ivalue[offset];
	else if (this->type == T_CHARTK) return this->_cvalue[offset];
	return -10492; // ENSURE: NEVER HAPPENS
}

bool Var::is_const()
{
	return this->_is_const;
}

vector<int> Var::get_dim()
{
	return this->dim;
}

string Var::to_string()
{
	string ret = this->type + " " + this->name;
	ret = this->_is_const ? "const " + ret : ret;
	for (int i = 0; i < this->dim.size(); i++)
		ret += "[" + std::to_string(this->dim[i]) + "]";
	ret += !this->_has_value ? " false" :
		this->type == T_INTTK ? " = " + std::to_string(_ivalue[0]) :
		" = '" + string() + _cvalue[0] + "'";
	ret += (this->dim.size() != 0 && this->_has_value) ? ", ..." : "";
	return ret;
}