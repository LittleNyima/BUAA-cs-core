#ifndef LEX_H
#define LEX_H

#include <vector>
#include "tokens.h"
#include "status.h"
#include "util.h"
#include "merror.h"

class Lex
{
private:
	bool print2file;
	bool print_log;
	vector<BaseToken> tokens;
	Status status;
	ErrorReporter reporter;

	void add_token(string, int, int);
	bool add_keyword(string, int, int);
	void add_operator(string, int, int);

	void after_parse();

public:
	Lex();
	void debug_mode();
	void submit_mode();
	void parse(string);
	vector<BaseToken> get_tokens();
	vector<ErrorDescripter> get_errors();
};

#endif // !LEX_H
