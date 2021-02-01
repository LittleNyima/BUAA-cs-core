#include "util.h"

bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}

bool is_blank(char c)
{
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool is_letter(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_character(char c)
{
	return is_digit(c) || is_letter(c) || c == '+' || c == '-' || c == '*' || c == '/';
}

bool is_legal_ascii(char c)
{
	return c == 32 || c == 33 || (c >= 35 && c <= 126);
}

bool is_operator(char c)
{
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '>' || \
		c == '<' || c == '=' || c == '!' || c == ':' || c == ';' || \
		c == ',' || c == '(' || c == ')' || c == '[' || c == ']' || \
		c == '{' || c == '}';
}

bool starts_with(string s, string prefix)
{
	if (prefix.size() > s.size())
	{
		return false;
	}
	for (int i = 0; i < prefix.size(); i++)
	{
		if (s[i] != prefix[i]) return false;
	}
	return true;
}

bool contains(string s, char c)
{
	for (int i = 0; i < s.size(); i++)
	{
		if (s[i] == c)
		{
			return true;
		}
	}
	return false;
}

bool is_2_pow(int num)
{
	return num > 0 && (num & (num - 1)) == 0;
}

int log2_(int num)
{
	if (num <= 0) return -1;
	unsigned int mask[5] = { 0xffff0000, 0xff00ff00, 0xf0f0f0f0, 0xcccccccc, 0xaaaaaaaa };
	unsigned int log2 = 0;
	int num_ = num;
	for (int i = 0; i < 5; i++)
	{
		bool bitExist = (num_ & mask[i]) != 0;
		if (bitExist)
		{
			log2 |= (1 << (4 - i));
			num_ &= mask[i];
		}
	}
	return (int)log2;
}

string to_lowercase(string s)
{
	string _s = "";
	for (int i = 0; i < s.size(); i++)
	{
		_s += (s[i] >= 'A' && s[i] <= 'Z') ? s[i] - 'A' + 'a' : s[i];
	}
	return _s;
}

string slice(string s, int start_index, int end_index)
{
	if (start_index < 0) start_index += s.size();
	if (end_index < 0) end_index += s.size();
	string _s = "";
	for (int i = start_index; i < end_index; i++)
	{
		_s += s[i];
	}
	return _s;
}

string slice(string s, int start_index)
{
	return slice(s, start_index, s.length());
}
