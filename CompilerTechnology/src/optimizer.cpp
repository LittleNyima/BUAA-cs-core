#include "optimize.h"
#include "assembly.h"
#include "util.h"

#define ist(n) starts_with(n, G_TEMPPREFIX)

vector<Pseudo> Optimizer::modify_assign(vector<Pseudo> source)
{
	vector<Pseudo> new_codes;
	for (int i = 0; i < source.size() - 1; i++)
	{
		if (ist(source[i].get_update()) && \
			source[i + 1].get_type() == P_ASSN && \
			source[i].update(source[i + 1].get_opr1().first))
		{
			new_codes.push_back(Pseudo(&source[i], source[i + 1].get_result()));
			i++;
		}
		else
		{
			new_codes.push_back(source[i]);
		}
	}
	new_codes.push_back(source[source.size() - 1]);
	return new_codes;
}

struct ModifyNode
{
	string name;
	vector<struct ModifyNode*> sons;
};

typedef struct ModifyNode MN;
MN* found_node = nullptr;

void search_name(MN* node, string name)
{
	if (node->name == name) found_node = node;
	else for (int i = 0; i < node->sons.size(); i++)
		search_name(node->sons[i], name);
}

vector<Pseudo> Optimizer::loop_const(vector<Pseudo> source)
{
	vector<Pseudo> new_codes;
	for (int i = 0; i < source.size(); i++)
	{
		if (source[i].get_type() != P_LOOP) new_codes.push_back(source[i]);
		else
		{
			vector<Pseudo> consts;
			vector<Pseudo> nonconsts;
			vector<string> modified;
			int end = find_paired_loop(&source, i);
			MN root;
			root.name = "";
			root.sons = vector<MN*>();
			for (int j = i + 1; j < end; j++)
			{
				if (source[j].get_update() != "") 
					modified.push_back(source[j].get_update());
			}
			for (int j = end; j > i; j--)
			{
				string update = source[j].get_update();
				if (update != "")
				{
					found_node = nullptr;
					search_name(&root, to_lowercase(update));
					if (found_node == nullptr)
					{
						found_node = new MN();
						found_node->name = to_lowercase(update);
						found_node->sons = vector<MN*>();
						root.sons.push_back(found_node);
					}
					MN* modify_node = found_node;
					vector<string> uses = source[j].get_use();
					for (int k = 0; k < uses.size(); k++)
					{
						found_node = nullptr;
						search_name(&root, to_lowercase(uses[k]));
						if (found_node == nullptr)
						{
							found_node = new MN();
							found_node->name = to_lowercase(update);
							found_node->sons = vector<MN*>();
						}
						else
						{
							for (int l = 0; l < modify_node->sons.size(); l++)
							{
								if (modify_node->sons[l] == found_node)
									goto NOT_ADD;
							}
							modify_node->sons.push_back(found_node);
						NOT_ADD:;
						}
					}
				}
			}
			new_codes.push_back(source[i]);
			for (int j = 0; j < nonconsts.size(); j++)
				new_codes.push_back(nonconsts[j]);
			new_codes.push_back(source[end]);
			for (int j = 0; j < consts.size(); j++)
				new_codes.push_back(consts[j]);
			i = end;
		}
	}
	return new_codes;
}

int Optimizer::find_paired_loop(vector<Pseudo>* source, int offset)
{
	int depth = 0;
	for (int i = offset; i < source->size(); i++)
	{
		if ((*source)[i].get_type() == P_LOOP) depth++;
		else if ((*source)[i].get_type() == P_ENDLOOP) depth--;
		if (depth == 0) return i;
	}
	return -1;
}
