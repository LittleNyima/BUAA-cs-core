#include "lex.h"
#include <iostream>
#include <fstream>

Lex::Lex()
{
	this->print2file = false;
	this->print_log = false;
	this->status = S_INITIAL;
}

void Lex::debug_mode()
{
	this->print2file = true;
	this->print_log = true;
}

void Lex::submit_mode()
{
	this->print2file = true;
	this->print_log = false;
}

void Lex::parse(string source)
{
	string buffer = "";
	int ln = 1, col = -1;
	for (int i = 0; i < source.size(); i++)
	{
		char c = source[i];
		if (c == '\n')
		{
			col = -1;
			ln += 1;
		}
		else col += 1;
	LOOP_START:
		switch (this->status)
		{
		case S_INITIAL:
			if (is_blank(c)) continue;
			else if (is_letter(c)) 
			{
				buffer += c;
				this->status = S_WORD; 
			}
			else if (is_digit(c))
			{
				buffer += c;
				this->status = S_NUMBER;
			}
			else if (c == '"') this->status = S_DQUOT;
			else if (c == '\'') this->status = S_SQUOT;
			else if (is_operator(c))
			{
				buffer += c;
				this->status = S_OPERATR;
			}
			else
			{
				// Exception: Illegal character in program.
				// We just ignore it and continue the lexing progress.
				reporter.add_error(E_ILLEGALSYMBOL, ln, string() + "Illegal character " + c + to_string(__LINE__));
			}
			break;
		case S_WORD:
			if (is_letter(c) || is_digit(c)) buffer += c;
			else
			{
				this->add_token(buffer, ln, col);
				buffer = "";
				this->status = S_INITIAL;
				goto LOOP_START;
			}
			break;
		case S_NUMBER:
			if (is_digit(c)) buffer += c;
			else
			{
				this->add_token(buffer, ln, col);
				buffer = "";
				this->status = S_INITIAL; 
				goto LOOP_START;
			}
			break;
		case S_DQUOT:
			if (is_legal_ascii(c))
			{
				buffer += c;
				this->status = S_STRING;
			}
			else if (c == '"')
			{
				if (buffer == "")
				{
					// Exception: Empty string.
					// Report it and treat it as a normal string.
					reporter.add_error(E_ILLEGALSYMBOL, ln, string() + "Empty string" + to_string(__LINE__));
				}
				this->add_token(buffer, ln, col);
				buffer = "";
				this->status = S_INITIAL;
			}
			else
			{
				// Exception: Illegal character in string.
				// Ignore it and continue the process.
				reporter.add_error(E_ILLEGALSYMBOL, ln, string() + "Illegal char " + c + " in string" + to_string(__LINE__));
				this->status = S_STRING;
			}
			break;
		case S_SQUOT:
			if (is_character(c))
			{
				buffer += c;
				this->status = S_CHARCON;
			}
			else
			{
				// Exception: Illegal character in charcon.
				// Report it and treat it as a normal char.
				reporter.add_error(E_ILLEGALSYMBOL, ln, string() + "Illegal char " + c + " in char constance" + to_string(__LINE__));
				buffer += c;
				this->status = S_CHARCON;
			}
			break;
		case S_CHARCON:
			if (c == '\'')
			{
				this->add_token(buffer, ln, col);
				buffer = "";
				this->status = S_INITIAL;
			}
			else
			{
				// Exception: Charcon didn't terminate properly.
				// Report it until find the next `'`.
				reporter.add_error(E_ILLEGALSYMBOL, ln, string() + "Charcon didn't terminate properly" + to_string(__LINE__));
			}
			break;
		case S_STRING:
			if (is_legal_ascii(c)) buffer += c;
			else if (c == '"')
			{
				this->add_token(buffer, ln, col);
				buffer = "";
				this->status = S_INITIAL;
			}
			else
			{
				// Exception: Illegal character in string.
				// Ignore it and continue the process.
				reporter.add_error(E_ILLEGALSYMBOL, ln, string() + "Illegal char " + c + " in string" + to_string(__LINE__));
			}
			break;
		case S_OPERATR:
			if ((buffer == "<" || buffer == ">" || buffer == "!" || buffer == "=") && c == '=')
				buffer += c;
			else if (is_operator(c))
			{
				this->add_token(buffer, ln, col);
				buffer = string(1, c);
			}
			else
			{
				this->add_token(buffer, ln, col);
				buffer = "";
				this->status = S_INITIAL;
				goto LOOP_START;
			}
			break;
		default:
			break; // ENSURES: NEVER HAPPEN
		}
		if (this->print_log)
		{
			cout << "[LEX] CHAR: " << c << ", STATUS: " << this->status;
			cout << ", Ln: " << ln + 1 << ", Col: " << col + 1 << endl;
		}
	}
	if (this->status != S_INITIAL)
	{
		this->add_token(buffer, ln, col);
		this->status = S_INITIAL;
	}
	this->after_parse();
}

void Lex::add_token(string s, int line, int col)
{
	if (this->print_log)
	{
		cout << "[LEX] ADD TOKEN: " << s << ", STATUS: " << this->status << endl;
	}
	if (this->status == S_WORD && !this->add_keyword(s, line, col))
		this->tokens.push_back(BaseToken("IDENFR", s, line, col));
	else if (this->status == S_NUMBER)
		this->tokens.push_back(IntConstToken(s, line, col));
	else if (this->status == S_DQUOT || this->status == S_STRING)
		this->tokens.push_back(BaseToken("STRCON", s, line, col));
	else if (this->status == S_CHARCON)
		this->tokens.push_back(CharConstToken(s, line, col));
	else if (this->status == S_OPERATR)
		this->add_operator(s, line, col);
	else;// ENSURES: NEVER HAPPEN
}

bool Lex::add_keyword(string s, int line, int col)
{
	string _s = to_lowercase(s);
	string token = "NONE";
	if (_s == "const") token = "CONSTTK";
	else if (_s == "int") token = "INTTK";
	else if (_s == "char") token = "CHARTK";
	else if (_s == "void") token = "VOIDTK";
	else if (_s == "main") token = "MAINTK";
	else if (_s == "if") token = "IFTK";
	else if (_s == "else") token = "ELSETK";
	else if (_s == "switch") token = "SWITCHTK";
	else if (_s == "case") token = "CASETK";
	else if (_s == "default") token = "DEFAULTTK";
	else if (_s == "while") token = "WHILETK";
	else if (_s == "for") token = "FORTK";
	else if (_s == "scanf") token = "SCANFTK";
	else if (_s == "printf") token = "PRINTFTK";
	else if (_s == "return") token = "RETURNTK";
	else return false;
	this->tokens.push_back(BaseToken(token, s, line, col));
	return true;
}

void Lex::add_operator(string s, int line, int col)
{
	string token = "NONE";
	if (s == "+") token = "PLUS";
	else if (s == "-") token = "MINU";
	else if (s == "*") token = "MULT";
	else if (s == "/") token = "DIV";
	else if (s == "<") token = "LSS";
	else if (s == "<=") token = "LEQ";
	else if (s == ">") token = "GRE";
	else if (s == ">=") token = "GEQ";
	else if (s == "==") token = "EQL";
	else if (s == "!=") token = "NEQ";
	else if (s == ":") token = "COLON";
	else if (s == "=") token = "ASSIGN";
	else if (s == ";") token = "SEMICN";
	else if (s == ",") token = "COMMA";
	else if (s == "(") token = "LPARENT";
	else if (s == ")") token = "RPARENT";
	else if (s == "[") token = "LBRACK";
	else if (s == "]") token = "RBRACK";
	else if (s == "{") token = "LBRACE";
	else if (s == "}") token = "RBRACE";
	else return; // ENSURES: NEVER HAPPEN
	this->tokens.push_back(BaseToken(token, s, line, col));
}

void Lex::after_parse()
{
	if (this->print2file)
	{
		ofstream fout("output.txt");
		if (fout.is_open())
		{
			for (int i = 0; i < this->tokens.size(); i++)
			{
				fout << this->tokens[i].to_string() << endl;
			}
			fout.close();
		}
	}
	if (this->print_log)
	{
		cout << "[AFTER] Tokens:" << endl;
		for (int i = 0; i < this->tokens.size(); i++)
		{
			cout << "[AFTER] " << this->tokens[i].to_string();
			cout << " at ln: " << this->tokens[i].get_start_line();
			cout << ", col: " << this->tokens[i].get_start_col() << endl;
		}
		cout << "[AFTER] Lexing finished." << endl;
	}
}

vector<BaseToken> Lex::get_tokens()
{
	return this->tokens;
}

vector<ErrorDescripter> Lex::get_errors()
{
	return this->reporter.get_errors();
}
