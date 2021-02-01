#ifndef PSEUDO_H
#define PSEUDO_H

#include <string>
#include <utility>
#include <vector>

using namespace std;

// type: LINT, LCHAR, VINT, VCHAR
#define P_ADD  "ADD"   // ADD <name, type>, <name, type>, r
#define P_SUB  "SUB"   // SUB <name, type>, <name, type>, r
#define P_MUL  "MUL"   // MUL <name, type>, <name, type>, r
#define P_DIV  "DIV"   // DIV <name, type>, <name, type>, r
#define P_ARR  "ARR"   // ARR name, <name, type>, r
#define P_FUNC "@FUNC" // @FUNC <name, retype>, {<name, type>}
#define P_LOOP "@LOOP" // @LOOP <name, type>, rel, <name, type>
#define P_IF   "@IF"   // @IF <name, type>, rel, <name, type>
#define P_RET  "RET"   // RET <name, type>
#define P_RETN "RETN"
#define P_CALL "CALL"  // CALL name, {<name, type>}, r
#define P_CALN "CALN"  // CALN name, {<name, type>}
#define P_ASSN "ASSN"  // ASSN <name, type>, r
#define P_ASSR "ASSR"  // ASSN <name, type>, r, <name, type>
// type: INT, CHAR
#define P_READ "READ"  // READ <name, type>
// type: LINT, LCHAR, VINT, VCHAR, STR
#define P_DISP "DISP"  // DISP <name, type>, endl
// plain text, no actual effect
#define P_COMN "COMN"  // COMN text

// mark
#define P_STARTLOOP "@STARTLOOP"
#define P_ENDFUNC   "@ENDFUNC"
#define P_ENDLOOP   "@ENDLOOP"
#define P_ELSE      "@ELSE"
#define P_ENDIF     "@ENDIF"

#define P_LINT     "LINT"
#define P_LCHAR    "LCHAR"
#define P_VINT     "VINT"
#define P_VCHAR    "VCHAR"
#define READ_INT   "INT"
#define READ_CHAR  "CHAR"
#define DISP_LINT  "LINT"
#define DISP_LCHAR "LCHAR"
#define DISP_VINT  "VINT"
#define DISP_VCHAR "VCHAR"
#define DISP_STR   "STR"
#define DISP_ENDL  "ENDL"
#define DISP_END   "END"

class Pseudo
{
private:
	string type; // type of pseudo code

	// ADD, SUB, MUL, DIV, @LOOP, @IF
	pair<string, string> opr1; // ASSN, ASSR
	pair<string, string> opr2;
	// ADD, SUB, MUL, DIV, CALL, ARR, ASSN, ASSR
	string result;
	// @LOOP, @IF
	string rel;
	// ARR, CALL, CALN
	string name;
	// ARR
	pair<string, string> ref;
	// FUNC
	pair<string, string> header;
	// READ, DISP
	pair<string, string> io;
	// RET
	pair<string, string> ret;
	// FUNC, CALL, CALN
	vector<pair<string, string>> args;
	// DISP
	string endl;
	// COMN
	string text;
	// ASSR
	pair<string, string> offset;

	vector<string> use_names;
	string update_name;

	string func_to_string();
	string call_to_string();
	string caln_to_string();

public:
	Pseudo(string, pair<string, string>, pair<string, string>, string); // ADD, SUB, MUL, DIV
	Pseudo(string, string, pair<string, string>, string); // ARR
	Pseudo(string, pair<string, string>, vector<pair<string, string>>); // @FUNC
	Pseudo(string, pair<string, string>, string, pair<string, string>); // @LOOP, @IF, ASSR
	Pseudo(string, pair<string, string>); // RET, READ
	Pseudo(string, string, vector<pair<string, string>>, string); // CALL
	Pseudo(string, string, vector<pair<string, string>>); // CALN
	Pseudo(string, pair<string, string>, string); // DISP, ASSN
	Pseudo(string, string); // COMN
	Pseudo(string); // RETN, MARKS
	Pseudo(Pseudo*, string); // modify assign
	string get_type();
	pair<string, string> get_opr1();
	pair<string, string> get_opr2();
	string get_result();
	string get_rel();
	string get_name();
	pair<string, string> get_ref();
	pair<string, string> get_header();
	pair<string, string> get_io();
	pair<string, string> get_ret();
	vector<pair<string, string>> get_args();
	string get_endl();
	pair<string, string> get_offset();
	string to_string();
	bool use(string);
	bool update(string);
	string get_update();
	vector<string> get_use();
};

#endif // !PSEUDO_H
