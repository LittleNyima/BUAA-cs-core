#ifndef TOKENS_H
#define TOKENS_H

#include <string>

using namespace std;

class BaseToken
{
public:
	BaseToken();
	BaseToken(string, string, int, int);
	string to_string();
	bool is_empty();
	string get_type();
	string get_content();
	int get_start_line();
	int get_start_col();

protected:
	string type;
	string content;
	int startLine;
	int startCol;
};

class IntConstToken
	: public BaseToken
{
public:
	IntConstToken();
	IntConstToken(string, int, int);
	int get_value();

private:
	int value;
};

class CharConstToken
	: public BaseToken
{
public:
	CharConstToken();
	CharConstToken(string, int, int);
	char get_value();

private:
	char value;
};

#define T_IDENFR    "IDENFR"
#define T_INTCON    "INTCON"
#define T_CHARCON   "CHARCON"
#define T_STRCON    "STRCON"
#define T_CONSTTK   "CONSTTK"
#define T_INTTK     "INTTK"
#define T_CHARTK    "CHARTK"
#define T_VOIDTK    "VOIDTK"
#define T_MAINTK    "MAINTK"
#define T_IFTK      "IFTK"
#define T_ELSETK    "ELSETK"
#define T_SWITCHTK  "SWITCHTK"
#define T_CASETK    "CASETK"
#define T_DEFAULTTK "DEFAULTTK"
#define T_WHILETK   "WHILETK"
#define T_FORTK     "FORTK"
#define T_SCANFTK   "SCANFTK"
#define T_PRINTFTK  "PRINTFTK"
#define T_RETURNTK  "RETURNTK"
#define T_PLUS      "PLUS"
#define T_MINU      "MINU"
#define T_MULT      "MULT"
#define T_DIV       "DIV"
#define T_LSS       "LSS"
#define T_LEQ       "LEQ"
#define T_GRE       "GRE"
#define T_GEQ       "GEQ"
#define T_EQL       "EQL"
#define T_NEQ       "NEQ"
#define T_COLON     "COLON"
#define T_ASSIGN    "ASSIGN"
#define T_SEMICN    "SEMICN"
#define T_COMMA     "COMMA"
#define T_LPARENT   "LPARENT"
#define T_RPARENT   "RPARENT"
#define T_LBRACK    "LBRACK"
#define T_RBRACK    "RBRACK"
#define T_LBRACE    "LBRACE"
#define T_RBRACE    "RBRACE"

#endif // !TOKENS_H
