#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include "pseudo.h"

class Optimizer
{
private:
	int find_paired_loop(vector<Pseudo>*, int);

public:
	vector<Pseudo> modify_assign(vector<Pseudo>);
	vector<Pseudo> loop_const(vector<Pseudo>);
};

#endif // !OPTIMIZE_H
