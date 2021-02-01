#include "gtree.h"

Func::Func(string ret, string name, vector<string> arg_type, vector<P> args)
{
	this->ret = ret;
	this->name = name;
	this->arg_type = arg_type;
	this->args = args;
}

string Func::get_ret()
{
	return this->ret;
}

string Func::get_name()
{
	return this->name;
}

vector<string> Func::get_arg_type()
{
	return this->arg_type;
}

vector<P> Func::get_args()
{
	return this->args;
}