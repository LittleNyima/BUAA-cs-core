#ifndef MERROR_H
#define MERROR_H

#include "merrno.h"
#include <vector>

using namespace std;

struct ErrorDescripter
{
	Errno merrno;
	int lino;
	string message;
};

typedef struct ErrorDescripter ErrorDescripter;

class ErrorReporter
{
private:
	vector<ErrorDescripter> errors;

public:
	void add_error(Errno, int, string);
	void add_error(Errno, int);
	void report_error();
	void print_errors();
	void add_errors(vector<ErrorDescripter>);
	vector<ErrorDescripter> get_errors();
};

#endif // !MERROR_H
