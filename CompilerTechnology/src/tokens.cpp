#include "tokens.h"

BaseToken::BaseToken()
{
	this->type = "NONE";
	this->content = "";
	this->startLine = 0;
	this->startCol = 0;
}

BaseToken::BaseToken(string type, string content, int startLine, int startCol)
{
	this->type = type;
	this->content = content;
	this->startLine = startLine;
	this->startCol = startCol;
}

string BaseToken::to_string()
{
	return this->type + " " + this->content;
}

bool BaseToken::is_empty()
{
	return this->type == "NONE";
}

string BaseToken::get_type()
{
	return this->type;
}

string BaseToken::get_content()
{
	return this->content;
}

int BaseToken::get_start_line()
{
	return this->startLine;
}

int BaseToken::get_start_col()
{
	return this->startCol;
}

IntConstToken::IntConstToken(): BaseToken()
{
	this->type = "INTCON";
	this->value = 0;
}

IntConstToken::IntConstToken(string content, int startLine, int startCol)
	: BaseToken("INTCON", content, startLine, startCol)
{
	this->type = "INTCON";
	this->value = stoi(content);
	// TODO: IGNORE OVERFLOW
}

int IntConstToken::get_value()
{
	return this->value;
}

CharConstToken::CharConstToken() : BaseToken()
{
	this->type = "CHARCON";
	this->value = '\0';
}

CharConstToken::CharConstToken(string content, int startLine, int startCol)
	: BaseToken("CHARCON", content, startLine, startCol)
{
	this->type = "CHARCON";
	// TODO: CHECK CONTENT.SIZE
	this->value = content[0];
}

char CharConstToken::get_value()
{
	return this->value;
}
