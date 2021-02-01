#include "gtree.h"
#include "util.h"
#include <queue>
#include <iostream>

GNode::GNode(GToken type)
	:GNode(type, "", nullptr)
{
}

GNode::GNode(GToken type, string content)
	:GNode(type, content, nullptr)
{
}

GNode::GNode(GToken type, GNode* father)
	:GNode(type, "", father)
{
}

GNode::GNode(GToken type, string content, GNode* father)
{
	this->type = type;
	this->content = content;
	this->father = father;
}

GToken GNode::get_type()
{
	return this->type;
}

string GNode::get_content()
{
	return this->content;
}

GNode* GNode::get_father()
{
	return this->father;
}

void GNode::set_type(GToken type)
{
	this->type = type;
}

void GNode::set_sons(vector<GNode*> sons)
{
	this->sons = sons;
}

void GNode::set_father(GNode* father)
{
	this->father = father;
}

void GNode::add_son(GNode* son)
{
	this->sons.push_back(son);
}

vector<GNode*> GNode::get_sons()
{
	return this->sons;
}

GNode* GNode::get_son(int index)
{
	return this->sons[index];
}
