#include "merror.h"
#include <fstream>
#include <iostream>

void ErrorReporter::add_error(Errno err, int line_num, string message)
{
	ErrorDescripter ed;
	ed.merrno = err;
	ed.lino = line_num;
	ed.message = message;
	this->errors.push_back(ed);
}

void ErrorReporter::add_error(Errno err, int line_num)
{
	this->add_error(err, line_num, "");
}

void ErrorReporter::report_error()
{
	int lns[3] = { -1, -1, -1 };
	int count = 0;
	for (int i = 0; i < (int)this->errors.size() - 1; i++)
	{
		for (int j = i + 1; j < this->errors.size(); j++)
		{
			if (errors[i].lino > errors[j].lino)
			{
				ErrorDescripter tmp = errors[i];
				errors[i] = errors[j];
				errors[j] = tmp;
			}
		}
	}
	ofstream fout("error.txt");
	if (!fout.is_open())
	{
		cerr << "Open `error.txt` failed." << endl;
		return;
	}
	for (int i = 0; i < this->errors.size(); i++)
	{
		int ln = errors[i].lino;
		Errno merrno = errors[i].merrno;
		if (lns[0] != ln && lns[1] != ln && lns[2] != ln)
		{
			lns[count] = ln;
			count++;
			fout << ln << " " << merrno << endl;
		}
		if (count >= 3)
		{
			break;
		}
	}
	fout.close();
}

void ErrorReporter::print_errors()
{
	if (this->errors.empty())
	{
		cout << "[AFTER] Analysis completed without errors." << endl;
	}
	else
	{
		cout << "[AFTER] Analysis completed, " << this->errors.size() << " errors found." << endl;
		for (int i = 0; i < errors.size(); i++)
		{
			int ln = errors[i].lino;
			Errno merrno = errors[i].merrno;
			string message = errors[i].message;
			cout << "[AFTER]  - " << ln << " " << merrno << ": " << message << endl;
		}
	}
}

void ErrorReporter::add_errors(vector<ErrorDescripter> errors)
{
	for (int i = 0; i < errors.size(); i++)
	{
		this->errors.push_back(errors[i]);
	}
}

vector<ErrorDescripter> ErrorReporter::get_errors()
{
	return this->errors;
}
