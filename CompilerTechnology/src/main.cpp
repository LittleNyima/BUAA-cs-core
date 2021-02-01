#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "lex.h"
#include "grammar.h"
#include "merror.h"
#include "assembly.h"
#include "optimize.h"

using namespace std;

int main(int argc, char* argv[])
{
	ifstream fin("testfile.txt");
	char buffer[256] = { '\0' };
	string source = "";
	if (fin.is_open())
	{
		while (!fin.eof())
		{
			fin.read(buffer, 255);
			source += buffer;
			memset(buffer, 0, 256 * sizeof(char));
		}
	}
	string _source = "";
	for (int i = 0; i < source.size(); i++)
	{
		if (source[i] == '\r') _source += " \r";
		else if (source[i] == '\n') _source += " \n";
		else _source += source[i];
	}
	Lex lex;
	lex.parse(_source);
	Grammar grammar;
	//grammar.debug_mode();
	grammar.parse(lex.get_tokens());
#ifdef ENABLE_ERRORPROC
	ErrorReporter reporter;
	reporter.add_errors(lex.get_errors());
	reporter.add_errors(grammar.get_errors());
	reporter.print_errors(); // debug mode
	reporter.report_error();
#endif // ENABLE_ERRORPROC
	GTree gtree = grammar.build_tree();
	//SimpleGTree gtree = grammar.build_simple_tree();
	gtree.submit_mode();
	gtree.debug_mode();
	//gtree.print_tree();
	gtree.analyze();
	//gtree.print_tree();
	//gtree.interpret_run();
	//gtree.assembly();
	Optimizer opt;
	vector<Pseudo> pcodes = gtree.get_pcodes();
#ifdef OPTIMIZE_3
	pcodes = opt.modify_assign(pcodes);
#endif // OPTIMIZE_3
	for (int i = 0; i < pcodes.size(); i++) cout << pcodes[i].to_string() << endl;
	SimpleAssembler sa;
	sa.debug_mode();
	sa.assemble(pcodes, gtree.get_symtable());
	return 0;
}
