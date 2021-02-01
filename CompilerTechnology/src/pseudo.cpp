#include "pseudo.h"
#include "util.h"

Pseudo::Pseudo(string type, pair<string, string> opr1, pair<string, string> opr2, string r)
{
	this->type = type;
	this->opr1 = opr1;
	this->opr2 = opr2;
	this->result = r;
	if (opr1.second == P_VINT || opr1.second == P_VCHAR)
		this->use_names.push_back(to_lowercase(opr1.first));
	if (opr2.second == P_VINT || opr2.second == P_VCHAR)
		this->use_names.push_back(to_lowercase(opr2.first));
	this->update_name = r;
}

Pseudo::Pseudo(string type, string name, pair<string, string> ref, string r)
{
	this->type = type;
	this->name = name;
	this->ref = ref;
	this->result = r;
	this->use_names.push_back(to_lowercase(name));
	if (ref.second == P_VINT || ref.second == P_VCHAR)
		this->use_names.push_back(to_lowercase(ref.first));
	this->update_name = r;
}

Pseudo::Pseudo(string type, pair<string, string> header, vector<pair<string, string>> args)
{
	this->type = type;
	this->header = header;
	this->args = args;
	this->update_name = "";
}

Pseudo::Pseudo(string type, pair<string, string> opr1, string rel, pair<string, string> opr2)
{
	this->type = type;
	this->opr1 = opr1;
	this->opr2 = opr2;
	this->rel = rel;
	this->result = rel;
	this->name = rel;
	this->offset = opr2;
	if (opr1.second == P_VINT || opr1.second == P_VCHAR)
		this->use_names.push_back(to_lowercase(opr1.first));
	if (opr2.second == P_VINT || opr2.second == P_VCHAR)
		this->use_names.push_back(to_lowercase(opr2.first));
	if (type == P_LOOP || type == P_IF)
		this->update_name = "";
	else this->update_name = name;
}

Pseudo::Pseudo(string type, pair<string, string> p)
{
	this->type = type;
	this->ret = p;
	this->io = p;
	if (type == P_RET)
	{
		this->use_names.push_back(to_lowercase(p.first));
		this->update_name = "";
	}
	else
	{
		this->update_name = p.first;
	}
}

Pseudo::Pseudo(string type, string name, vector<pair<string, string>> args, string r)
{
	this->type = type;
	this->name = name;
	this->args = args;
	this->result = r;
	for (int i = 0; i < args.size(); i++)
		if (args[i].second == P_VINT || args[i].second == P_VCHAR)
			this->use_names.push_back(to_lowercase(args[i].first));
	this->update_name = r;
}

Pseudo::Pseudo(string type, string name, vector<pair<string, string>> args)
{
	this->type = type;
	this->name = name;
	this->args = args;
	for (int i = 0; i < args.size(); i++)
		if (args[i].second == P_VINT || args[i].second == P_VCHAR)
			this->use_names.push_back(to_lowercase(args[i].first));
	this->update_name = "";
}

Pseudo::Pseudo(string type, pair<string, string> p, string s)
{
	this->type = type;
	this->io = p;
	this->opr1 = p;
	this->endl = s;
	this->result = s;
	if (type == P_DISP)
	{
		if (p.second == P_VINT || p.second == P_VCHAR)
			this->use_names.push_back(to_lowercase(p.first));
		this->update_name = "";
	}
	else
	{
		if (p.second == P_VINT || p.second == P_VCHAR)
			this->use_names.push_back(to_lowercase(p.first));
		this->update_name = s;
	}
}

Pseudo::Pseudo(string type, string text)
{
	this->type = type;
	this->text = text;
}

Pseudo::Pseudo(string type)
{
	this->type = type;
}

Pseudo::Pseudo(Pseudo* another, string assign)
{
	this->type = another->type;
	this->opr1 = another->opr1;
	this->opr2 = another->opr2;
	this->result = assign;
	this->rel = another->rel;
	this->name = another->name;
	this->ref = another->ref;
	this->header = another->header;
	this->io = another->io;
	this->ret = another->ret;
	this->args = another->args;
	this->endl = another->endl;
	this->text = another->text;
	this->offset = another->offset;
	this->use_names = another->use_names;
	this->update_name = another->update_name;
}

string Pseudo::get_type()
{
	return this->type;
}

pair<string, string> Pseudo::get_opr1()
{
	return this->opr1;
}

pair<string, string> Pseudo::get_opr2()
{
	return this->opr2;
}

string Pseudo::get_result()
{
	return this->result;
}

string Pseudo::get_rel()
{
	return this->rel;
}

string Pseudo::get_name()
{
	return this->name;
}

pair<string, string> Pseudo::get_ref()
{
	return this->ref;
}

pair<string, string> Pseudo::get_header()
{
	return this->header;
}

pair<string, string> Pseudo::get_io()
{
	return this->io;
}

pair<string, string> Pseudo::get_ret()
{
	return this->ret;
}

vector<pair<string, string>> Pseudo::get_args()
{
	return this->args;
}

string Pseudo::get_endl()
{
	return endl;
}

pair<string, string> Pseudo::get_offset()
{
	return offset;
}

string Pseudo::to_string()
{
	if (type == P_ADD || type == P_SUB || type == P_MUL || type == P_DIV)
		return type + " " + result + " <" + opr1.first + ", " + opr1.second + \
		">, <" + opr2.first + ", " + opr2.second + ">";
	else if (type == P_ARR)
		return type + " " + result + " " + name + " <" + ref.first + ", " + ref.second + ">";
	else if (type == P_FUNC)
		return func_to_string();
	else if (type == P_LOOP || type == P_IF)
		return type + " <" + opr1.first + ", " + opr1.second + ">, " + rel + \
		", <" + opr2.first + ", " + opr2.second + ">";
	else if (type == P_RET || type == P_READ)
		return type + " <" + ret.first + ", " + ret.second + ">";
	else if (type == P_CALL)
		return call_to_string();
	else if (type == P_CALN)
		return caln_to_string();
	else if (type == P_DISP)
		return type + " <" + io.first + ", " + io.second + ">, " + endl;
	else if (type == P_COMN)
		return "# " + text;
	else if (type == P_ASSN)
		return type + " " + result + " <" + opr1.first + ", " + opr1.second + ">";
	else if (type == P_ASSR)
		return type + " <" + opr1.first + ", " + opr1.second + ">, " + result + \
		"[<" + offset.first + ", " + offset.second + ">]";
	else return this->type;
}

string Pseudo::func_to_string()
{
	string ret = type + " <" + header.first + ", " + header.second + ">(";
	for (int i = 0; i < args.size(); i++)
	{
		ret += "<" + args[i].first + ", " + args[i].second + ">";
		if (i < args.size() - 1) ret += ", ";
	}
	return ret + ")";
}

string Pseudo::call_to_string()
{
	string ret = type + " " + result + " " + name + "(";
	for (int i = 0; i < args.size(); i++)
	{
		ret += "<" + args[i].first + ", " + args[i].second + ">";
		if (i < args.size() - 1) ret += ", ";
	}
	return ret + ")";
}

string Pseudo::caln_to_string()
{
	string ret = type + " " + name + "(";
	for (int i = 0; i < args.size(); i++)
	{
		ret += "<" + args[i].first + ", " + args[i].second + ">";
		if (i < args.size() - 1) ret += ", ";
	}
	return ret + ")";
}

bool Pseudo::use(string name)
{
	for (int i = 0; i < use_names.size(); i++)
	{
		if (to_lowercase(name) == to_lowercase(use_names[i])) return true;
	}
	return false;
}

bool Pseudo::update(string name)
{
	return this->update_name != "" && \
		to_lowercase(name) == to_lowercase(update_name);
}

string Pseudo::get_update()
{
	return this->update_name;
}

vector<string> Pseudo::get_use()
{
	return this->use_names;
}